/*
 * PSURemote.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 29, 2015
 *      Author: Kevin C. Castner
 */

#include <avr/io.h>

#include "VoltMeter.h"
#include "eventBuffer.h"
extern eventBuffer eb0;

#include "Scheduler.h"
extern Scheduler sch0;

VoltMeter::VoltMeter() {
	DDRB |= (_BV(PB2)); // Set PB2 to an output (DCVM)
}

void VoltMeter::on(void) {
	PORTB |= _BV(PB2);
}
void VoltMeter::off(void) {
	PORTB &= ~(_BV(PB2));
}
void VoltMeter::toggle(void) {
	PORTB ^= _BV(PB2);
}
void VoltMeter::blink(void) {
	eb0.addEvent(DCVM_TOGGLE);
	sch0.scheduleEvent(90, DCVM_TOGGLE);
}
void VoltMeter::blink_2(void) {
	eb0.addEvent(DCVM_TOGGLE);
	sch0.scheduleEvent(90, DCVM_TOGGLE);
	sch0.scheduleEvent(180, DCVM_TOGGLE);
	sch0.scheduleEvent(270, DCVM_TOGGLE);
}
void VoltMeter::blink_3(void) {
	eb0.addEvent(DCVM_TOGGLE);
	sch0.scheduleEvent(90, DCVM_TOGGLE);
	sch0.scheduleEvent(180, DCVM_TOGGLE);
	sch0.scheduleEvent(270, DCVM_TOGGLE);
	sch0.scheduleEvent(360, DCVM_TOGGLE);
	sch0.scheduleEvent(450, DCVM_TOGGLE);
}
