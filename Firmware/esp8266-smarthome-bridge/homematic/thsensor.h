/*
* thsensor.h
*
*  Created on: 06.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#ifndef THSENSOR_H_
#define THSENSOR_H_

#ifdef __cplusplus

#include "homematic.h"
#include "homematic_sensor.h"

class THSensor : public HomematicSensor
{
private:
	void (*fMeasureCallback)(void);
	uint8_t *pTemperatureValue;
	uint8_t *pHumidityValue;

public:
	void Init(void MeasureFunc(), uint8_t *pTemperatureValue, uint8_t *pHumidityValue);
	
protected:
	void Poll();
	void ConfigChangeEvent();
	void PairSetEvent(uint8_t * data, uint8_t len);
	void PairStatusRequest();
	void PeerAddEvent(uint8_t * data, uint8_t len);
	void PeerMessageEvent(uint8_t byte3, uint8_t * data, uint8_t len);
};

#endif

// Type definitions for C code
typedef void *thsensor_handle_t;

// C style prototypes
#ifdef __cplusplus
extern "C"
{
#endif

thsensor_handle_t thsensor_construct();
void thsensor_destroy(thsensor_handle_t handle);
void thsensor_init(thsensor_handle_t handle, void MeasureFunc(), uint8_t *pTemperatureValue, uint8_t *pHumidityValue);
void thsensor_homematic_register(thsensor_handle_t handle, uint8_t moduleChannel, uint8_t listIndex, homematic_handle_t homematicHandle);

#ifdef __cplusplus  
} // extern "C"  
#endif

#endif /* HOMEMATIC_SENSOR_H_ */