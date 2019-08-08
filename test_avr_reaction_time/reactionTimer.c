/*
 * to compute the reaction time using timer1 (16bits)in ms. 
 * connect the button to PD2  PD2 -> button -> Gnd and LEDs to PORT B (like pb0)
   Press the button as quickly as you can after the LEDs light up.
   Your time is printed out over the serial port.
   We use timer 1 and we suppose a reaction time smaller than 4s
   This code is to see how the timer 1 works. It is a 16 bits timer so it counts from 0 to 65535 . The CPU clock is 16MHz but we use a 1028 prescaler. (cs10 =1 and cs12=1
   in the register for timer1 TCCR1B . so the frequency is 15,564Hz or 64 us between each tick. 64us is about 1/16 ms = 62.5 us . This is an approximation.
   So there is a tick of the counter every (1/16) ms. So the max time is 4s. That's enough for a reaction time. 
   The value of the counter is stored in a register TCNT1 . So the reaction time is stored in TCNT1 in units of 1/16 ms
   Then it is right shifted by 4 TCNT1>>4 which is like dividing the number by 16. The results is in units of ms
   
   When we enter a letter in the monitor the LED will lit after a random time and we need to hit the button. 
   The counter counts how long it takes to hit the button which is the reaction time. 
   Some functions are defined in support.c like the random generator. */

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "pinDefines.h"
#include "USART.h"

#include "support.h"

static inline void initTimer1(void) {
  // Normal mode (default), just counting */
  TCCR1B |= (1 << CS10) | (1 << CS12); // prescaler is 1028 
  // Clock speed: 16 MHz / 1028 - the time between each tick is about 1/16 ms

}

int main(void) {
  uint16_t timerValue;

  // -------- Inits --------- //

  initUSART(); // for serial communication
  initTimer1(); // initialisation of the timer
  LED_DDR = 0xff;    /* all LEDs for output. LED_DDR means  DDRB - LEDs on PORT B */
  BUTTON_PORT |= (1 << BUTTON);             /* The button is at PD2. enable internal pull-up.  */

  printString("\r\nReaction Timer:\r\n");
  printString("---------------\r\n");
  printString("Press any key to start.\r\n");

  // ------ Event loop ------ //
  while (1) {

    receiveByte();                                    /* press any key */
    printString("\r\nGet ready...");
    randomDelay(); // this function is in support.c . we use the counter itself to generate a time between 1s and 3.5s. 

    printString("\r\nGo!\r\n");
    LED_PORT = 0xff;                                     /* light LEDs */
    TCNT1 = 0;                                        /* reset counter . the register TCNT1 has the value of the counter */

    if (bit_is_clear(BUTTON_PIN, BUTTON)) {
      /* Button pressed _exactly_ as LEDs light up.  Suspicious. */
      printString("You're only cheating yourself.\r\n");
    }
    else {
      // Wait until button pressed, save timer value.
      loop_until_bit_is_clear(BUTTON_PIN, BUTTON); // we wait for button to be pressed. This is a macro from io.h . BUTTON is PD2 
      timerValue = TCNT1 >> 4; // this right shift the value so the number is divided by 16. so the result is in ms. 
                               // if the counter is 32 that means 32 x (1/16) ms = 2 ms. So if we divide by 16 then we get 2 ms. 
      printString("counter \r\n");
      printWord(TCNT1);
      printString("\r\n");

      /* each tick is approx 1/16 milliseconds, so we bit-shift divide */

      printMilliseconds(timerValue); // see support.c
      printComments(timerValue); // see support.c
      

    }

    // Clear LEDs and start again.
    LED_PORT = 0x00;
    printString("Press any key to try again.\r\n");

  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
