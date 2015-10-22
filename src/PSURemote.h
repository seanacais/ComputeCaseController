/*
 * PSURemote.h
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Oct 20, 2015
 *      Author: Kevin C. Castner
 */

#ifndef SRC_PSUREMOTE_H_
#define SRC_PSUREMOTE_H_

class PSURemote {
private:
	bool  state;
public:
	PSURemote();
	void off();
	void on();
	bool isOn();
	bool isOff();
};

#endif /* SRC_PSUREMOTE_H_ */
