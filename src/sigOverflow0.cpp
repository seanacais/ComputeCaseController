/*
 * sigOverflow0.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: Kevin C. Castner
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Scheduler.h"
extern Scheduler sch0;

#include "eventBuffer.h"
extern eventBuffer eb0;

#include "Button.h"
extern Button sw0;

uint8_t TCNT0_VALUE = 143;

//uint8_t bcount = 1;
//uint8_t bcount2 = 1;

volatile uint16_t g_currentTick = 0;

// This interrupt needs to fire every 1 mSec
SIGNAL (SIG_OVERFLOW0) {

	g_currentTick++;
	TCNT0 = TCNT0_VALUE;
	static uint8_t switch0_debounce = SW_RELEASED;
	static uint8_t switch0_state = SW_STATE_UP;

	switch0_debounce = ((switch0_debounce << 1) | (PINB & 0x01));
	if (switch0_debounce == SW_PRESSED && switch0_state == SW_STATE_UP) {
		switch0_state = SW_STATE_DOWN;
		eb0.addEvent(BUTTON_DOWN);
	}

	if (switch0_debounce == SW_RELEASED && switch0_state == SW_STATE_DOWN) {
		switch0_state = SW_STATE_UP;
		eb0.addEvent(BUTTON_UP);
	}

	sw0.tick();

	if (sch0.isDue() == true) {
		eb0.addEvent(SCH_EVENT_READY);
	}

	if ((g_currentTick % SW_ASSESS_INTERVAL) == 0) {
		eb0.addEvent(BUTTON_ASSESS);
	}

//	if ((g_currentTick % 1024) == 0) { // every Sec
//		dcvm0.blink();
//	}
//

// Weird stuff seen.  If you set two different intervals to blink
// different LED's, it only works if at least one of the intervalse
// is a power of two.  For example, set one to blink at 7 seconds
// (modulo 7168) and one at 5 seconds (modulo 5120) and neither light
// blinks.  However if you set one to four seconds and one to 7 seconds
// the 7 second light loses sync at 42 seconds, regains sync at 70, loses
// it again at 84, and cyles through 126, 168, and 252.  I've seen it
// with other combinations of the numbers but did not document.

//	if ((g_currentTick % 4096) == 0) { // every 4 sec
//		switch (bcount++) {
//		case 0:
//			break;
//		case 1:
//			eb0.addEvent(DIAG_LED_BLINK);
//			break;
//		case 2:
//			eb0.addEvent(DIAG_LED_BLINK_2);
//			break;
//		case 3:
//			eb0.addEvent(DIAG_LED_BLINK_3);
//			break;
//		}
//		if (bcount >= 4) {
//			bcount = 1;
//		}
//	}
//
//	if (((g_currentTick % 4096) - 2048) == 0) { // every 4 Sec (2 sec offset)
//		switch (bcount2++) {
//		case 0:
//			break;
//		case 1:
//			eb0.addEvent(DCVM_BLINK);
//			break;
//		case 2:
//			eb0.addEvent(DCVM_BLINK_2);
//			break;
//		case 3:
//			eb0.addEvent(DCVM_BLINK_3);
//			break;
//		}
//		if (bcount2 >= 4) {
//			bcount2 = 1;
//		}
//	}

}
