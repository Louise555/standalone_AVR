/*
 * this is to demonstrate that any pin (here PC1) can be attached to an interrupt. A change in voltage at PC1 will trigger an interrupt. 
 * PC1 is attached to a wire (capacitor) and will sense a change in voltate (when cap charges or discharges)
   Capacitive touch sensor demo. The idea is turn on an LED when a loose wire is touched or a hand-made capacitor (2 aluminum foil) is touched.
   If we touch the loose wire (CP1) its capacitance increases so it discharge time increases (time constant increases). 
   So the idea is to count how many discharges there are in 50ms depending if it is touched or not. That count depends on the capacitance. 
   When the wire is untouched the number of discharges is THRESHOLD. If we touch it the capacitance increases and the count of discharges  (chargecyclecount) is smaller
   so the LED is turned on. 

be aware of:
- when the uC starts all the pins are in input mode so capacitor is discharged. When the system launch the program the cap will charge and the interrupt will be called 
(change of voltage from LOW to HIGH) at first
- we can write a 1 to pc1 (PORT =1) ounce for all and we turn off the pull-up option. If PC1 is an input then PC1 the " 1" does not matter. PC1 has a high impedance
and the cap discharge through the resistor. If PC1 is an output then PC1 is a high. WRiting a 1 at a pin has no effect if it is an input and if there is no pull up. 
  
  Pc1 -> loose jumper to be touched - 200K resistor - Gnd. The the loose jumper (small) has a side at PC1 and the other is loose.
  PB0 we have a LED or we can have LEDS at all the pins of PORT B. 
 
  When PC1 is an input so the jumper discharges in the Gnd through the resistor. 
   When we touch the jumper firmly the capacitance increases. It takes more time to discharge the jumper+person. 

    When we start the program the pin cap (CP1)  goes to 0 (reset pin is on so  all pins are inputs attached to high impedance.
    the ISR starts and we count how many discharges in 50ms. Discharge means change in voltage from HIGH to LOW. LOW means about 1.5V for arduino. 

    We need to adjust THRESHOLD (50000 for me ). 
   It is about 50000 times when the jumper is let to float. The LED at PORT B is off. 
   If we touch the jumper. we change its capacitance. To it takes longer to discharge. to reach the 1.5V LOW of arduino. So we get less cycles in 50ms.
   I checked and if we get less than 200 cycles then we know it was touched. It works.

Note:
   If we connect a voltmeter at cap pin we see that when it is untouched we get an average voltage of about 3V. (average the exponential function of the discharge).
   If we touch it the average voltage drops to 2V or less. Then is because the time constant increases so the average decreases (integral of exp(-t/tau) between t=0 
   and the time it reaches the LOW 1.5V). 
Note:
  the pin interrupt detects changed in voltage which is LOW here. LOW is 1.5V about
Note: 
 if PORT = 1 for a pin it means that if we set the pin as an output (direction =1) then the pin is 1 (HIGH). but we can still change the direction to input (input=0)
 and this is ok (so capacitor discharges. input state means high impedance) . When it will be back to direction = 1 it will be a HIGH again. 
 Now we have to be careful and disable the pull-up during running time (with register).
 Otherwise when we go from output to input the input will be attached to 5V through a pullup resistor and we don't want that. We want the capacitor to discharge. 

   important: delay in main gets frozen when running the ISR
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "pinDefines.h"
#include "USART.h"

#define SENSE_TIME   50  // we count how many cycles in 50ms
#define THRESHOLD    50000 // for a stray capacitor (jumper wire) the number of cycles is below 50000 when touched (capacitance increases). 

// -------  Global Variables ---------- //
volatile uint16_t chargeCycleCount; // this is important. Since that variable is not change in main() if we don't use volatile the compiler will try
                                    //to optimize the code and remove it at compilation. It is also a global variable so it is seen by ISR and main

// ------- Functions -------- //

void initPinChangeInterrupt(void) {
  PCICR |= (1 << PCIE1);    /* enable Pin-change interrupts 1 (bank C) . All pins in bank C are interrupt pins*/
  PCMSK1 |= (1 << PC1);   /* enable specific interrupt for our pin PC1 . We refine and only set PC1 as a interrupt pin*/
}

ISR(PCINT1_vect) { // ISR routine. The vector points to the address in memory where the routine is located. 
  chargeCycleCount++;                             /* count this change . the number of cycles charge, discharge.*/

  CAP_SENSOR_DDR |= (1 << CAP_SENSOR);          /* cap is charged output mode. so direction is 1 which means it is a HIGH (PORT = 1 in the main) if pin is output then it is a 1 */
  _delay_us(1);                                      /* charging delay . Maybe not required. */

  CAP_SENSOR_DDR &= ~(1 << CAP_SENSOR);          /* cap as input . now we change the direction. input means 0 and high resistance. the capacitor discharges
                                                       through the resistor. */
  PCIFR |= (1 << PCIF1);             /* clear the pin-change interrupt manually. 1 means clear here. so we can go back to main. otherwise we get stuck in the routine 
                                       and don't get the 50ms . we get stuck in infinite loop. Until we get out of the routine it wont detect a chage in voltage from HIGH to LOW */
}


int main(void) {
  // -------- Inits --------- //
 // clock_prescale_set(clock_div_1);                       /* full speed. This is not necessarly if we use an external oscillator */
  initUSART();
  printString("==[ Cap Sensor ]==\r\n\r\n");

  LED_DDR = 0xff;
  MCUCR |= (1 << PUD);                          /* disable all pullups. so when the CP1 is an input it will discharge */
  CAP_SENSOR_PORT |= (1 << CAP_SENSOR);    /* we can leave output high . So if the pin is HIGH when the direction is output the cap charges. if direction is input
                                         then the capacitor discharges */

  initPinChangeInterrupt();

  // ------ Event loop ------ //
  while (1) {

    chargeCycleCount = 0;                             /* reset counter */
    CAP_SENSOR_DDR |= (1 << CAP_SENSOR);     /* when the program starts (reset is 0 ) pins are input including  CAP1 . The pin is LOW (cap discharges) then it is charged
                                             because DDR =1 here (this line) so  there is a change in voltage so the routine starts . cap charges again inside ISR then discharges etc.. 
                                            */
    sei();                            /* start up interrupts, counting . This is the global flag. Enable the interrupts. */
    _delay_ms(SENSE_TIME); // count the cycles during this time. 
    cli();                                                     /* done . disable the interruot functions. */
    if (chargeCycleCount < THRESHOLD) { // the pin cap was touched.
      
      LED_PORT = 0xff;  // LED turned on if touch 
    }
    else {
      LED_PORT = 0;   // LED turn off if not touched. 
    }
    printWord(chargeCycleCount);                    /* for fine tuning */
    printString("\r\n");

  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
