/*
 * Scheduler.cpp
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
#include <avr/interrupt.h>

#include "eventBuffer.h"

extern volatile uint32_t g_currentTick;
extern volatile bool g_tickCounterStarted;
extern eventBuffer eb;

#include "Scheduler.h"

Scheduler::Scheduler() {
	head = 0;

	uint8_t n = 0;
	while (n < MAX_NODES) {
		nodes[n++].inUse = false;
	}
}

void Scheduler::scheduleEvent(const uint32_t interval, const uint8_t event) {
	addNode(interval, event);
}

void Scheduler::addNode(const uint32_t interval, const uint8_t event) {
	cli();
	uint8_t nfree = findNextUnused();
	nodes[nfree].inUse = true;
	nodes[nfree].tick = (interval + g_currentTick);
	nodes[nfree].event = event;

	if (head != 0) {  // list is not empty
		uint8_t nbefore = findLastScheduledPrior(nodes[nfree].tick);
		if (nbefore == 0) {  // Not empty but this goes first to the head
			nodes[nfree].next = head;
			head = nfree;
		} else { // This goes after the one returned
			nodes[nfree].next = nodes[nbefore].next;
			nodes[nbefore].next = nfree;
		}
	} else {  // This is the only item on the list
		head = nfree;
		nodes[nfree].next = 0;
	}
	sei();
}

uint8_t Scheduler::findNextUnused() {
	uint8_t n = 1;
	while (n < MAX_NODES) {
		if (nodes[n].inUse == false) {
			return n;
		}
		n++;
	}
	return 0;
}

uint8_t Scheduler::findLastScheduledPrior(const uint32_t notAfter) {
	if (head == 0) { // don't go looking if the list is empty.
		return 0;
	}

	uint8_t n = head;
	uint8_t l = 0;
	while (n != 0) {
		if (nodes[n].tick > notAfter) {
			return l;
		}

		if (nodes[n].next == 0) {
			return n;
		}
		l = n;
		n = nodes[n].next;
	}
	return n;
}

bool Scheduler::isDue(void) {
	if ((head != 0) && (g_currentTick >= nodes[head].tick)) {
		return true;
	}
	return false;
}

uint8_t Scheduler::getEvent(void) {
	cli();
	uint8_t n = head;
	uint8_t e = nodes[n].event;
	head = nodes[n].next;
	nodes[n].inUse = false;
	sei();
	return e;
}
/*
 *
 * $Log: $
 */
