/*
 * wait_timer.h
 *
 *  Created on: 01.05.2017
 *      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 *
 *  @short Timer class for non-blocking delays
 *
 *  The following examples shows how to use the waitTimer class to
 *  perform an action every 500ms. Note that the first time loop()
 *  is called, delay.done() will return true and the action will
 *  be performed. 
 *
 *  @attention The waitTimer can only make sure a minimum time passes
 *  between set() and done(). If calls to done() are delayed due to other
 *  actions, more time may pass. Also, actual delay times strongly depend
 *  on the behaviour of the system clock.
 *
 *  @see http://www.gammon.com.au/forum/?id=12127
 */

#ifndef WAIT_TIMER_H_
#define WAIT_TIMER_H_

#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>

class WaitTimer 
{
private:
	bool isArmed;
	TickType_t checkTime;
	TickType_t startTime;

public:
	bool IsDone(void);
	void Set(uint32_t ms);
	uint32_t Remain(void);
};

#endif /* WAIT_TIMER_H_ */