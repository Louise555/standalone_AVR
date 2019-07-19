/*
 * reference: Make: AVR Programming - Elliot Williams
 * you need to add in your library folder AVR-Programming-Library from https://github.com/hexagon5un/AVR-Programming
 * The sketch is uploaded in the standalone uC with an arduino used as a programmer using the SPI protocole. 
 * 
 */
#include <binaryMacro.h>
#include <macros.h>
#include <pinDefines.h>
#include <portpins.h>
#include <USART.h>                                                       /* Blinker Demo */

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */


int main(void) {

  // -------- Inits --------- //
  DDRB |= 0b00000001;            /* Data Direction Register B:
                                   writing a one to the bit
                                   enables output. */

  // ------ Event loop ------ //
  while (1) {

    PORTB = 0b00100001;          /* Turn on first LED bit/pin in PORTB */
    _delay_ms(100);                                           /* wait */

    PORTB = 0b00000000;          /* Turn off all B pins, including LED */
    _delay_ms(100);                                           /* wait */

  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
