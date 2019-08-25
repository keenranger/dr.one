
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "RTFusionRTQF.h"
#include "CalLib.h"
#include <EEPROM.h>

/// PWM PINS
// DC모터 정면오른쪽부터 시계 방향 a-b-c-d
#define DC_A_PINNUM 6        // front-right
#define DC_B_PINNUM 9        // rear-right
#define DC_C_PINNUM 10       // rear-left
#define DC_D_PINNUM 11       // front-left
#define BLDC_TOP_PINNUM 3    // top
#define BLDC_BOTTOM_PINNUM 5 // bottom

/// DC DRIVER PINS
#define DC_DIRECTION 4
#define DC_EN        12

/// COMMUNICATION PINS
#define JOYSTICK_RX_PINNUM 8 // 
#define JOYSTICK_TX_PINNUM 7 // 사용 X, 임시로
#define DISPLAY_INTERVAL  50 // 

typedef struct Pid {
  float diffAngle;
  float angle;
  float preAngle;
  int control;
  float kp;
  float ki;
  float kd;
  float p;
  float i;
  float d;
  float err;
  float err_prev;
  float de;
  float dt;
  float final;
} PID;

RTIMU *imu;
RTFusionRTQF fusion;
RTIMUSettings settings;

SoftwareSerial joystick { JOYSTICK_RX_PINNUM, JOYSTICK_TX_PINNUM };
Servo bl0;
Servo bl1;
PID yaw = { 0, 0, 0, 0, 1, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
PID roll = { 0, 0, 0, 0, 5, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
PID pitch = { 0, 0, 0, 0, 5, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// buf1[Aileron], Buf2[Elevator], buf3[Throttle], buf4[Rudder], buf5[Checksum], 나머지는 사용x
int buf[8] = {0, 129, 129, 0, 129, 0, 0, 0}; // 카메라보드로부터 읽어들일 8개의 변수
int Lastbuf[8] = {0, 129, 129, 0, 129, 0, 0,0}; // 카메라보드로부터 읽어들 8개의 변수의 이전 값
boolean Loop = true; // Control_UART_Update가 실행될 때 if문으로 딱 한 번만 처리해주기위한 변수

int bldc_Control = 0;
float DcA_setValue = 0;
float DcB_setValue = 0;
float DcC_setValue = 0;
float DcD_setValue = 0;
float bl0_setValue = 0;
float bl1_setValue = 0;

unsigned long lastDisplay;
unsigned long lastRate;
int sampleCount;
unsigned long now;
unsigned long nowJoystick = 0;

void setup() {
  Wire.begin();
  Serial.begin(19200);
  joystick.begin(19200);

  int errcode;
  imu = RTIMU::createIMU(&settings);
  
  Serial.print("ArduinoIMU starting using device "); Serial.println(imu->IMUName());
  if ((errcode = imu->IMUInit()) < 0) {
    Serial.print("Failed to init IMU: "); Serial.println(errcode);
  }

  if (imu->getCalibrationValid())
    Serial.println("Using compass calibration");
  else
    Serial.println("No valid compass calibration data");
  
  lastDisplay = lastRate = millis();
  sampleCount = 0;
  fusion.setSlerpPower(0.02);

  fusion.setGyroEnable(true);
  fusion.setAccelEnable(true);
  fusion.setCompassEnable(true);

  bl0.attach(BLDC_TOP_PINNUM, 1000, 2000);    // top
  bl1.attach(BLDC_BOTTOM_PINNUM, 1000, 2000); // bottom  
  bl0.writeMicroseconds(1000);
  bl1.writeMicroseconds(1000);

  pinMode(DC_A_PINNUM,OUTPUT);
  pinMode(DC_B_PINNUM,OUTPUT);
  pinMode(DC_C_PINNUM,OUTPUT);
  pinMode(DC_D_PINNUM,OUTPUT);
  pinMode(DC_DIRECTION,OUTPUT);
  pinMode(DC_EN,OUTPUT);
  digitalWrite(DC_A_PINNUM,LOW);
  digitalWrite(DC_B_PINNUM,LOW);
  digitalWrite(DC_C_PINNUM,LOW);
  digitalWrite(DC_D_PINNUM,LOW);
  digitalWrite(DC_DIRECTION,HIGH);
  digitalWrite(DC_EN,HIGH);
    
  delay(1000);
}

int test = 1000;

void loop() {
  
  analogWrite(DC_A_PINNUM,200+(int)DcA_setValue);
  analogWrite(DC_B_PINNUM,200+(int)DcB_setValue);
  analogWrite(DC_C_PINNUM,200+(int)DcC_setValue);
  analogWrite(DC_D_PINNUM,200+(int)DcD_setValue);  
  
  readIMU();
  readJoystick();

  pitch.control = map(buf[2], 88, 168, -30, 30);        //  pitch 조이스틱 데이터
  roll.control = map(buf[1], 88, 168, -30, 30);         //  roll 조이스틱 데이터
  yaw.control = map(buf[4], 80, 176., -45, 45) + 10;    //   yaw 조이스틱 데이터
  bldc_Control = map(buf[3], 0, 255, 0, 875);           //   throttle 조이스틱 데이터
  yaw.diffAngle = yaw.angle - yaw.preAngle;
  
  setPidBLDC(&yaw);
  //setPidRoll(&roll);
  //setPidPitch(&pitch);
  
  yaw.preAngle = yaw.angle;
  
  if (bldc_Control == 0) {
    bl0.writeMicroseconds(1000);
    bl1.writeMicroseconds(1000);
    
    pitch.i = 0;
    pitch.final = 0;
    roll.i = 0;
    roll.final = 0;
    yaw.i = 0;
    yaw.final = 0;
    
    DcA_setValue = 0;
    DcB_setValue = 0;
    DcC_setValue = 0;
    DcD_setValue = 0;
    bl0_setValue = 0;
    bl1_setValue = 0;
  }
  else {
    if (yaw.final > 0) {
      bl0_setValue = -fabs(yaw.final) / 2.;
      bl1_setValue = +fabs(yaw.final) / 2.;
    }
    else {
      bl0_setValue = +fabs(yaw.final) / 2.;
      bl1_setValue = -fabs(yaw.final) / 2.;
    }
    
    bl0_setValue += bldc_Control;
    bl1_setValue += bldc_Control;

    bl0_setValue = constrain(bl0_setValue, 0, 900);
    bl1_setValue = constrain(bl1_setValue, 0, 900);

    bl0.writeMicroseconds(1100 + bl0_setValue);
    bl1.writeMicroseconds(1100 + bl1_setValue);
  }
  //dc모터 제어부분 비활성화
  /*
  if(pitch.final>0){
    DcA_setValue -= pitch.final;
    DcB_setValue += pitch.final;
    DcC_setValue += pitch.final;
    DcD_setValue -= pitch.final;
  }
  else{
    DcA_setValue += pitch.final;
    DcB_setValue -= pitch.final;
    DcC_setValue -= pitch.final;
    DcD_setValue += pitch.final;
  }
  if(roll.final>0){
    DcA_setValue += roll.final;
    DcB_setValue += roll.final;
    DcC_setValue -= roll.final;
    DcD_setValue -= roll.final;
  }
  else{
    DcA_setValue -= roll.final;
    DcB_setValue -= roll.final;
    DcC_setValue += roll.final;
    DcD_setValue += roll.final;      
  }
  
  DcA_setValue = constrain(DcA_setValue,-20,20);
  DcB_setValue = constrain(DcB_setValue,-20,20);
  DcC_setValue = constrain(DcC_setValue,-20,20);
  DcD_setValue = constrain(DcD_setValue,-20,20);

  DcA_setValue = DcA_setValue - pitch.control + roll.control;
  DcB_setValue = DcB_setValue + pitch.control + roll.control;
  DcC_setValue = DcC_setValue + pitch.control - roll.control;
  DcD_setValue = DcD_setValue - pitch.control - roll.control;

  DcA_setValue = constrain(DcA_setValue, -50, 50);
  DcB_setValue = constrain(DcB_setValue, -50, 50);
  DcC_setValue = constrain(DcC_setValue, -50, 50);
  DcD_setValue = constrain(DcD_setValue, -50, 50);

  //displaySerial();
  */
}

void readJoystick() {
  if ( joystick.available() < 1) {
    
    if (millis() - nowJoystick > 3000) {
      buf[1] = 129;
      buf[2] = 129;
      buf[3] = 0;
      buf[4] = 129;
    } 
    
  }
  else {
    nowJoystick = millis();
  }
  // 카메라보드로부터 수신될 때 실행되는 while문
  while (joystick.available()) {
    for (int j = 0; j < 8; j++) {
      buf[j] = joystick.read();
    }
    if (buf[5] != 0) { // checksum의 값이 0과 다를 때 Goto
      for (int i = 0; i < 8; i++) {
        //if (i == 5)
        //  continue;
        buf[i] = Lastbuf[i];
      }
    }
    
    if (Loop) { // 48번줄을 위한 용도
      for (int i = 0; i < 8; i++) {
        //if (i == 5)
        //  continue;
        Lastbuf[i] = buf[i];
      }
      Loop = false;
    }
    
    // buf값이 송신측에서 규정한 값의 범위를 벗어날경우 이전의 buf값을 현재 buf값에 대입

    if ((buf[1] > 168) || (buf[1] < 88))
      buf[1] = Lastbuf[1];
    if ((buf[2] > 168) || (buf[2] < 88))
      buf[2] = Lastbuf[2];
    if ((buf[3] > 255) || (buf[3] < 0))
      buf[3] = Lastbuf[3];
    if ((buf[4] > 176) || (buf[4] < 80))
      buf[4] = Lastbuf[4];

  }

  if (!Loop) {
    for (int i = 0; i < 8; i++) {
      //if (i == 5)
      //  continue;
      Lastbuf[i] = buf[i];
    }
  }
}

//IMU 읽기 제거
/*
void readIMU() {
  int loopCount = 1;
  unsigned long delta;
  now = millis();
  while (imu->IMURead()) {
    if (++loopCount >= 10)
      continue;
    fusion.newIMUData(imu->getGyro(), imu->getAccel(), imu->getCompass(), imu->getTimestamp());
    
    sampleCount++;
    if ((delta = now - lastRate) >= 1000) {
      //Serial.print("Sample rate: "); Serial.print(sampleCount);
      if (imu->IMUGyroBiasValid());
        Serial.println(", gyro bias valid");    
      //else
        //Serial.println(", calculating gyro bias");
    
      sampleCount = 0;
      lastRate = now;
    }
    
    RTVector3& vec = (RTVector3&)fusion.getFusionPose();
    roll.angle = vec.x() * RTMATH_RAD_TO_DEGREE;
    pitch.angle = vec.y() * RTMATH_RAD_TO_DEGREE;
    yaw.angle = vec.z() * RTMATH_RAD_TO_DEGREE;
  }
}
*/

void setPidBLDC(PID * pid) {
  pid->err = yaw.control - yaw.diffAngle;
  pid->dt = 0.001;
  pid->p = pid->err * pid->kp;
  pid->i = pid->i + pid->err * pid->dt * pid->ki;
  pid->i = constrain(pid->i, -10, 10);
  pid->final = pid->p + pid->i;
  pid->final = constrain(pid->final, -10, 10);
}
// 롤 pid부분
/*
void setPidRoll(PID * pid) {
  pid->err = roll.angle;
  pid->dt = 0.001;
  pid->p = pid->err * pid->kp;
  pid->i = pid->i + pid->err * pid->dt * pid->ki;
  pid->i = constrain(pid->i, -10, 10);
  pid->final = pid->p + pid->i;
  pid->final = constrain(pid->final, -10, 10);
}*/

// 피치 pid부분
/*
void setPidPitch(PID * pid) {
  pid->err = pitch.angle;
  pid->dt = 0.001;
  pid->p = pid->err * pid->kp;
  pid->i = pid->i + pid->err * pid->dt * pid->ki;
  pid->i = constrain(pid->i, -10, 10);
  pid->final = pid->p + pid->i;
  pid->final = constrain(pid->final, -10, 10);
}*/

// dc출력값
/*
void displaySerial(){
    Serial.print(analogRead(DC_A_PINNUM));
  Serial.print("\t");
  Serial.print(analogRead(DC_B_PINNUM));
  Serial.print("\t");
  Serial.print(analogRead(DC_C_PINNUM));
  Serial.print("\t");
  Serial.println(analogRead(DC_D_PINNUM));
}*/
