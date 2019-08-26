#include <Wire.h>

//////////////////////CONFIGURATION///////////////////////////////
#define chanel_number 4  //set the number of chanels
#define default_servo_value 1500  //set the default servo value
#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 9  //set PPM signal output pin on the arduino
#define bl0_pin 10
#define bl1_pin 11
//////////////////////////////////////////////////////////////////


/*this array holds the servo values for the ppm signal
  change these values in your code (usually servo values move between 1000 and 2000)*/
int ppm[chanel_number];
int bldc_value[2] = {0, 0};
int i2c_buffer[4];
const int zero_buffer[4] = {0, 0, 0, 0};

void setup() {
  Wire.begin(0x04);                // join i2c bus with address 0x04
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  //initiallize default ppm values
  for (int i = 0; i < chanel_number; i++) {
    ppm[i] = default_servo_value;
  }
  pinMode(sigPin, OUTPUT);
  pinMode(bl0_pin, OUTPUT);
  pinMode(bl1_pin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)

  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;

  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
}

void loop() {
  //put main code here


  static int val = 1;
  for (int i = 0; i < 4; i++) {
    Serial.print(ppm[i]);
    Serial.print(" ");
  }
  Serial.println("");
  for (int i = 0; i < 2; i++) {
    Serial.print(bldc_value[i]);
    Serial.print(" ");
  }
  Serial.println("");

  analogWrite(bl0_pin, map(bldc_value[0], 0, 100, 0, 255));
  analogWrite(bl1_pin, map(bldc_value[1], 0, 100, 0, 255));
  delay(10);
}

ISR(TIMER1_COMPA_vect) { //leave this alone
  static boolean state = true;

  TCNT1 = 0;

  if (state) { //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PPM_PulseLen * 2;
    state = false;
  }
  else { //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;

    digitalWrite(sigPin, !onState);
    state = true;

    if (cur_chan_numb >= chanel_number) {
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;//
      OCR1A = (PPM_FrLen - calc_rest) * 2;
      calc_rest = 0;
    }
    else {
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }
  }
}

void receiveEvent(int howMany) {
  int command_cnt = 0;
  int bldc_cnt = 0;
  int buffer_cnt = 0;
  int on_flag = 0;
  while (0 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    if (on_flag == 0) {
      if (c == '/') {
        on_flag = 1;
      }
    }
    else {
      if (c == '/' ) {
        if (command_cnt < 4) {
          ppm[command_cnt] = 1000 * i2c_buffer[0] + 100 * i2c_buffer[1] + 10 * i2c_buffer[2] + i2c_buffer[3];
          command_cnt++;
          buffer_cnt = 0;
          memcpy(&i2c_buffer, &zero_buffer, sizeof(zero_buffer));
        }
        else {
          bldc_value[command_cnt - 4] = 1000 * i2c_buffer[0] + 100 * i2c_buffer[1] + 10 * i2c_buffer[2] + i2c_buffer[3];
          bldc_value[command_cnt - 4] -= 1000;
          command_cnt++;
          buffer_cnt = 0;
          memcpy(&i2c_buffer, &zero_buffer, sizeof(zero_buffer));
        }
      }
      else {//stack buffer
        i2c_buffer[buffer_cnt] = (int)c;
        buffer_cnt++;
      }
    }
  }
}
