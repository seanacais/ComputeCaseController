/*
 * Button.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: Kevin C. Castner
 */

#include <stdio.h>

#include "eventBuffer.h"

extern eventBuffer eb0;

#include "Button.h"

Button::Button() {
	PORTB |= (_BV(PB0)); // Use the internal pullup on PB0 for the switch
	switch_state = SW_STATE_UP;
	in_multi_press = false;
	last_event_was_pandh = false;
	press_count = 1;
	resetEvent();
}

void Button::accept_down() {
	switch_state = SW_STATE_DOWN;
	switch_tick_on = true;
	last_down_tick = switch_tick;
	last_event_tick = last_down_tick;
	new_event = true;
	if ((last_up_tick + SW_PRESS_DELAY) > last_down_tick) {
		in_multi_press = true;
	} else {
		in_multi_press = false;
	}
}

void Button::accept_up() {
	switch_state = SW_STATE_UP;

	// Don't respond to the switch release if the previous event was a press and hold
	if (last_event_was_pandh) {
		last_event_was_pandh = false;
		return;
	}
	last_up_tick = switch_tick;
	last_event_tick = last_up_tick;
	if (in_multi_press) {
		press_count++;
		return;
	}
	new_event = true;
}

// assess() runs periodically from the timer overflow interrupt.
// At 8 mS it is guaranteed to catch every up/down switch event as the
// rate of switch events cannot be faster due to the de-bounce.
// Should the processor not be able to keep up, it may be sufficient
// to run this routine at a slower rate.
// Modify SW_ASSESS_INTERVAL to adjust.
void Button::assess() {

	// exit if there has been no switch activity (most frequent case)
	// or the switch sequence delay has not elapsed
	if ((new_event == false)
			|| (switch_tick < (last_event_tick + SW_PRESS_DELAY))) {
		return;
	}

	if (switch_state == SW_STATE_UP) {
		switch (press_count) {
		case 1:
			eb0.addEvent(BUTTON_PRESS);
			break;
		case 2:
			eb0.addEvent(BUTTON_PRESS_2);
			break;
		case 3:
			eb0.addEvent(BUTTON_PRESS_3);
			break;
		}
		press_count = 1;
		resetEvent();
	} else { // switch_state  is down
		if (switch_tick >= (last_down_tick + SW_HOLD_FOR_POWER_OFF)) {
			last_event_was_pandh = true;
			eb0.addEvent(BUTTON_P_AND_H);
			resetEvent();
		}
	}
}

void Button::tick() {
	if (switch_tick_on) {
		switch_tick++;
	}
}

void Button::resetEvent() {
	new_event = false;
	switch_tick_on = false;
	switch_tick = MAX_UINT16_VALUE / 2;
	last_down_tick = switch_tick;
	last_up_tick = switch_tick - SW_PRESS_DELAY - 1;
	last_event_tick = last_up_tick;
}

