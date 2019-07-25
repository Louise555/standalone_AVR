/*
 * from: AVR programming Elliot Williams 
bossButton.c builds on the previous code with push button. 
the push button is connected to PD2 and the LED to PB0
works with a python program bossButton.py
the program send a '0' first. The '0 is caught by the python program.
if you press the button the LED blinks and a 'X' is sent to python.
python open a website in reponse of the 'X'/ 
Upload this code to your AVR, run bossButton.py.
Press button.
*/

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"    
#include "USART.h"

// this is the blinking function
static inline void blinkLED(void) {
  LED_PORT = (1 << LED0); // see pinDefines.h . LED_PORT is PORTB LED0 is  PB0 (digital 8 of arduino pin 14 of the chip)
  _delay_ms(1000);
  LED_PORT &= ~(1 << LED0); // turn off . 
}


int main(void) {

  // -------- Inits --------- //
  // BUTTON_PORT is PORT D 
  BUTTON_PORT |= (1 << BUTTON);          /* input mode, turn on pullup. The button is at pin PD2 of the avr */

  LED_DDR = (1 << LED0); // the pin PB0 (or LED0) is an output pin. 
  blinkLED();

  initUSART(); // starting the serial communication   from USART.h
  // below we send a byte to the PC through the Tx/Rx lines. python "catches" the letter. 
  transmitByte('O'); // tell python we are ready so it wont shut down after 3 seconds. giving up on listening to data. 

  

  // ------ Event loop ------ //
  while (1) {

    if (bit_is_clear(BUTTON_PIN, BUTTON)) {  //macro to check if the bit is 0. if it is 0 the button is pushed. 
      transmitByte('X');  // tell python to open a web page. 'X' is sent to the PC. 
      blinkLED();
    }

  }                                                  /* End event loop */
  return 0;
}
