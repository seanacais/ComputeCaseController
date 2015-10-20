/*
 * VoltMeter.h
 *
 *  Copyright (c) Kevin C. Castner. 2015; All rights reserved.
 *  Created on: Oct 20, 2015
 *      Author: Kevin C. Castner
 */

#ifndef SRC_VOLTMETER_H_
#define SRC_VOLTMETER_H_

class VoltMeter {
private:
	;
public:
	VoltMeter();
	void on(void);
	void off(void);
	void toggle(void);
	void blink(void);
	void blink_2(void);
	void blink_3(void);
};

#endif /* SRC_VOLTMETER_H_ */
