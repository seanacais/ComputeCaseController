/*
 * PSURemote.cpp
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Sep 29, 2015
 *      Author: Kevin C. Castner
 */

#ifndef SRC_PSUREMOTE_H_
#define SRC_PSUREMOTE_H_

#define PSU_ON  0x01
#define PSU_OFF 0x00

class PSURemote {
private:
	uint8_t psu_remote_state;
public:
	PSURemote();
	void psu_off();
	void psu_on();
	uint8_t psu_state();
};

#endif /* SRC_PSUREMOTE_H_ */
