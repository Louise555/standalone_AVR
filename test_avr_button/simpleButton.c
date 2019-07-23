/*
 * reference: AVR programming (Elliot Williams)
   Demo of the simplest on/off button code
   Button connected to PD2
   LEDs connected to PB0
   connect Gnd - push button - PD2
   connect Gnd - 330 ohms - LED - PB0
   When the button is pressed the LED is lit. 
*/

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
  // -------- Inits --------- //
  PORTD |= (1 << PD2);  /* initialize pullup resistor on our input pin. By default PB0 is an input. If if PD2 is 1 we activate the pull up resistor 10K */
  DDRB = 0xff;                           /* set up all LEDs for output */

  // ------ Event loop ------ //
  while (1) {
    if (bit_is_clear(PIND, PD2)) {            /* look for button press This is a macro defined in io.f */
                      /* equivalent to if ((PIND & (1 << PD2)) == 0 ){ */
      PORTB = 0b00000001;                                   /* pressed */
    }
    else {                                              /* not pressed */
      PORTB = 0x00;
    }
  }                                                  /* End event loop */
  return 0;
}
