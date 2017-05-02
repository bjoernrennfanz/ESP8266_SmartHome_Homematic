/*
* homematic_receiver.h
*
*  Created on: 01.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#include "homematic.h"

#ifndef HOMEMATIC_RECEIVER_H_
#define HOMEMATIC_RECEIVER_H_

// Maximum length of received bytes
#define MAX_DATA_LEN	60	

class HomematicReceiver
{
	friend class Homematic;

protected:
	struct messageFlags_s 
	{
		uint8_t WKUP: 1;		// 0x01: send initially to keep the device awake
		uint8_t WKMEUP: 1;		// 0x02: awake - hurry up to send messages
		uint8_t CFG: 1;			// 0x04: Device in Config mode
		uint8_t RESERVED: 1;
		uint8_t BURST: 1;		// 0x10: set if burst is required by device
		uint8_t BIDI: 1;		// 0x20: response is expected
		uint8_t RPTED: 1;		// 0x40: repeated (repeater operation)
		uint8_t RPTEN: 1;		// 0x80: set in every message. Meaning?
	};

	struct messageBody_s
	{
		uint8_t       Length;						// message length
		uint8_t       Count;						// counter, if it is an answer counter has to reflect the answered message, otherwise own counter has to be used
		struct messageFlags_s Flags;				// see structure of message flags
		uint8_t       Type;							// type of message
		uint8_t       SenderID[3];					// sender ID
		uint8_t       ReceiverID[3];				// receiver id, broadcast for 0
		uint8_t       Type10;						// type of message
		uint8_t       Type11;						// type of message
		uint8_t       Payload[MAX_DATA_LEN - 12];// payload
	};

public:
	struct messageBody_s MessageBody;				// structure for easier message creation
	uint8_t PeerId[4];								// hold for messages >= 3E the peerID with channel
	uint8_t *Buffer;								// cast to byte array
#	define HasData		Buffer[0] ? 1 : 0			// check if something is in the buffer

private:
#	define BufLen		Buffer[0] + 1
#	define AckRequest	MessageBody.Flags.BIDI		// check if an ACK is requested
	class Homematic *pHomematic;					// pointer to main class for function calls

protected:
	void Init(Homematic *ptrMain);
	void Poll(void);
};

#endif /* HOMEMATIC_RECEIVER_H_ */