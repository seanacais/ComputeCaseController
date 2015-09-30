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

class Button {
	/* This class implements a switch handler to do timings for multi-presses
	 */

private:
	uint32_t last_up_tick;
	uint32_t last_down_tick;
public:
	Button();
	void accept_down();
	void accept_up();
	void accept_press();
	bool isMore();
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
