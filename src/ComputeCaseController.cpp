/*
 * ComputeCaseController.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: Kevin C. Castner
 */

static char const copyright[] =
		"Copyright (c) Kevin C. Castner, 2015; All Rights Reserved";

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
#include "DiagnosticLED.h"
#include "VoltMeter.h"

// Function Prototypes
void initialize(void);

//  Global Variables
// extern volatile uint16_t g_currentTick;
uint8_t count = 0;

// Global for timer overflow
extern uint8_t TCNT0_VALUE;

Button sw0;      // input momentary switch
Scheduler sch0;   // scheduler object
eventBuffer eb0; // global input event buffer
PSURemote p0;   // PSU with remote control
DiagnosticLED dled0;
VoltMeter dcvm0;

void initialize(void) {

//	TCCR0 |= _BV(CS02);	            // set timer 0 prescalar to CK/256
//	TCCR0 |= _BV(CS01) | _BV(CS00);	// set timer 0 prescalar to CK/64
	TCCR0 |= _BV(CS01);          	// set timer 0 prescalar to CK/8
	TCNT0 = TCNT0_VALUE; // Load the Timer Value
	TIMSK |= _BV(TOIE0); // enable Timer Counter 0 overflow interrupt

}


int main(void) {		//  Template Mainline

	initialize();

	sei();
	while (1) {   			// eternal loop
		while (eb0.isMore()) {
			switch (eb0.getNextEvent()) {
			case BUTTON_DOWN:
				sw0.accept_down();
				break;
			case BUTTON_UP:
				sw0.accept_up();
				break;
			case BUTTON_ASSESS:
				sw0.assess();
				break;
			case DIAG_LED_ON:
				dled0.on();
				break;
			case DIAG_LED_OFF:
				dled0.off();
				break;
			case DIAG_LED_TOGGLE:
				dled0.toggle();
				break;
			case DIAG_LED_BLINK:
				dled0.blink();
				break;
			case DIAG_LED_BLINK_2:
				dled0.blink_2();
				break;
			case DIAG_LED_BLINK_3:
				dled0.blink_3();
				break;
			case DCVM_ON:
				dcvm0.on();
				break;
			case DCVM_OFF:
				dcvm0.off();
				break;
			case DCVM_TOGGLE:
				dcvm0.toggle();
				break;
			case DCVM_BLINK:
				dcvm0.blink();
				break;
			case DCVM_BLINK_2:
				dcvm0.blink_2();
				break;
			case DCVM_BLINK_3:
				dcvm0.blink_3();
				break;
			case BUTTON_PRESS:
				eb0.addEvent(PSU_RMT_ON);
				eb0.addEvent(DIAG_LED_BLINK);
				break;
			case BUTTON_PRESS_2:
				eb0.addEvent(DIAG_LED_BLINK_2);
				break;
			case BUTTON_PRESS_3:
				eb0.addEvent(DIAG_LED_BLINK_3);
				break;
			case BUTTON_P_AND_H:
				//eb0.addEvent(PSU_RMT_OFF);
				//sch0.scheduleEvent(8192, DCVM_OFF);
				eb0.addEvent(DCVM_BLINK_2);
				break;
			case PSU_RMT_OFF:
				p0.off();
				break;
			case PSU_RMT_ON:
				p0.on();
				break;
			case SCH_EVENT_READY:
				eb0.addEvent(sch0.getEvent());
				break;
			}

		}
		set_sleep_mode(SLEEP_MODE_IDLE);
		cli();
		sleep_enable();
		sei();
		sleep_cpu();
		sleep_disable();
		sei();
	} //  endwhile
} // end main()
