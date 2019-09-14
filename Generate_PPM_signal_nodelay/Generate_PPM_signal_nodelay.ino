#include "pins_arduino.h"

//////////////////////CONFIGURATION///////////////////////////////
#define chanel_number 4  //set the number of chanels
#define default_servo_value 1500  //set the default servo value
#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 9  //set PPM signal output pin on the arduino
#define bl0_pin 5
#define bl1_pin 6
//////////////////////////////////////////////////////////////////


/*this array holds the servo values for the ppm signal
  change these values in your code (usually servo values move between 1000 and 2000)*/
int ppm[chanel_number];
int bldc_value[2] = {0, 0};


char buf [100];
volatile byte pos;
volatile boolean process_it;

void setup() {
  Serial.begin(115200);           // start serial for output

  //initiallize default ppm values
  for (int i = 0; i < chanel_number; i++) {
    ppm[i] = default_servo_value;
  }
  pinMode(sigPin, OUTPUT);
  pinMode(bl0_pin, OUTPUT);
  pinMode(bl1_pin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)

  cli();

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  // turn on interrupts
  SPCR |= _BV(SPIE);
  pos = 0;
  process_it = false;




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
  /*
    for (int i = 0; i < 2; i++) {
    Serial.print(bldc_value[i]);
    Serial.print(" ");
    }
    Serial.println("");

    analogWrite(bl0_pin, map(bldc_value[0], 0, 100, 0, 255));
    analogWrite(bl1_pin, map(bldc_value[1], 0, 100, 0, 255));
  */
  if (process_it)
  {
    buf [pos] = 0;
    ///////////for ppm//////////////
    for (int i = 0; i < 4; i++) {
      String string = "";
      for (int j = 0; j < 4; j++) {
        string += buf[4 * i + j];

      }
      if (i==2){
        ppm[3] = string.toInt();
      }
      else if(i==3){
        ppm[2] = string.toInt();
      }
      else{
        
      }
    }
    //////////////////////////////
    //    Serial.println (buf);
    pos = 0;
    process_it = false;
  }  // end
}

ISR (SPI_STC_vect)
{

  byte c = SPDR;
  // add to buffer if room
  if (pos < sizeof buf)
  {
    buf [pos++] = c;

    // example: newline means time to process buffer
    if (c == '\n')
      process_it = true;

  }  // end of room available
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
