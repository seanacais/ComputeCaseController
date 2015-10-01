/*
 * ComputeCaseController.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: Kevin C. Castner
 */

// Include definitions for the current part defined in Makefile
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/sleep.h>

// use only for temporary testing
//#include <util/delay.h>
//		_delay_ms(500);

#include "Button.h"
#include "Scheduler.h"
#include "eventBuffer.h"
#include "PSURemote.h"

// Function Prototypes
void initialize(void);
void diag_led_on(void);
void diag_led_off(void);
void dcvm_on(void);
void dcvm_off(void);

//  Global Variables
extern volatile uint32_t g_currentTick;
uint8_t count = 0;

// Global for timer overflow
extern uint8_t TCNT0_VALUE;

Button s0;      // input momentary switch
Scheduler t0;   // scheduler object
eventBuffer eb; // global input event buffer
PSURemote p0;   // PSU with remote control

void initialize(void) {

	DDRB |= (_BV(PB1)); // Set PB1 to an output (PSURC)
	DDRB |= (_BV(PB2)); // Set PB2 to an output (DCVM)
	DDRB |= (_BV(PB3)); // Set PB3 to an output (Diagnostic LED)
//	TCCR0 |= _BV(CS02);	            // set timer 0 prescalar to CK/256
//	TCCR0 |= _BV(CS01) | _BV(CS00);	// set timer 0 prescalar to CK/64
	TCCR0 |= _BV(CS01);          	// set timer 0 prescalar to CK/8
	TCNT0 = TCNT0_VALUE; // Load the Timer Value
	TIMSK |= _BV(TOIE0); // enable Timer Counter 0 overflow interrupt

	PORTB |= (_BV(PB0)); // Use the internal pullup on PB0
	PORTB |= (_BV(PB3)); // start with diagnostic LED on
}

void diag_led_on(void){
	PORTB |= _BV(PB3);
}
void diag_led_off(void){
	PORTB &= ~(_BV(PB3));
}
void diag_led_toggle(void){
	PORTB ^= _BV(PB3);
}
void diag_led_blink(void){
	eb.addEvent(DIAG_LED_TOGGLE);
	t0.scheduleEvent(75, DIAG_LED_TOGGLE);
}
void diag_led_blink_2(void){
	eb.addEvent(DIAG_LED_TOGGLE);
	t0.scheduleEvent(75, DIAG_LED_TOGGLE);
	t0.scheduleEvent(150, DIAG_LED_TOGGLE);
	t0.scheduleEvent(300, DIAG_LED_TOGGLE);
}
void dcvm_on(void){
	PORTB |= _BV(PB2);
}
void dcvm_off(void){
	PORTB &= ~(_BV(PB2));
}

int main(void) {		//  Template Mainline

	initialize();
	sei();

	diag_led_off();
	dcvm_on();
	p0.psu_on();


	while (1) {   			// eternal loop
		while (eb.isMore()) {
			switch(eb.getNextEvent()) {
			case BUTTON_DOWN:
				s0.accept_down();
				break;
			case BUTTON_UP:
				s0.accept_up();
				break;
			case BUTTON_ASSESS:
				s0.assess();
				break;
			case DIAG_LED_ON:
				diag_led_on();
				break;
			case DIAG_LED_OFF:
				diag_led_off();
				break;
			case DIAG_LED_TOGGLE:
				diag_led_toggle();
				break;
			case DIAG_LED_BLINK:
				diag_led_blink();
				break;
			case DIAG_LED_BLINK_2:
				diag_led_blink_2();
				break;
			case DCVM_ON:
				dcvm_on();
				break;
			case DCVM_OFF:
				dcvm_off();
				break;
			case DCVM_BLINK:
				break;
			case DCVM_BLINK_2:
				break;
			case DCVM_BLINK_3:
				break;
			case DCVM_BLINK_4:
				break;
			case BUTTON_PRESS:
				eb.addEvent(PSU_RMT_ON);
				break;
			case BUTTON_PRESS_2:
				break;
			case BUTTON_PRESS_3:
				break;
			case BUTTON_PRESS_4:
				break;
			case BUTTON_P_AND_H:
				eb.addEvent(PSU_RMT_OFF);
				break;
			case PSU_RMT_OFF:
				p0.psu_off();
				break;
			case PSU_RMT_ON:
				p0.psu_on();
				break;
			case SCH_EVENT_READY:
				eb.addEvent(t0.getEvent());
				break;
			}

		}
//		set_sleep_mode(SLEEP_MODE_IDLE);
//		cli();
//		sleep_enable()
//		;
//		sei();
//		sleep_cpu()
//		;
//		sleep_disable()
//		;
//		sei();
	} //  endwhile
} // end main()
