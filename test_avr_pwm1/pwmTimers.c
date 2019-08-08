/* 
 *  PWM Demo I modified the code to just change the brightness (voltage) of 3 pins. 
 *  The mode is phase correct. the timer goes from 0 to 255 and to 0. so up and down the counter. See the datasheet. p.136 and p.160.
 *   We can also go back to  fast PWM (by default by the author) with 8 bits counter. non-inverting. So it counts from 0 to 255 and falls back
 *  to 0. I pick PWM phase correct. non-inverting. so it runs from 0 to 255 then 255 to 0. It gets more accuracy. When I look at the signal with
 *  an oscilloscope the frequency does not move with phase correct. with fast pwm it keeps moving by a few (974Hz to 976Hz) it is not as stable. 
 *  if the mode is fast PWM then the frequency of a cycle is : Freq CPU (16Mhz)/256/64 = 980Hz
 *  if the mode is phase correct then the frequency is 16MHz/510/64 = 490Hz.
 *  we use a prescaler of 64 (see datasheet p.137 CS*1=1 and CS*0=1 to slow down the clock. 
To set 3 pins in PWM . pin OCR1A (PB1)and pin OCR1B (PB2) for timer 1 and
pin OCR02A (PB3) for timer2.  They are both used as 8 bits counter

see the slides below to understand inverting mode vs non-inverting modes. there are pictures to understand. 
 https://www.slideshare.net/modyskyline/day8-motor-drive-and-pwm-techniques


 
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

  // Timer 2
  TCCR2A |= (1 << WGM20);                             /* this is phase correct. so the counter goes up then down*/
 // TCCR2A |= (1 << WGM21);                       /* uncomment this if you want fast pwm  */
  TCCR2B |= (1 << CS21)|(1 << CS20);                     /* PWM Freq = F_CPU/64/510 */
  TCCR2A |= (1 << COM2A1);                      /* PWM output on OCR2A . This is pin PB3 which is pin 11 */
}

int main(void) {

  uint8_t brightness;

  // -------- Inits --------- //

  initTimers();
  initUSART();
  printString("-- LED PWM Demo --\r\n");

               /* enable output on LED pins, triggered by PWM hardware */
  LED_DDR |= (1 << LED1);   // LED are output. if you want the pwm pins to be 0 we need to write 0 at those pins (PORT.. =0) but also we need
                              // to decouple the pwm pins from the counter. so COM.. =0. See above. 
  LED_DDR |= (1 << LED2);
  LED_DDR |= (1 << LED3);

  // ------ Event loop ------ //
  while (1) {

   // printString("\r\nEnter (0-255) for PWM duty cycle: ");
   // brightness = getNumber();  It seems to mess up the pwm. To try to implement with arduino IDE and their functions. removed. 
    OCR1A=200; // setting brightness of PB1
    OCR1B=200;
    OCR2A=200;
  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
