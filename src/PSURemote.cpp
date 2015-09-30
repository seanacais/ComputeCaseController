/*
 * PSURemote.cpp
 *
 *  Created on: Sep 29, 2015
 *      Author: seanacais
 */

#include <avr/io.h>

#include "PSURemote.h"

PSURemote::PSURemote() : psu_remote_state(PSU_OFF) {
}

void PSURemote::psu_on(){
	if (psu_remote_state == PSU_OFF) {
		psu_remote_state = PSU_ON;
		PORTB &= ~(_BV(PB1));
	}
}

void PSURemote::psu_off(){
	if (psu_remote_state == PSU_ON) {
		psu_remote_state = PSU_OFF;
		PORTB |= _BV(PB1);
	}
}
