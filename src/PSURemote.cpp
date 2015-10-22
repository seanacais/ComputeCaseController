/*
 * PSURemote.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Oct 20, 2015
 *      Author: Kevin C. Castner
 */

#include <avr/io.h>
#include "PSURemote.h"

PSURemote::PSURemote() {

	DDRB |= (_BV(PB1)); // Set PB1 to an output (PSURC)
	state = false;
	PORTB |= _BV(PB1);  // Start with the PSU off
}

void PSURemote::on(){
	if (state == false) {
		state = true;
		PORTB &= ~(_BV(PB1));
	}
}

void PSURemote::off(){
	if (state == true) {
		state = false;
		PORTB |= _BV(PB1);
	}
}

bool PSURemote::isOn(){
	return state;
}

bool PSURemote::isOff(){
	return !state;
}
