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

#define SW_HOLD_FOR_POWER_OFF 3500
#define SW_PRESS_DELAY 400
//#define SW_MULTI_PRESS_SENSITIVITY 500

class Button {
	/* This class implements a switch handler to do timings for multi-presses
	 */

private:

	volatile bool new_event;
	volatile bool last_event_was_psu_off;
	volatile bool multi_press_finished;
	volatile uint8_t press_count;

	volatile uint8_t switch_state;

	volatile uint32_t last_up_tick;
	volatile uint32_t prev_up_tick;
	volatile uint32_t last_down_tick;
	volatile uint32_t last_event_tick;
public:
	Button();
	void accept_down();
	void accept_up();
	void assess();
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
