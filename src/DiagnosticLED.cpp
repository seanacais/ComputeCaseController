/*
 * DiagnosticLED.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 29, 2015
 *      Author: Kevin C. Castner
 */

#include <avr/io.h>

#include "DiagnosticLED.h"
#include "eventBuffer.h"
extern eventBuffer eb0;

#include "Scheduler.h"
extern Scheduler sch0;

DiagnosticLED::DiagnosticLED() {
	DDRB |= (_BV(PB3)); // Set PB3 to an output (Diagnostic LED)
	off();               // start with diagnostic LED off
}

void DiagnosticLED::on(void) {
	PORTB |= _BV(PB3);
}

void DiagnosticLED::off(void) {
	PORTB &= ~(_BV(PB3));
}

void DiagnosticLED::toggle(void) {
	PORTB ^= _BV(PB3);
}

void DiagnosticLED::blink(void) {
	eb0.addEvent(DIAG_LED_TOGGLE);
	sch0.scheduleEvent(75, DIAG_LED_TOGGLE);
}

void DiagnosticLED::blink_2(void) {
	eb0.addEvent(DIAG_LED_TOGGLE);
	sch0.scheduleEvent(75, DIAG_LED_TOGGLE);
	sch0.scheduleEvent(150, DIAG_LED_TOGGLE);
	sch0.scheduleEvent(225, DIAG_LED_TOGGLE);
}

void DiagnosticLED::blink_3(void) {
	sch0.scheduleEvent(625, DIAG_LED_TOGGLE);  // OFF  @ 175
	sch0.scheduleEvent(175, DIAG_LED_TOGGLE);  // ON  @ 100
	sch0.scheduleEvent(450, DIAG_LED_TOGGLE);  // ON   @ 150
	sch0.scheduleEvent(300, DIAG_LED_TOGGLE);  // OFF  @ 125
	eb0.addEvent(DIAG_LED_TOGGLE);  // ON
	sch0.scheduleEvent(75, DIAG_LED_TOGGLE);  // OFF  @ 75
}
