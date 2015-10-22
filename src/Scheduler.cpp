/*
 * Scheduler.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: Kevin C. Castner
 */

#include <stdio.h>
#include <avr/interrupt.h>

#include "Scheduler.h"

Scheduler::Scheduler() {
	head = 0;

	uint8_t n = 0;
	while (n < MAX_NODES) {
		nodes[n++].inUse = false;
	}
}

void Scheduler::scheduleEvent(const uint16_t interval, const uint8_t event) {
	cli();
	addNode(interval, event);
	sei();
}

void Scheduler::addNode(const uint16_t deltat, const uint8_t event) {
	const uint8_t nbefore = findLastScheduledPrior(deltat);
	const uint8_t nfree = findNextUnused();
	uint8_t nnext = 0;
	nodes[nfree].inUse = true;
	nodes[nfree].tick = deltat;
	nodes[nfree].event = event;

	if (head != 0) {  // list is not empty
		if (nbefore == 0) {  // List not empty but added entry goes first
			nnext = head;
			// fix the chain
			nodes[nfree].next = nnext;
			head = nfree;
			// adjust the ticks
			nodes[nnext].tick = nodes[nnext].tick - totalTicksUntil(nfree);
		} else { // Entry being added goes after the one returned
			nnext = nodes[nbefore].next;
			// fix the chain
			nodes[nfree].next = nnext;
			nodes[nbefore].next = nfree;
			// now adjust the ticks
			nodes[nfree].tick = nodes[nfree].tick - totalTicksUntil(nbefore);
			if (nnext) {
				nodes[nnext].tick = nodes[nnext].tick - nodes[nfree].tick;
			}
		}
	} else {  // This is the only item on the list
		head = nfree;
		nodes[nfree].next = 0;
	}
}

uint8_t Scheduler::removeNode(const uint8_t nodeCurr) {
	const uint8_t nodePrev = findPreviousNode(nodeCurr);
	const uint8_t nodeNext = nodes[nodeCurr].next;

	nodes[nodeCurr].inUse = false;
	if (nodePrev == 0) { // This is the first node in the list
		head = nodes[nodeCurr].next;
	} else { // somewhere in the middle or the end of the list.
		nodes[nodePrev].next = nodeNext;
	}

	// Removing this node so add the delta ticks to the
	// next if the next is valid.
	if (nodeNext != 0) {
		nodes[nodeNext].tick += nodes[nodeCurr].tick;
	}
	return nodeNext;
}

uint8_t Scheduler::findPreviousNode(const uint8_t nodeCurr) {
	uint8_t curr = head;
	uint8_t last = 0;

	while(curr) {
		if (curr == nodeCurr) {
			return last;
		}
		last = curr;
		curr = nodes[curr].next;
	}
	return curr;
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

uint8_t Scheduler::findLastScheduledPrior(const uint16_t deltat) {

	// return of zero indicates that the event currently being
	// scheduled will be the first scheduled event.

	if (head == 0) { // don't go looking if the list is empty.
		return 0;
	}

	uint16_t totalt = 0;
	uint8_t curr = head;
	uint8_t last = 0;
	while (curr != 0) {
		totalt += nodes[curr].tick;
		if (totalt > deltat) {
			return last;
		}

		last = curr;
		curr = nodes[curr].next;
	}
	return last;
}

uint16_t Scheduler::totalTicksUntil(const uint8_t stopNode) {

	// Sum the ticks in the list up to and including the index passed

	if (head == 0) { // don't go looking if the list is empty.
		return 0;
	}

	uint16_t totalt = 0;
	uint8_t curr = head;
	while (curr != 0) {
		totalt += nodes[curr].tick;
		if (curr == stopNode) {
			return totalt;
		}
		curr = nodes[curr].next;
	}
	return 0;
}

bool Scheduler::isDue(void) {
	// isDue gets called from the timerInterrupt
	// decrement the tick and return true when it
	// reaches zero.
	if (head) {
		if (nodes[head].tick) {
			// decrement only if not at zero
			// deals with the case where two events are scheduled
			// for the same tick
			nodes[head].tick --;
		}
		if (nodes[head].tick) {
			return false;
		}
		return true;
	}
	return false;
}

uint8_t Scheduler::getEvent(void) {
	// This function returns the event at the head of the list
	// and removes the node.
	cli();
	uint8_t n = head;
	uint8_t e = nodes[n].event;
	head = nodes[n].next;
	nodes[n].inUse = false;
	sei();
	return e;
}

void Scheduler::removeEvents(void) {
	// When no argument is given, clear all events
	if (head == 0) { // no events to remove if the list is empty.
		return;
	}

	while (head != 0) {
		getEvent();
	}
}

void Scheduler::removeEventType(const uint8_t event) {
	if (head == 0) { // no events to remove if the list is empty.
		return;
	}

	cli();
	uint8_t curr = head;
	while (curr != 0) {
		if (nodes[curr].event == event) {
			curr = removeNode(curr);
		} else {
			curr = nodes[curr].next;
		}
	}
	sei();
}
