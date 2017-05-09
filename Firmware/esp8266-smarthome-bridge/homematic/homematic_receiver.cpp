/*
* homematic_receiver.cpp
*
*  Created on: 01.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#define HOMEMATIC_RECEIVER_DEBUG

#ifdef HOMEMATIC_RECEIVER_DEBUG
#	include <stdio.h>
#	define debug(fmt, ...) printf("%s: " fmt "\n", "RECEIVER", ## __VA_ARGS__)
#else
#	define debug(fmt, ...)
#endif

#include <string.h>

#include "homematic_receiver.h"

void HomematicReceiver::Init(Homematic *pHomematic)
{
	this->Buffer = (uint8_t*)&this->MessageBody;
	this->pHomematic = pHomematic;
}

void HomematicReceiver::Poll()
{
	static uint8_t lastReadCount;

	// Check if packet with payload was received
	if (this->BufLen > 10) 
	{	
		// Create search string for peer id
		memcpy(this->PeerId, this->MessageBody.ReceiverID, 3);

		// Mask out long and battery low
		this->PeerId[3] = (this->Buffer[10] & 0x3f);
	}

	// filter out repeated messages
	if ((this->MessageBody.Flags.RPTED) && (lastReadCount == this->MessageBody.Length)) 
	{	// check if message was already received

		debug("%s", "Repeated message, droping.");
		
		// clear receive buffer
		this->MessageBody.Length = 0;
		
		// wait for next message
		return;	
	}

	lastReadCount = this->MessageBody.Length;
}