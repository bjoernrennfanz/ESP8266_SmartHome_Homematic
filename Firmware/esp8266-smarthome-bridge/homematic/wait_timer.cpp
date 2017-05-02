/*
* wait_timer.cpp
*
*  Created on: 01.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#include "wait_timer.h"

/**
* @brief Query if the timer has expired
*
* @return false if timer is still running, true if not.
*         If the timer was never set(), return value is 1
*/
bool WaitTimer::IsDone(void) 
{
	if (!isArmed)
	{   // not armed, so nothing to do
		return true; 
	}

	if ((xTaskGetTickCount() - startTime) < checkTime)
	{	// not ready yet
		return false;
	}

	// if we are here, timeout was happened
	// next loop status "done" will indicated
	isArmed = false;
	checkTime = 0;

	return true;
}

/**
* @brief Start the timer
*
* @param ms Time until timer is IsDone() (unit: ms)
*/
void WaitTimer::Set(uint32_t ms) 
{
	isArmed = ms ? true : false;
	if (isArmed)
	{
		startTime = xTaskGetTickCount();
		checkTime = ms / portTICK_PERIOD_MS;
	}
}

/**
* @brief Query the remaing time until the timer is done
*
* @return Time until timer is done() (unit: ms)
*/
uint32_t WaitTimer::Remain(void) 
{
	if (!isArmed)
	{
		return 0;
	}

	return (uint32_t)((checkTime - (xTaskGetTickCount() - startTime)) * portTICK_PERIOD_MS);
}