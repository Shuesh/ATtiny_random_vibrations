#include <arduino.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/fuse.h>
#include <avr/wdt.h>

// This define NEEDS to go before util/delay.h, otherwise none of the delays are correct.
#define F_CPU 1000000UL
#include <util/delay.h>


int iterations;
int target;

// This code executes when the watchdog timer fires. Interrupt Service Routine
ISR(WDT_vect){
}

void setup(){
    cli(); //disable all interrupts
    MCUSR = 0; //Clear all reset registers

    WDTCR |= (1<<WDCE) | (1<<WDE); //Set the watchdog change enable (required for any edits to the watchdog timer) and watchdog enable bits. Keep the rest the same
    WDTCR = (1<<WDCE) | (1<<WDP3) | (1<<WDP0); //Set the change enable flag, then configure the watchdog timer prescaler for ~8 seconds. P.46
    WDTCR |= (1<<WDIE); //Set the watchdog timeout interrupt enable bit. Executes an interrupt as opposed to a reset when the timer fires. 
                        // Must reset this bit after each interrupt, or it will default back to resetting the MCU when the timer fires.
    sei(); //enable all interrupts

    DDRB |= (1<<DDB3); // Set pin 3 to be an output
    iterations = 0;
    //maximum duration of 2 hours = 900x 8s cycles. Max rand value is at least 32767
    target = rand() % 900 + 1;
    // target = rand() % 4 + 1; //debug

    set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Sets sleep mode to Power-down

    PORTB |= (1<<PORTB3); // Set pin 3 high
    _delay_ms(1000);
    PORTB &= ~(1<<PORTB3); // set pin 3 low
    _delay_ms(1000);
    PORTB |= (1<<PORTB3); // Set pin 3 high
    _delay_ms(1000);
    PORTB &= ~(1<<PORTB3); // set pin 3 low
}

void loop(){
  
  iterations += 1;

  if (iterations == target){
    PORTB |= (1<<PORTB3); // Set pin 3 high
    _delay_ms(2000); //multiplies by 8 for some reason unless arduino.h is included
    PORTB &= ~(1<<PORTB3); // set pin 3 low

    target = rand() % 900 + 1;
    // target = rand() % 4 + 1; //debug
    iterations = 0;
  }

  power_all_disable(); //Disables all the extra functions for power saving purposes
  WDTCR |= (1<<WDIE); //Re-enable the watchdog timer as an interrupt (as opposed to a reset)
  sleep_bod_disable(); //Disables the brown-out detector while sleeping. Gets reset upon waking up
  sleep_mode(); //Goes to sleep
}