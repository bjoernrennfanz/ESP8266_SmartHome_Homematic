/*
* homematic_power.h
*
*  Created on: 05.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#include "homematic.h"
#include "wait_timer.h"

#ifndef HOMEMATIC_POWER_H_
#define HOMEMATIC_POWER_H_

class HomematicPower
{
	friend class Homematic;

private:
	class Homematic *pHomematic;					// pointer to main class for function calls
	WaitTimer stayAwakeTimer;

protected:
	uint8_t powerMode;
	uint8_t comStat;

	uint8_t checkCCBurst;
	uint8_t tmpCCBurst;

public:
	void SetMode(uint8_t mode);
	void StayAwake(uint16_t time);

	void Init(Homematic *pHomematic);
	void Poll(void);

};
#endif /* HOMEMATIC_POWER_H_ */