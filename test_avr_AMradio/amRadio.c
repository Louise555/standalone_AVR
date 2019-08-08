/*
5-17-19
Plays a simple tune, broadcasts it in the AM radio band.
We wiggle the volume envelop of the carrier with a pitch. 

The idea is to create a radio signal that has a carrier (high constant  frequency signal 1 MhZ) and a radio signal that will
modulate the amplitude of the carrier. The radio signal has a frequency between 61Hz and 1560Hz

timer0 will toggle pin PD5 at a frequency of 1MHz and timer 1 will turn on and off this carrier to produce a pitch (note) using an interrupt. 

so timer0 is the carrier. Timer0: The tick clock is 16MHz. It is in CTC mode. it is toggling PD5 (antenna pin according to pinDefines.h_
non stop but only if PD5 is in output mode. so if PD5 is not in an output mode then the carrier can not be emitted. by changing the
direction (output or input)  of PD5 we will modulate the volume of the carrier (just HIGH or LOW at the pitch frequency)

So the value of the counter of timer0 does not change (OCR0A). It will toggle every time the counter reaches
its max value set in register OCR0A. if the value is 7 and the tick frequency is 16MHz (prescaler =1 so 0.063us between tick) that means
the counts counts 7+1 ticks before toggling. so every tick =  0.063 x 8 us. The counter counts till 0.5us. The period of the carrier is 0.5us x 2 = 1us so
the frequency of the carrier is 1Mhz. So the carrier is controlled by timer 0.
its job is to toggle the PD5 at a frequency of 1MHz as long as PD5 direction=1

Note if the uC has a frequency of 8MHz without external cristal then the number of the counter to reach is 3. 
for a 8Mhz and a prescaler of 1 then we need a counter value of 3. 
but here we have an external cristal so we don't have to mess up with the fuses.


timer1 creates the radio signal that modulates the timer0.
Timer1 is attached to an interrupt. The interrupt routine is called when the value in the counter 0CR1A is reached. The routine  will toggle PD5
thus modulating the carrier. The routine till toggle PD5 (turning on and off the carrier) by cahding the direction of PD5.
The frequency of the clock tick for timer 1 is reduced to 2Mhz. (prescaler of 8) 
the value of the counter (OC1R!A)  will change with the note we want to play. For example A0 (note) is 5946x2 for the value of the counter to reach.
It is a 16bits counter so max is 65,535. We are supposed to get a better accuracy than we the 8 bits timer from last code (counter_waveform).
so in our example for A0 the counter counts 5947x2 ticks = 5942 x 2 (1/2000000) = 6ms/ so a period of 12ms or frequency of 84Hz
this is 3 times *27.5Hz the fundamental of A0. So the radio plays the third harmonics. 
https://pages.mtu.edu/~suits/notefreqs.html see for fundamentals. 

The values of the counter for timer1 to reach to play a given note are defined in scale16.h (I multiplied the values by 2 because my clock
runs at 2MHz and not 1MHz. prescaler of 8 so 16MHx/2 = 8 MHz). 
for example 5946x2 for A2. When the timer 1 reaches this value the interrupt is called and we change the direction of PD5 to turn it on and off.
So the 1MHz carrier is modulates with a period defined by 5947x2x2x(2/1000000)-> 84Hz (A2 note).
When we are done with emitting the note only the carrier is emitted. (PD5 has a direction of 1). 


reference:
https://www.sparkfun.com/datasheets/Components/SMD/ATMega328.pdf


*/

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/power.h>
#include <avr/interrupt.h>
#include "pinDefines.h"
#include "scale16.h"

#define COUNTER_VALUE   7              /* determines carrier frequency . 1Mhz here. 
the carrier is the frequency the radio (reciever)  has to be tuned to to hear the signal. 
we have a 16MHz uC with external cristal and a prescaler of 1. If the counter has a value of 7 it counts till 8 so the frequency is 1MHz
1/16000000 = time between tick. that value X  (COUNTER-VALUE + 1) = time for counter to reach max. that value X 2 = period. frequency= 1/T */


