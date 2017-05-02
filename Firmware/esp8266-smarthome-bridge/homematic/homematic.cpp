/*
* homematic.cpp
*
*  Created on: 01.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#include "homematic.h"
#include "stdio.h"

Homematic::Homematic()
	: configTimer()
	, pairingTimer()
	, homematicReceiver()
	, IsPairingActive(false)
{
}

void Homematic::Init(cc1101_driver_t *cc1101_driver, cc1101_driver_config_t *cc1101_config)
{
	// Store CC1101 driver callbacks and settings
	this->cc1101_driver = cc1101_driver;
	this->cc1101_config = cc1101_config;

	// Initialize CC1101
	this->cc1101_driver->init(this->cc1101_config);

	// Initialize sub components
	this->homematicReceiver.Init(this);
}

void Homematic::Poll()
{
	if (pairingTimer.IsDone())
	{
		pairingTimer.Set(5000);
		printf("Paring timer reset...\n");
	}
	
	tmpCCBurst = cc1101_driver->detect_burst(cc1101_config);
	if ((tmpCCBurst) && (!chkCCBurst))
	{
		// burst detected for the first time
		chkCCBurst = 1;	// set the flag
	}
	else
	{
		if ((tmpCCBurst) && (chkCCBurst))
		{		// burst detected for the second time
			chkCCBurst = 0;	// reset the flag
			printf("CC1101: Burst detected!\n");
		}
		else
		{
			if ((!tmpCCBurst) && (chkCCBurst))
			{		// secondary test was negative, reset the flag
				chkCCBurst = 0;	// reset the flag	
			}
		}
	}

	if (!chkCCBurst) vTaskDelay(128 / portTICK_PERIOD_MS);
	if (chkCCBurst) vTaskDelay(32 / portTICK_PERIOD_MS);

	uint8_t num = cc1101_driver->rcv_data(cc1101_config, homematicReceiver.Buffer);
	if (num)
	{
		// this->homematicReceiver.MessageBody.Length
		printf("CC1101: %d Bytes read, CRC %s, RSSI %d, Link quality %d\n", num, this->cc1101_config->crc_ok ? "Ok" : "NOk", this->cc1101_config->rssi, this->cc1101_config->lqi);
	}

	// handle send and receive buffer
	if (homematicReceiver.HasData) homematicReceiver.Poll();
}

homematic_handle_t homematic_construct()
{
	Homematic *out(new Homematic());
	return reinterpret_cast<homematic_handle_t>(out);
}

void homematic_destroy(homematic_handle_t handle)
{
	delete(reinterpret_cast<Homematic*>(handle));
}

void homematic_poll(homematic_handle_t handle)
{
	reinterpret_cast<Homematic*>(handle)->Poll();
}

void homematic_init(homematic_handle_t handle, cc1101_driver_t *cc1101_driver, cc1101_driver_config_t *cc1101_config)
{
	reinterpret_cast<Homematic*>(handle)->Init(cc1101_driver, cc1101_config);
}