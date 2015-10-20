/*
 * eventBuffer.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: Kevin C. Castner
 */

#include <avr/interrupt.h>
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
}

uint8_t eventBuffer::getNextEvent() {
	return buffer[tail++];
}

