/*
 * Button.h
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: seanacais
 */

#ifndef SRC_BUTTON_H_
#define SRC_BUTTON_H_

#include <avr/io.h>

#define SW_STATE_UP 0x00
#define SW_STATE_DOWN 0xFF
#define SW_PRESSED 0x00
#define SW_RELEASED 0xFF


#define SW_HOLD_FOR_POWER_OFF 3000
#define SW_PRESS_DELAY 400
//#define SW_MULTI_PRESS_SENSITIVITY 500

#define SW_ASSESS_INTERVAL 32
#define MAX_UINT16_VALUE 0xFFFF

class Button {
	/* This class implements a switch handler to do timings for multi-presses
	 */

private:

	volatile bool new_event;
	volatile bool last_event_was_pandh;
	volatile bool in_multi_press;
	volatile uint8_t press_count;

	volatile uint8_t switch_state;

	volatile uint16_t switch_tick;
	volatile bool switch_tick_on;
	volatile uint16_t last_up_tick;
	volatile uint16_t prev_up_tick;
	volatile uint16_t last_down_tick;
	volatile uint16_t last_event_tick;

	void resetEvent();
public:
	Button();
	void accept_down();
	void accept_up();
	void assess();
	void tick();
};


#endif /* SRC_BUTTON_H_ */

/*
 * $Log: $
 *
 * static char const copyright[] =
 *   "Kevin C. Castner, 2015, All Rights Reserved";
 * static char const rcsid[] = "$Id: $";
 * static char const rcsversion[] = "$Revision: $";
 */
