// Quick and dirty adjustable-threshold night-light.
// this is to demonstrate how the multiplexer works. A multi way switch. The ADC reads alternatively channels PC0/ADC0 (A0 of arduino) and PC3/ADC3 (A3 of arduino. 
// PC0 is reading the light intensity (voltage across 33K in a voltage divider 5V- LDR-33K  - Gnd. see previous set up. 
// PC3 reads the voltage from a pot Its not free-running. We laternate between the channels. (the hardware uses a multiplexer)
// Then we can adjust the pot such as the LED turns on when we put the finger in front of the LDR.
// you need a pot 10K. The wiper (middle pin)  is connected to PC3 or ADC3. The LDR is connected to PC0 or ADC0. 

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"

uint16_t readADC(uint8_t channel) {
  ADMUX = (0xf0 & ADMUX) | channel; 
  // In the first part 0xf0=0b11110000 so we erase the  4 first bits with the " AND". Then we enable the channel we want to read from. 
  // the four least significant bits define the channels we are reading from 0000 is for channel 0. 0001 is for channel 1. 0010 for channel 2
  // 0011 for channel 3 etc.. Here we are only reading from channel 0 and channel3. 
  // if the channel is 0 that means PC0 is reading. If channel = 3 or 0011  for the least significant bits, that means the channel 3 is reading.
  // see datasheet. The first bits of the register ADMUX define the channel we are reading from. 
  ADCSRA |= (1 << ADSC);            // start first conversion. 
  loop_until_bit_is_clear(ADCSRA, ADSC); // wait for conversion and returns a 10 bits number.
  return (ADC);
}

int main(void) {
  // -------- Inits --------- //
  uint16_t lightThreshold; // we have to find this value turning the pot. The LED should lit when the finger is on the LDR. 
  uint16_t sensorValue;
  // Set up ADC
  ADMUX |= (1 << REFS0);                  /* reference voltage on AVCC */
  ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);     /* ADC clock prescaler 128 */
  ADCSRA |= (1 << ADEN);                                 /* enable ADC */

  LED_DDR = 0xff; // You can have  8 LED attached to port B . Or you can have just one. This is an output direction. 
  // ------ Event loop ------ //
  while (1) {

    lightThreshold = readADC(POT); // reads the POT at PC3  see the pinDefines.h POT means PC3 or ADC3 . 
    sensorValue = readADC(LIGHT_SENSOR); // LIGHT_SENSOR means PC0 or ADC0.

    if (sensorValue < lightThreshold) {
      LED_PORT = 0xff; // we lit the LEDs
    }
    else {
      LED_PORT = 0x00; // we turn off LEDs
    }
  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
