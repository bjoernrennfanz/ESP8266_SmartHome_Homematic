/*
* homematic_sensor.h
*
*  Created on: 06.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#include "homematic.h"

#ifndef HOMEMATIC_SENSOR_H_
#define HOMEMATIC_SENSOR_H_

class HomematicSensor
{
public:
	virtual ~HomematicSensor() {};

	void RegisterInHomematic(uint8_t moduleChannel, uint8_t listIndex, Homematic *pHomematic);
	void ProcessHomematicEvent(uint8_t byte3, uint8_t byte10, uint8_t byte11, uint8_t *data, uint8_t len);

protected:
	// Type definition for module states
	typedef struct {
		static const uint8_t Undefined = 0;
		static const uint8_t DelayOn = 1;
		static const uint8_t RampOn = 2;
		static const uint8_t On = 3;
		static const uint8_t DlyOff = 4;
		static const uint8_t RampOff = 5;
		static const uint8_t Off = 6;
	} ModuleStates;

	// Type definition for down up low battery byte
	typedef struct {
		static const uint8_t None = 0;
		static const uint8_t Up = 0x10;
		static const uint8_t Down = 0x20;
		static const uint8_t Error = 0x30;
		static const uint8_t Delay = 0x40;
		static const uint8_t LowBat = 0x80;
	} ModuleDownUpLowFlags;

	uint8_t moduleState;		// Module status byte, needed for list3 modules to answer status requests
	uint8_t moduleFlags;		// Module down up low battery byte
	uint8_t moduleChannel;		// Holds the channel for the module

	// Pointer to the homematic class
	Homematic *pHomematic;

	virtual void InitEvent() {};
	virtual void SetToggle() {};

	virtual void Poll() = 0;
	virtual void ConfigChangeEvent() = 0;
	virtual void PairSetEvent(uint8_t *data, uint8_t len) = 0;
	virtual void PairStatusRequest() = 0;
	virtual void PeerAddEvent(uint8_t *data, uint8_t len) = 0;
	virtual void PeerMessageEvent(uint8_t byte3, uint8_t *data, uint8_t len) = 0;
};

#endif /* HOMEMATIC_SENSOR_H_ */