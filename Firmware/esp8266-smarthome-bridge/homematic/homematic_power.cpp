/*
* homematic_power.cpp
*
*  Created on: 05.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#define HOMEMATIC_POWER_DEBUG

#ifdef HOMEMATIC_POWER_DEBUG
#	include <stdio.h>
#	define debug(fmt, ...) printf("%s: " fmt "\n", "POWER", ## __VA_ARGS__)
#else
#	define debug(fmt, ...)
#endif

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "homematic_power.h"

void HomematicPower::SetMode(uint8_t mode)
{
	powerMode = mode;
	debug("PowerMode: %d", powerMode);		// ...and some information
	
	// Stay alive for 10 seconds
	StayAwake(10000);
}

void HomematicPower::StayAwake(uint16_t time)
{
	// Set new timeout only if we have to add something
	if (time < stayAwakeTimer.Remain())
	{
		return;
	}

	stayAwakeTimer.Set(time);
}

void HomematicPower::Init(Homematic *pHomematic)
{
	this->pHomematic = pHomematic;
	this->powerMode = 0;
}

void HomematicPower::Poll(void)
{
	if (powerMode == 0) 
		return;	// No power savings, there for we can exit

	if (!stayAwakeTimer.IsDone()) 
		return;	// Timer is still active, jump out
																			
	if ((pHomematic->sliceSendStore.isActive) || (pHomematic->configFlag.isActive) || (pHomematic->IsPairingActive)) 
		return; // Some communication still active, jump out

	if (powerMode == 1)
	{
		tmpCCBurst = pHomematic->cc1101_driver->detect_burst(pHomematic->cc1101_config);
		if ((tmpCCBurst) && (!checkCCBurst))
		{
			// Burst detected for the first time
			checkCCBurst = 1;	// set the flag
		}
		else
		{
			if ((tmpCCBurst) && (checkCCBurst))
			{	
				// Burst detected for the second time
				checkCCBurst = 0;	// reset the flag
				debug("%s", "Burst detected!");
				
				StayAwake(500);
			}
			else
			{
				if ((!tmpCCBurst) && (checkCCBurst))
				{		
					// Secondary test was negative, reset the flag
					checkCCBurst = 0;
				}
			}
		}
	}

	if ((powerMode == 1) && (!checkCCBurst))
	{
		debug("%s", "Sleep for next 250ms");
		vTaskDelay(250 / portTICK_PERIOD_MS);
	}

	if ((powerMode == 1) && (checkCCBurst))
	{
		debug("%s", "Sleep for next 32ms");
		vTaskDelay(32 / portTICK_PERIOD_MS);
	}

	if (powerMode == 2)
	{
		debug("%s", "Sleep for next 250ms");
		vTaskDelay(250 / portTICK_PERIOD_MS);
	}

	if (powerMode == 3)
	{
		debug("%s", "Sleep for next 8s");
		vTaskDelay(8000 / portTICK_PERIOD_MS);
	}
}