static inline void initTimer0(void) {
  // timer0 is the carrier. It will toggle the pd5 as long as its output is 1 (output mode) 
  TCCR0A |= (1 << WGM01);                                  /* CTC mode */
  TCCR0A |= (1 << COM0B0);            /* Toggles pin each time through */
  TCCR0B |= (1 << CS00);              /* Clock at CPU frequency, 16MHz prescaler is 1 */
  OCR0A = COUNTER_VALUE;                          /* carrier frequency */
}

static inline void initTimer1(void) {
  TCCR1B |= (1 << WGM12);                                  /* CTC mode */
  TCCR1B |= (1 << CS11);            /* Clock at CPU/8 frequency, ~2MHz because I have an external cristal 16MHz. 
  we are supposed to get 1Mhz because the code (and the scale16.h)  was written for a 8MHz standalone chip. So to fix this problem 
  I will multiply the pitch (max counter) is the routine transmitBeep by 2. It is a 16 bits counter so it should not be a problem.
  max value for counter is 10,000 for a C2 . 10,000x2 = 20,000 smaller than (2^16 -1) */
  TIMSK1 |= (1 << OCIE1A);          /* enable output compare interrupt . For timer1 ISR will be called when interrupts are on
  and when the counter reach the pitch value or max counter given in 0CR1A counter*/
}
// if interrupts are on . as soon as pitch is reached the PD5 is toggled by changing the mode output on/off. Only when it is on the carrier is emitted. 
ISR(TIMER1_COMPA_vect) {                 /* ISR for audio-rate Timer 1 */
  ANTENNA_DDR ^= (1 << ANTENNA);          /* toggle carrier on and off */
}

static inline void transmitBeep(uint16_t pitch, uint16_t duration) {
  OCR1A = 2*pitch;                               /* set pitch for timer1 . I multiply by 2 because I don't have a 1MHz tick clock but 2Mhz
                                                  and the music scale is for a 1MHz. */
  sei();                                         /* turn on interrupts */
  do {
    _delay_ms(1);                            /* delay for pitch cycles */
    duration--;
  } while (duration > 0);
  cli();                  /* and disable ISR so that it stops toggling */
  ANTENNA_DDR |= (1 << ANTENNA);               /* back on full carrier . carrier is on when there is no pitch. It is a flat signal. to remove noise.*/
}

int main(void) {
  // -------- Inits --------- //

  //clock_prescale_set(clock_div_1);                  /* if we don't have an external clock. I don't need that because I have an external cristal
 // If you buy a chip without external chrystal they divide the clock by 8 to reduce power apparently. this command is to get back the full clock/

  initTimer0(); //initialization of timers. 
  initTimer1();

  // ------ Event loop ------ //
  while (1) {
  
    transmitBeep(A0, 200); // play the radio signal that will modulate the volume of the carrier. 
    /* if you want to check if we get the right frequency you can repeat the above line for ever.You get the carrier modulated by A0 = 84Hz
     *  which is the third harmonix 27.5 x 3. The counter max value is 5946x2 . With a clock tick of 2Mhz (16Mhz/8). 
     *  if I connect an ocilloscope to the signal I can detect only the carrier 1Mhz. So you can  connect the singal to a low pass filter RC with
     *  R = 5Kohms and C=0.1uF  pin PD5  -> R -> C -> gnd. I connect the oscilloscope across the C and I do get the 84Hz!! (plus some residual Mhz on top.
     *  but most if it is gone.
    */
    _delay_ms(200);
    transmitBeep(E3, 200);
    _delay_ms(200);
    transmitBeep(E3, 200);
    _delay_ms(200);
    transmitBeep(C3, 200);
    transmitBeep(E3, 200);
    _delay_ms(200);
    transmitBeep(G3, 400);
    _delay_ms(500);
    transmitBeep(G2, 400);

    _delay_ms(2500);

  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
