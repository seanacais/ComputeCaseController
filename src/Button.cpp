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
	last_down_tick = 0;
	last_up_tick = 0;
}

void Button::accept_down(){
	last_down_tick = g_currentTick;
}

void Button::accept_up(){
	last_up_tick = g_currentTick;
}

void Button::accept_press(){
	last_up_tick = g_currentTick;
}

void Button::assess(){
//	last_up_tick = g_currentTick;
}
/*
 *
 * $Log: $
 */
