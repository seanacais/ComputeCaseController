/*
 * Button.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: seanacais
 */

static char const copyright[] =
		"Copyright Kevin C. Castner, 2015; All Rights Reserved";
static char const rcsid[] = "$Id: $";
static char const rcsversion[] = "$Revision: $ $Name:  $";

#include <stdio.h>

#include "eventBuffer.h"

extern volatile uint32_t g_currentTick;
extern eventBuffer eb;

#include "Button.h"

Button::Button() {
	switch_state = SW_STATE_UP;
	multi_press_finished = true;
	last_event_was_psu_off = false;
	new_event = false;
	last_down_tick = 0;
	last_up_tick = g_currentTick;
	last_event_tick = last_up_tick;
	press_count = 1;
}

void Button::accept_down() {
	switch_state = SW_STATE_DOWN;
	last_down_tick = g_currentTick;
	last_event_tick = last_down_tick;
	new_event = true;
	if ((last_up_tick + SW_PRESS_DELAY) > last_down_tick) {
		multi_press_finished = false;
	} else {
		multi_press_finished = true;
	}
}

void Button::accept_up() {
	switch_state = SW_STATE_UP;
	// Don't do anything with the switch release if the last event caused a PSU Off event
	if (last_event_was_psu_off) {
		last_event_was_psu_off = false;
		return;
	}
	last_up_tick = g_currentTick;
	last_event_tick = last_up_tick;
	if (!multi_press_finished) {
		press_count++;
		return;
	}
	new_event = true;
}

void Button::assess() {
// assess() runs every eight mS by virtue of an interrupt.
// At 8 mS it is guaranteed to catch every up/down switch event as the
// rate of switch events cannot be faster due to the de-bounce.
// Should the processor not be able to keep up, it may be sufficient
// to run this routine at 16mS but it would be recommended to make the
// de-bounce in the timer interrupt the same length of time.

// exit if there has been no switch activity (most frequent case)
// or the switch sequence delay has not elapsed
	if ((new_event == false)
			|| (g_currentTick < (last_event_tick + SW_PRESS_DELAY))) {
		return;
	}

	if (switch_state == SW_STATE_UP) {
		switch (press_count) {
		case 1:
			eb.addEvent(BUTTON_PRESS);
			break;
		case 2:
			eb.addEvent(BUTTON_PRESS_2);
			break;
		case 3:
			eb.addEvent(BUTTON_PRESS_3);
			break;
		}
		press_count = 1;
		new_event = false;
	} else { // switch_state  is down
		if ((last_down_tick >= last_up_tick)
				&& (g_currentTick >= (last_down_tick + SW_HOLD_FOR_POWER_OFF))) {
			last_event_was_psu_off = true;
			eb.addEvent(BUTTON_P_AND_H);
			new_event = false;
		}
	}
}

/*
 *
 * $Log: $
 */
