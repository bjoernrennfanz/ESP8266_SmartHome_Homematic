/*
* homematic_receiver.cpp
*
*  Created on: 01.05.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#include <stdio.h>
#include <string.h>

#include "homematic_receiver.h"

void HomematicReceiver::Init(Homematic *ptrHomematic)
{
	this->Buffer = (uint8_t*)&this->MessageBody;
	this->pHomematic = ptrHomematic;
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

		printf("RECV: repeated message\n");
		
		// clear receive buffer
		this->MessageBody.Length = 0;
		
		// wait for next message
		return;	
	}

	lastReadCount = this->MessageBody.Length;
}