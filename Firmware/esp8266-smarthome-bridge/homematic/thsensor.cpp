/*
* thsensor.cpp
*
*  Created on: 07.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#define THSENSOR_DEBUG

#ifdef THSENSOR_DEBUG
#	include <stdio.h>
#	define debug(fmt, ...) printf("%s: " fmt "\n", "THSENSOR", ## __VA_ARGS__)
#else
#	define debug(fmt, ...)
#endif

#include "thsensor.h"

void THSensor::Init(void MeasureFunc(), uint8_t *pTemperatureValue, uint8_t *pHumidityValue)
{
	this->fMeasureCallback = MeasureFunc;
	
	this->pTemperatureValue = pTemperatureValue;
	this->pHumidityValue = pHumidityValue;
}

void THSensor::Poll()
{

}

void THSensor::ConfigChangeEvent()
{

}

void THSensor::PairSetEvent(uint8_t *data, uint8_t len)
{

}

void THSensor::PairStatusRequest()
{

}

void THSensor::PeerAddEvent(uint8_t *data, uint8_t len)
{

}

void THSensor::PeerMessageEvent(uint8_t byte3, uint8_t *data, uint8_t len)
{

}

thsensor_handle_t thsensor_construct()
{
	THSensor *out(new THSensor());
	return reinterpret_cast<thsensor_handle_t>(out);
}

void thsensor_destroy(thsensor_handle_t handle)
{
	delete(reinterpret_cast<THSensor*>(handle));
}

void thsensor_init(thsensor_handle_t handle, void MeasureFunc(), uint8_t *pTemperatureValue, uint8_t *pHumidityValue)
{
	reinterpret_cast<THSensor*>(handle)->Init(MeasureFunc, pTemperatureValue, pHumidityValue);
}

void thsensor_homematic_register(thsensor_handle_t handle, uint8_t moduleChannel, uint8_t listIndex, homematic_handle_t homematicHandle)
{
	reinterpret_cast<THSensor*>(handle)->RegisterInHomematic(moduleChannel, listIndex, reinterpret_cast<Homematic*>(homematicHandle));
}
