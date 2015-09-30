/*
 * eventBuffer.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: seanacais
 */

static char const copyright[] =
		"Copyright Kevin C. Castner, 2015; All Rights Reserved";
static char const rcsid[] = "$Id: $";
static char const rcsversion[] = "$Revision: $ $Name:  $";

#include "eventBuffer.h"

eventBuffer::eventBuffer() :
		head(0), tail(0) {
}

bool eventBuffer::isMore() {
	if (head == tail)
		return false;
	else
		return true;
}

void eventBuffer::addEvent(const uint8_t command) {
	buffer[head++] = command;
	;
}

uint8_t eventBuffer::getNextEvent() {
	return buffer[tail++];
}

/*
 *
 * $Log: $
 */
