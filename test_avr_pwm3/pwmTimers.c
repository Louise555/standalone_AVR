
/*
 * see sine_wave .  Using the PWM we create a sine wave. However, the sinewave can not be seen by an oscilloscope because it sees
 * the pulses instead of averaging the cycles to get the analog value. It is too fast. The sine wave can be fed into a LED for example.
 * The LED would see the pulse but not our eyes that averages things out. We can add a low pass filter to remove the pulses
 * and see the sine wave with an oscilloscope. 
 * if we take 100 samples and if there is 50us between each sample then
 * 50us x 100 = 0.005s -> 200Hz. 
 *= 
 */

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include "pinDefines.h"
#include "USART.h"


static inline void initTimers(void) {
  // Timer 1 A,B
  TCCR1A |= (1 << WGM10);                      /* I tried phase correct and it works. this is accoring to p.136 of datasheet. 
                                                   so it counts from 0 to 255 then 255 to 0*/
 // TCCR1B |= (1 << WGM12);                       /* uncomment this if you want fast PWM so just counting from 0 to 255  */
  TCCR1B |= (1 << CS11)|(1 << CS10);                     /* PWM Freq = F_CPU/64/510 if phase correct so 490Hz  */
  TCCR1A |= (1 << COM1A1);                      /* PWM output on OCR1A . So the timer 1 has 2 pins for PWM A and B. 
                                               A is PB1 which is digital 9 and PB2 which is digital 10 . if we want to decouple the pin from
                                                     PWM we need to set those bits to 0. The COM.. bits to 0. The brightness=0 does not work. */
  TCCR1A |= (1 << COM1B1);                      /* PWM output on OCR1B . second pin for PWM. Also. those bits mean that it is non inverting
                                                 see page 135 of datasheet*/

}

int main(void) {
uint8_t  sine[] = {127, 134, 142, 150, 158, 166, 173, 181, 188, 195, 201, 207, 213, 219, 224, 229, 234, 238, 241, 245, 247, 250, 251, 252, 253, 254, 253, 252, 251, 250, 247, 245, 241, 238, 234, 229, 224, 219, 213, 207, 201, 195, 188, 181, 173, 166, 158, 150, 142, 134, 127, 119, 111, 103, 95, 87, 80, 72, 65, 58, 52, 46, 40, 34, 29, 24, 19, 15, 12, 8, 6, 3, 2, 1, 0, 0, 0, 1, 2, 3, 6, 8, 12, 15, 19, 24, 29, 34, 40, 46, 52, 58, 65, 72, 80, 87, 95, 103, 111, 119,};
  uint8_t brightness;

  // -------- Inits --------- //

  initTimers();
  initUSART();
  printString("-- LED PWM Demo --\r\n");

               /* enable output on LED pins, triggered by PWM hardware */
  LED_DDR |= (1 << LED1);   // LED are output. if you want the pwm pins to be 0 we need to write 0 at those pins (PORT.. =0) bue also we need
                              // to decouple the pwm pins from the counter. so COM.. =0. See above. 
  LED_DDR |= (1 << LED2);
  LED_DDR |= (1 << LED3);

  // ------ Event loop ------ //
  while (1) {

    printString("\r\nEnter (0-255) for PWM duty cycle: ");
      for (int t=0; t<1000; t++){  //increment "t"
    OCR1A = sine[t];
    _delay_us(50); //wait 50us
      }
  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
