/*
 * PSURemote.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 29, 2015
 *      Author: Kevin C. Castner
 */

#include <avr/io.h>
#include "PSURemote.h"

PSURemote::PSURemote() : psu_remote_state(PSU_OFF) {
	DDRB |= (_BV(PB1)); // Set PB1 to an output (PSURC)
	off();              // Start with the PSU off
}

void PSURemote::on(){
	if (psu_remote_state == PSU_OFF) {
		psu_remote_state = PSU_ON;
		PORTB &= ~(_BV(PB1));
	}
}

void PSURemote::off(){
	if (psu_remote_state == PSU_ON) {
		psu_remote_state = PSU_OFF;
		PORTB |= _BV(PB1);
	}
}

uint8_t PSURemote::state(){
	return psu_remote_state;
}
