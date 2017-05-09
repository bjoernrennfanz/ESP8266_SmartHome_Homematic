/*
* homematic_sensor.cpp
*
*  Created on: 07.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#define HOMEMATIC_SENSOR_DEBUG

#ifdef HOMEMATIC_SENSOR_DEBUG
#	include <stdio.h>
#	define debug(fmt, ...) printf("%s: " fmt "\n", "BASESENSOR", ## __VA_ARGS__)
#else
#	define debug(fmt, ...)
#endif

#include "homematic_sensor.h"

void HomematicSensor::RegisterInHomematic(uint8_t moduleChannel, uint8_t listIndex, Homematic *pHomematic)
{
	this->pHomematic = pHomematic;
	// this->pHomematic->
	this->moduleChannel = moduleChannel;
}

void HomematicSensor::ProcessHomematicEvent(uint8_t byte3, uint8_t byte10, uint8_t byte11, uint8_t *data, uint8_t len)
{
#ifdef HOMEMATIC_SENSOR_DEBUG
	// Generate hex string from dataPtr
	char hexBuffer[((len * 3) + 1)];
	for (uint8_t i = 0; i < len; i++)
		sprintf(&hexBuffer[i * 3], " %02x", data[i]);

	// Print debug message
	debug("byte3: %d, byte10: %d, data: %s", byte3, byte10, hexBuffer);
#endif

	// Process depending on command types
	if ((byte3 == 0x00) && (byte10 == 0x00))      Poll();
	else if ((byte3 == 0x00) && (byte10 == 0x01)) SetToggle();
	else if ((byte3 == 0x00) && (byte10 == 0x02)) InitEvent();
	else if ((byte3 == 0x01) && (byte11 == 0x06)) ConfigChangeEvent();
	else if ((byte3 == 0x11) && (byte10 == 0x02)) PairSetEvent(data, len);
	else if ((byte3 == 0x01) && (byte11 == 0x0E)) PairStatusRequest();
	else if ((byte3 == 0x01) && (byte11 == 0x01)) PeerAddEvent(data, len);
	else if  (byte3 >= 0x3E)                      PeerMessageEvent(byte3, data, len);
	else return;
}