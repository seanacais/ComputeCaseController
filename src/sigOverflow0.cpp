/*
 * sigOverflow0.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: seanacais
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Scheduler.h"
extern Scheduler t0;

#include "eventBuffer.h"

//uint8_t TCNT0_VALUE = 128;
uint8_t TCNT0_VALUE = 143;

extern volatile uint32_t g_currentTick;
extern volatile bool g_tickCounterStarted;
extern eventBuffer eb;

// This interrupt needs to fire every 1 mSec
// ATMega 8 has a 1 MHz internal oscillator
// Prescalar of 64 has counter freq of 16.384 KHz
// Full 8 bit give rollover freq of approx 64 Hz (period of 15 mSec)
// Use 128 as Timer Value (TCNT0_VALUE)

#define UP 0x00
#define DOWN 0xFF
#define PRESSED 0x00
#define RELEASED 0xFF

SIGNAL (SIG_OVERFLOW0) {

	g_currentTick++;
	TCNT0 = TCNT0_VALUE;
	static uint8_t switch0_debounce = RELEASED;
	static uint8_t switch0_state = UP;

	switch0_debounce = ((switch0_debounce << 1) | (PINB & 0x01));
	if (switch0_debounce == PRESSED && switch0_state == UP) {
		switch0_state = DOWN;
		eb.addEvent(BUTTON_DOWN);
	}

	if (switch0_debounce == RELEASED && switch0_state == DOWN) {
		switch0_state = UP;
		eb.addEvent(BUTTON_UP);
	}

//	if (t0.isDue() && (g_currentTick % 256)) {
//	if ((g_currentTick % 1024) == 0) {
//		eb.addEvent(SCH_EVENT_READY);
//	}
//	if (t0.isDue() == true) {
//		eb.addEvent(DIAG_LED_ON);
//	}

	if ((g_currentTick % 32) == 0) { // every 32 mSec
		eb.addEvent(BUTTON_ASSESS);
//		PORTB ^= _BV(PB3);
	}

//	if ((g_currentTick % 1024) == 0) {  // every second
//		eb.addEvent(DIAG_LED_BLINK);
//		PORTB ^= _BV(PB3);
//	}

//PORTB ^= _BV(PB3);

	if (t0.isDue() == true) {
		eb.addEvent(SCH_EVENT_READY);
//		PORTB ^= _BV(PB3);
//		PORTB &= ~(_BV(PB3));
	}
//	else {
//		PORTB |= _BV(PB2);
//		PORTB &= ~(_BV(PB2));
//	}
}
