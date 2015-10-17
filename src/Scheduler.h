/*
 * Scheduler.h
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 26, 2015
 *      Author: seanacais
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include <avr/io.h>

class Scheduler {
	/* This class implements a scheduled event list
	 */

#define MAX_NODES 48

private:
	struct Node {
		uint16_t tick;
		uint8_t event;
		bool inUse;
		uint8_t next;
	};

	Node nodes[MAX_NODES];
	volatile uint8_t head;

	uint8_t findNextUnused();
	void addNode(const uint16_t, const uint8_t);
	uint8_t findLastScheduledPrior(const uint16_t);
	uint16_t totalTicksUntil(const uint8_t);

public:
	Scheduler();
	void scheduleEvent(const uint16_t, const uint8_t);
	bool isDue(void);
	uint8_t getEvent(void);
};

#endif /* SRC_SCHEDULER_H_ */

/*
 * $Log: $
 *
 * static char const copyright[] =
 *   "Kevin C. Castner, 2015, All Rights Reserved";
 * static char const rcsid[] = "$Id: $";
 * static char const rcsversion[] = "$Revision: $";
 */
