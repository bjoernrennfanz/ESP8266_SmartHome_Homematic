/*
* homematic.h
*
*  Created on: 01.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#ifndef HOMEMATIC_H_
#define HOMEMATIC_H_

#include "../drivers/cc1101.h"

#ifdef __cplusplus

#include "homematic_receiver.h"
#include "wait_timer.h"
  
class Homematic
{
public:
	Homematic();

	/**
	* @brief Initialize the Homematic module
	*/
	void Init(cc1101_driver_t *cc1101_driver, cc1101_driver_config_t *cc1101_config);

	/**
	* @brief Poll routine for regular operation
	*
	* It takes care of all major tasks like sending 
	* and receiving messages, device configuration
	* and message delegation.
	*/
	void Poll(void);

private:
	// CC1101 driver structs
	cc1101_driver_t *cc1101_driver;
	cc1101_driver_config_t *cc1101_config;

	// Timer functionality
	WaitTimer configTimer;
	WaitTimer pairingTimer;

	// Homematic modules
	HomematicReceiver homematicReceiver;
	uint8_t tmpCCBurst, chkCCBurst;
	uint8_t receiveBuffer[CC1101_DATA_LEN];
 
	// Indication for paring
	bool IsPairingActive;

	struct configFlag_s
	{	// Remember that we are in config mode, for config start message receive
		bool isActive;			// indicates status of poll routine, true is active
		uint8_t channel;		// channel
		uint8_t listIndex;		// list index
		uint8_t peerIndex;		// peer index
	} configFlag;

	struct sliceSendStore_s
	{	// Send peers or register in slices, store for send slice function
		bool isActive;			// indicates status of poll routine, true is active
		bool isPeer;			// is it a peer list message
		bool isRegister;		// or a register send
		uint8_t totalSlices;	// amount of necessary slices to send content
		uint8_t currentSlice;	// counter for slices which are already send
		uint8_t channel;		// indicates channel
		uint8_t listIndex;		// list index
		uint8_t peerIndex;		// peer index
		uint8_t messageCount;	// the message counter
		uint8_t toID[3];		// to whom to send
	} sliceSendStore;

	struct peerSendStore_s {
		bool isActive;			// indicates status of poll routine, 1 is active
		bool needBurstMode;		// burst flag for send function
		bool needAck;			// ack required
		uint8_t sendRetries;	// send retries
		uint8_t messageType;	// message type to build the right message
		uint8_t *payloadPtr;	// pointer to payload
		uint8_t payloadLenght;	// length of payload
		uint8_t channel;		// which channel is the sender
		uint8_t curPeerIndex;	// current peer slots
		uint8_t maxPeerIndex;	// amount of peer slots
		uint8_t slot[8];		// slot measure, all filled in a first step, 
								// if ACK was received, one is taken away by slot
	} peerSendStore;
};

#endif

// Type definitions for C code
typedef void *homematic_handle_t;

// C style prototypes
#ifdef __cplusplus
extern "C"
{
#endif
homematic_handle_t homematic_construct();
void homematic_destroy(homematic_handle_t handle);
void homematic_poll(homematic_handle_t handle);
void homematic_init(homematic_handle_t handle, cc1101_driver_t *cc1101_driver, cc1101_driver_config_t *cc1101_config);
#ifdef __cplusplus  
} // extern "C"  
#endif

#endif /* HOMEMATIC_H_ */