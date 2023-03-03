#include <SPI.h>
#include <Adafruit_DotStar.h>
#include <stdint.h>

#include "america.h"


// * Defining out LED strip object and its parameters.

#define NUMLED 132
#define BRIGHTNESS 25

Adafruit_DotStar strip = Adafruit_DotStar(NUMLED, DOTSTAR_BRG);

// #define DATAPIN 11
// #define CLOCKPIN 13
// Adafruit_DotStar strip = Adafruit_DotStar(NUMLED, DATAPIN, CLOCKPIN, DOTSTAR_BRG);


// * Our image dimensions
// * IMG_W can be changed while IMG_H is based on the number of physical
// * LEDs that we have. 

#define IMG_H 66
#define IMG_W 144

// => assert NUMLED/2 == IMG_H

const int position_pulse_pin = 4;
const int motor_control_pin = 2;

volatile byte position_counter = 0;

int set_duty;

int starting_duty = 300;
const int target_RPM = 700;


// * Set the pixel colours of the arm from pixelColumn
// * Pixel column should be of size IMG_H

void setColumnSide(uint32_t pixelColumn [], bool front){
  
  int ledOffset = NUMLED/2;

  for (int led = 0; led < IMG_H; led++){

    uint32_t color = pixelColumn[led];

    if(front){
      strip.setPixelColor(ledOffset - led, color);
    } else {
      strip.setPixelColor(ledOffset + led, color);
    }
  }
  
}

void allColours(uint32_t colour){
  uint32_t fullColour[NUMLED/2] = {colour};
  setColumnSide(fullColour, true);
  setColumnSide(fullColour, false);
}

void halfColours(uint32_t colour){
  uint32_t fullColour[NUMLED/2] = {colour};
  setColumnSide(fullColour, true);
}


// * Increment position counter

void incPosition() {
  position_counter = (position_counter + 1) % IMG_W;
}

void setup() {  

  // * Initalise pin modes and PWM output

  pinMode(motor_control_pin, OUTPUT);
  pinMode(position_pulse_pin, INPUT);

  analogWriteResolution(12); // [0, 4095]
  analogWriteFrequency(motor_control_pin, 1000);

  // * Starting LED colours
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  allColours(0x00FFFF);
  strip.show();

  set_duty = 718; // ~700 RPM;

  allColours(0x000000);
  strip.show();

  delay(1000);
}


bool showing_led = false;
bool prev_state = LOW;

void loop() {
  
  // * RISING || FALLING EDGE
  if ( (digitalRead(position_pulse_pin) == HIGH && prev_state == LOW) ||
       (digitalRead(position_pulse_pin) == LOW && prev_state == HIGH)){
    
    incPosition();
    
    prev_state = (prev_state + 1) % 2;
    showing_led = false;
  }
  
  if (showing_led == false){
    setColumnSide(pixelsArray[position_counter], true);
    setColumnSide(pixelsArray[(position_counter + IMG_W/2) % IMG_W], false);
    strip.show();
    showing_led = true;
  }


  // * Write the PWM for the motor speed
  analogWrite(motor_control_pin, set_duty);
}
