/*
 * Quick audio demo using Timer 0 to generate audio frequencies directly.
This show that we can use the pins attached to the counters (here  timer 0 pin PD6/0C0A) to generate waveforms (square wave) We toggle a pin when the counter reaches a given value.
period of the waveform = 2 rounds of the counter. (pin attached to counter is HIGH then LOW during a period). 
we can use that to toggle a speaker and produce a melody. The speaker is attached to the pin PD6. The square wave will pull and push the speaker making
a sound. 
Although I am not sure how accurage are the frequencies for a given note. 
We use the timer0. So the speaker is attached to one of the pin attached to timer 0 (0C0A pin or PD6) 
We use the timer0 in CTC mode (compare match mode). The counter counts from 0 to value in register OCR0A before it starts again.
So the counter is reset every OCR0A + 1 value because it starts counting at 0. 
to the timer0 is toggled only if the pin is an output. So a waveform is generated. on - off - on -off
But 1 period is on - off so the period is the time it takes for the counter to count to (  OCR0A +`1 )x2 . When we flip we get
the frequency of the wavefront. so we are generating wavefron. If we have 16MHz CPU and prescaler of 1028 we get 64 us between each tick.
so the smallest period is (64 us)x 2 -> 7.8KHz largest frequency. the timer 0 is 8 bits so it counts from 0 to 255 max. 
256 x 64us x 2 = largest period of the waveform. or 30.5Hz smaller frequency. 
The speaker is toggled during a given duration with a frequency defined by the max value of the counter. 
We use the file scale8.h to match timer value and note (frequency) you want from scaler. It is not perfect that table. 
Not exactly matching the music frequency <-> note

set up: attached a 8ohms speaker at PD6 -> capacitor 47uF -> Gnd. The speaker has a very small resistance (8ohms) that means it wants to draw
lots of current and the uC can't provide that much current. It would damage it. So we place a capacitor between one side of the speaker
and the Gnd. The capacitor charges at 5V and will block DC current. It will only let go small AC current generated when the pin is toggled.

*/

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include "pinDefines.h"   //Pd6 is for timer 0 
#include "scale8.h"                                     /* 8-bit scale */

// initialisation of the timer0 . CTC mode. the pin to toggle is PD6 (0C0A)
static inline void initTimer(void) {
  TCCR0A |= (1 << WGM01);                                  /* CTC mode. waveform generation */
  TCCR0A |= (1 << COM0A0);           /* Toggles pin each cycle through. pin is attached to PD6 */
  TCCR0B |= (1 << CS00) | (1 << CS02);      /* prescaler is 1028. CPU clock / 1028 for 16MHz clock with external christal   between tick is 64us.
  see data sheet https://www.sparkfun.com/datasheets/Components/SMD/ATMega328.pdf p.110  . the time between click is 64uS */
}

static inline void playNote(uint8_t period, uint16_t duration) {

  TCNT0 = 0;                                      /* reset the counter */
  OCR0A = period;                                     /* set pitch . the counter counts till OCR0A+1. THat will be half of a period of the wave*/
                                          // SPEAKER_DDR stands for DDRD (direction of PORTD) see pinDefines.h 
                                          // SPEAKER is PD6 
  SPEAKER_DDR |= (1 << SPEAKER);           /* enable output on speaker. speaker has to be an output.  speaker at PD6*/

  while (duration) {                                 /* Variable delay */
    _delay_ms(1);
    duration--;
  }
  SPEAKER_DDR &= ~(1 << SPEAKER);                  /* turn speaker off . The PD6 can not be toggled anymore*/
}

int main(void) {
  // -------- Inits --------- //
  initTimer();
  // ------ Event loop ------ //
  while (1) {
                                                    /* Play some notes */
    playNote(C2, 200); /* with the hantel oscilloscope I find a signal of 77 Hz. which make sense because in scale8.h it says C2 is 100 for the counter 
    so the counter counts 101 ticks = 101 x 64 us = 0.006464s. One cycle is twice that number (period) or 0.12928 s frequency = 1/T = 77 Hz about. so it is consistent. */
    playNote(E2, 200);
    playNote(G2, 200);
    playNote(C3, 400);

    _delay_ms(1000);
    _delay_ms(1000);
    _delay_ms(1000);

  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
