/*

Demo of using interrupts for doing what they do best --
two things at once.

Flashes LED0 (PB0) at a fixed rate (main function), interrupting whenever button is pressed. When it is pressed the interrupt call a handler
and LED1 (PB1)  is turned on or off. 
It uses a button attached to pin PD2 (digital 2 of arduino)  Which is INT0 ( attached to external interrupt) or pin 4 of the chip.  .
It uses a LED attached to PB1 (callled pin LED1 see the header pindefines.h )
and a LED attached to PB0 (called pin LED0) 
The button is attached to PD0 and to ground. When the button is not pressed PD2 is HIGH because there is a pull-up resistor (input mode with PORT =1)
and when the button is pressed PD2 is 0. So voltage fails or rises trigeering the interrupt. 

 */

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "pinDefines.h"

ISR(INT0_vect) {         /* Run every time there is a change on button. this is the interruupt service routine. The routine is run when the button is pushed
  or released. that is when there is a change in voltage.  INT0_vect is a memory location at which the address of the ISR can be found.
  It is called an interrupt vector. That points to an interrupt handler. This code ISR is saved at that location.*/
  if (bit_is_clear(BUTTON_PIN, BUTTON)) {  // This is a macro by AVR. BUTTON_PIN is defined in pinDefines.h it means PIND (to read the pin_
    // BUTTON is PD2. If PD2 goes from 5V to  0 then the button is pressed and the LED1 (PB1) is turned on. If the button is released the LED1 is turned off
      
    LED_PORT |= (1 << LED1); // LED_PORT is PORTB
  }
  else {
    LED_PORT &= ~(1 << LED1); // else toggle
  }
}
// tuning the registers
void initInterrupt0(void) {
  EIMSK |= (1 << INT0);             /* enable INT0 which is coneected to pin PD2. The CPU will check the flag for INTO to see if it is on regularly */
  EICRA |= (1 << ISC00);                /* trigger when button changes. we can trigger the interrupt also when voltage rises or falls */
  sei();                          /* set (global) interrupt enable bit. This is important. 
                             When the flag for INT0 is switched on , then the global flag is tuned off. After the routine ISR is run it 
                             so the uC can only execute one interrupt at a time only following some piorities */
}

int main(void) {
  // -------- Inits --------- //
  LED_DDR = 0xff;                                   /* all LEDs active. which is PORT B. so all the pins of PORT B are active. output mode  */
  BUTTON_PORT |= (1 << BUTTON);                       /* By default the button (PD2) is in input. If the pin 0 of register PORTD is 1 that means
                                                    we activate the pullup resistance for the button. When not pressed PD2 is HIGH. When pressed it is LOW */
  initInterrupt0(); // setting for the interrupt. 

  // ------ Event loop ------ //
  while (1) {

    _delay_ms(200);
    LED_PORT ^= (1 << LED0); // blink the LED0 in the main function

  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
