/*
 * VoltMeter.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Oct 20, 2015
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
	state = false;
	off();  // start with the meter off
}

bool VoltMeter::isOn(){
	return state;
}

bool VoltMeter::isOff(){
	return !state;
}

void VoltMeter::on(void) {
	state = true;
	PORTB |= _BV(PB2);
}
void VoltMeter::off(void) {
	state = false;
	PORTB &= ~(_BV(PB2));
}
void VoltMeter::toggle(void) {
	state = !state;
	PORTB ^= _BV(PB2);
}
void VoltMeter::blink(void) {
	if (state) {
		eb0.addEvent(DCVM_TOGGLE);
		sch0.scheduleEvent(DCVM_BLINK_TIME, DCVM_TOGGLE);
	}
}
void VoltMeter::blink_2(void) {
	if (state) {
		eb0.addEvent(DCVM_TOGGLE);
		sch0.scheduleEvent(DCVM_BLINK_2_TIME/3, DCVM_TOGGLE);
	}
	sch0.scheduleEvent(DCVM_BLINK_2_TIME / 3 * 2, DCVM_TOGGLE);
	sch0.scheduleEvent(DCVM_BLINK_2_TIME, DCVM_TOGGLE);
}
void VoltMeter::blink_3(void) {
	if (state) {
		eb0.addEvent(DCVM_TOGGLE);
		sch0.scheduleEvent(DCVM_BLINK_3_TIME / 5, DCVM_TOGGLE);
	}
	sch0.scheduleEvent(DCVM_BLINK_3_TIME / 5 * 2, DCVM_TOGGLE);
	sch0.scheduleEvent(DCVM_BLINK_3_TIME / 5 * 3, DCVM_TOGGLE);
	sch0.scheduleEvent(DCVM_BLINK_3_TIME / 5 * 4, DCVM_TOGGLE);
	sch0.scheduleEvent(DCVM_BLINK_3_TIME, DCVM_TOGGLE);
}
