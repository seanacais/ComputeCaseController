/*
 * eventBuffer.h
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: seanacais
 */

#ifndef SRC_EVENTBUFFER_H_
#define SRC_EVENTBUFFER_H_

#include <avr/io.h>

class eventBuffer {
	/* This class implements a ring buffer where button and input events
	 * can be placed for future scheduling.
	 */

private:
	uint8_t buffer[256];
	volatile uint8_t head, tail;

public:
	eventBuffer();
	bool isMore();
	void addEvent(const uint8_t);
	uint8_t getNextEvent();
};

// Command Codes
// #define RESERVED 0x00 // Reserved
// #define RESERVED 0x01 // Reserved
#define BUTTON_ASSESS    0x08 // Kick the Button Class into assessing it's internal state
#define BUTTON_DOWN      0x09 // Momentary Switch is pressed
#define BUTTON_UP        0x0A // Momentary Switch is released
#define BUTTON_PRESS     0x0B // Momentary Switch is pressed and released
#define BUTTON_PRESS_2   0x0C // Momentary Switch is pressed and released twice
#define BUTTON_PRESS_3   0x0D // Momentary Switch is pressed and released 3 times
#define BUTTON_PRESS_4   0x0E // Momentary Switch is pressed and released 4 times
#define BUTTON_P_AND_H   0x0F // Momentary Switch is pressed and held for 4.5 seconds
#define SCH_EVENT_READY  0x10 // There is an event scheduled that is due
#define PSU_RMT_ON       0x20 // Turn of the PSU via Remote Interface
#define PSU_RMT_OFF      0x21 // Turn of the PSU via Remote Interface
#define DCVM_ON          0x30 // Turn on DC Volt Meter
#define DCVM_OFF         0x31 // Turn off DC Volt Meter
#define DCVM_TOGGLE      0x32 // Toggle the Diagnostic LED
#define DCVM_BLINK       0x32 // Blink the DC Volt Meter
#define DCVM_BLINK_2     0x33 // Blink the DC Volt Meter 2x
#define DCVM_BLINK_3     0x34 // Blink the DC Volt Meter 3x
#define DCVM_BLINK_4     0x35 // Blink the DC Volt Meter 4x
#define DIAG_LED_ON      0x40 // Turn on Diagnostic LED
#define DIAG_LED_OFF     0x41 // Turn off Diagnostic LED
#define DIAG_LED_TOGGLE  0x42 // Toggle the Diagnostic LED
#define DIAG_LED_BLINK   0x43 // Blink the Diagnostic LED
#define DIAG_LED_BLINK_2 0x44 // Blink the Diagnostic LED 2x
#define DIAG_LED_BLINK_3 0x45 // Blink the Diagnostic LED 3x
#define DIAG_LED_BLINK_4 0x46 // Blink the Diagnostic LED 4x
#endif /* SRC_EVENTBUFFER_H_ */

/*
 * $Log: $new
 *
 * static char const copyright[] =
 *   "Kevin C. Castner, 2015, All Rights Reserved";
 * static char const rcsid[] = "$Id: $";
 * static char const rcsversion[] = "$Revision: $";
 */
