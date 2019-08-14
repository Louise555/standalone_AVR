/* Quick Demo of light sensor
* The voltage is divided. Vcc - LDR - 33K - Gnd/ The analog pin is connected between the LDR and the resistor. The pin is PC0 (A0 of arduino)
*select a prescaler of 128 because tiy use the frequency of 16Mhz.
* you  can add a 0.1F capacitor between Aref and gnd for decoupling.
* You can catch the serial data with a python program and the library pyserial. see  python.reading_values_serial.py
*for more on ADC http://maxembedded.com/2011/06/the-adc-of-the-avr/
*http://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-adc
*/
// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"

// -------- Functions --------- //
static inline void initADC0(void) {
  // register ADMUX ADC Multiplexer Selection Register has to be tuned so the reference voltage is 5V <-> 1023
  ADMUX |= (1 << REFS0);                  /* reference voltage on AVCC. They recommend to connect the AVcc to the 5V the voltge source. 
                                            But not to share the 5V with the uC */
  // ADCSRA – ADC Control and Status Register A
  ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);    /* ADC clock prescaler 128 . So the clock (16MHz is slowed down to 125KHz
                                                     the datasheet says the ADC operates within a frequency range of 50kHz to 200kHz
                                                     it takes 13 cycles to make a converion or 100us with a 125KHz clock. or 9600 Hz.*/
  ADCSRA |= (1 << ADEN);                                 /* this bit enables ADC */
}

int main(void) {
  initUSART(); // launch the serial communication see library USART.h
  printString("Hello World!\r\n");
  // -------- Inits --------- //
  uint8_t ledValue;
  uint16_t adcValue;
  uint8_t i;

  initADC0(); // we start the initialization the function is described above. 
  // optional: connect 8 LED to PORT B. The measurement (10-bits between 0 to 1023) will be converted to a 3 bits number (between 0 and 7) 
  // and that 3 bits number will be writen in digital form with the LEDS. You need to connext the 8 bits to PORT B. (PB0 to PB7)
  LED_DDR = 0xff; // 8  leds are attached to port B . The mode is output so the register DDRB is = 1 (see pinDefines.h)

  // ------ Event loop ------ //
  while (1) {

    ADCSRA |= (1 << ADSC);                     /* start ADC conversion */
    loop_until_bit_is_clear(ADCSRA, ADSC);          /* wait until done. when the conversion is over the bit ADSC is 0 */
    adcValue = ADC;                                     /* read ADC in . This is the 10 bits value */
                        /* Have 10 bits, want 3 (eight LEDs after all) */
 
   printWord(adcValue); // we write that value to the serial monitor. it can be also catched by a python program in the same folder. 
   
  //  printString("\r\n"); // skip lines. 
    ledValue=(adcValue>>7); // we turn that 10 bits value into a 3 bits value by keeping only the 3 highest bits. So from a valye between 0 to 1023 
                           // we keep a value between 0 and 7. 7 is 5V. So it is an approximation. 
    _delay_ms(2000);
  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
