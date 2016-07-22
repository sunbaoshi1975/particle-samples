/*******************************************************************************
SerialCommand - An Arduino library to tokenize and parse commands received over
a serial port.
Copyright (C) 2011-2013 Steven Cogswell  <steven.cogswell@gmail.com>
http://awtfy.com

See SerialCommand.h for version history.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
***********************************************************************************/

#include "xliCommon.h"
#include "SerialCommand.h"

#ifndef SERIALCOMMAND_HARDWAREONLY
#include <SoftwareSerial.h>
#endif

// Constructor makes sure some things are set.
SerialCommand::SerialCommand()
{
	usingSoftwareSerial = false;
	strncpy(delim," ",MAXDELIMETER);  // strtok_r needs a null-terminated string
	numCommand = 0;    								// Number of callback handlers installed
	currentCommand = 0;
	currentState = 0;
	prevState = 0;
	clearBuffer();
	CommandList = NULL;
}

#ifndef SERIALCOMMAND_HARDWAREONLY
// Constructor to use a SoftwareSerial object
SerialCommand::SerialCommand(SoftwareSerial &_SoftSer)
{
	usingSoftwareSerial = true;
	SoftSerial = &_SoftSer;
	strncpy(delim," ",MAXDELIMETER);  // strtok_r needs a null-terminated string
	numCommand = 0;    								// Number of callback handlers installed
	currentCommand = 0;
	currentState = 0;
	prevState = 0;
	clearBuffer();
	CommandList = NULL;
}
#endif


//
// Initialize the command buffer being processed to all null characters
//
void SerialCommand::clearBuffer()
{
	for(int i=0; i<SERIALCOMMANDBUFFER; i++) {
		buffer[i] = 0;
	}
	bufPos=0;
	token = NULL;
}

// Retrieve the first token ("word" or "argument") from the Command buffer.
// returns a NULL if no more tokens exist.
char *SerialCommand::first()
{
	char *firstToken;
	firstToken = strtok_r(buffer, delim, &last);
	return firstToken;
}

// Retrieve the next token ("word" or "argument") from the Command buffer.
// returns a NULL if no more tokens exist.
char *SerialCommand::next()
{
	char *nextToken;
	nextToken = strtok_r(NULL, delim, &last);
	return nextToken;
}

// This checks the Serial stream for characters, and assembles them into a buffer.
// When the terminator character (default '\r') is seen, it starts parsing the
// buffer for a prefix command, and calls handlers setup by addCommand() member
bool SerialCommand::readSerial()
{
	// If we're using the Hardware port, check it.   Otherwise check the user-created SoftwareSerial Port
	#ifdef SERIALCOMMAND_HARDWAREONLY
	while (Serial.available() > 0)
	#else
	while ((usingSoftwareSerial==0 && Serial.available() > 0) || (usingSoftwareSerial==1 && SoftSerial->available() > 0) )
	#endif
	{
		if (!usingSoftwareSerial) {
			// Hardware serial port
			inChar=Serial.read();   // Read single available character, there may be more waiting
		} else {
			#ifndef SERIALCOMMAND_HARDWAREONLY
			// SoftwareSerial port
			inChar = SoftSerial->read();   // Read single available character, there may be more waiting
			#endif
		}

		IF_SERIAL_DEBUG(SERIAL("%c", inChar));   	// Echo back to serial stream

		if (inChar == '\r' || inChar == '\n') {     // Check for the terminator meaning end of command string
			IF_SERIAL_DEBUG(SERIAL_LN("Received: %s", buffer));
			return scanStateMachine();
		} else if (isprint(inChar))	{
			// Only printable characters into the buffer
			buffer[bufPos++] = inChar;   	// Put character into buffer
			if (bufPos > SERIALCOMMANDBUFFER-1) bufPos=0; // wrap buffer around if full
		}
	}

	return true;
}

bool SerialCommand::scanStateMachine()
{
	if( !CommandList )
		return false;

	bool bRunCmd = true;
	boolean matched = false;
	bufPos = 0;           	// Reset to start of buffer

	if( !token ) {
		token = first();   // Search for command at start of buffer
	} else {
		token = next();
	}
	int i = findFirstCommand(currentState);
	for (; i<numCommand; i++) {
		if( currentState != (uint8_t)(CommandList[i].state) )
			break;

	  // Compare the found event against the list of known events in the same state for a match
		if (strlen(CommandList[i].event) == 0 ||
		    strnicmp(token, CommandList[i].event, SERIALCOMMANDBUFFER) == 0)
		{
			currentCommand = i;
			matched = true;
			prevState = currentState;
			currentState = (uint8_t)(CommandList[i].next);			// Change to the next state

			IF_SERIAL_DEBUG(SERIAL_LN("Matched Command: %s in state %d, index=%d", token, currentState, i));

			// Execute the stored handler function for the command
			if( CommandList[i].function ) {
				bRunCmd = (*CommandList[i].function)(token);
			} else {
				bRunCmd = callbackCommand(token);
			}
			clearBuffer();
			break;
		}
	}

	// No macthed item found
	if (!matched) {
		if( defaultHandler ) {
			bRunCmd = (*defaultHandler)(token);
		} else {
			bRunCmd = callbackDefault(token);
		}
		clearBuffer();
	}

	return bRunCmd;
}

int SerialCommand::findFirstCommand(uint8_t state)
{
	int nStart, nMid, nEnd, nFound;

	nStart= 0;
	nEnd = numCommand - 1;
	nFound = numCommand;

	while( nStart <= nEnd ) {
		nMid = (nStart + nEnd) / 2;
		if( state > (uint8_t)(CommandList[nMid].state) ) {
			nStart = nMid + 1;
		} else if( state < (uint8_t)(CommandList[nMid].state) ) {
			nEnd = nMid - 1;
		} else {
			nFound = nMid;
			nEnd = nMid - 1;
		}
	}

	return nFound;
}

void SerialCommand::SetStateMachine(const StateMachine_t *newSM, int sizeSM, uint8_t initState)
{
	CommandList = newSM;
	numCommand = sizeSM;
	currentCommand = 0;
	currentState = initState;
	prevState = initState;
	clearBuffer();

	IF_SERIAL_DEBUG(SERIAL_LN("Setup State Machine of %d items with initial state %d", sizeSM, initState));
}

// This sets up a handler to be called in the event that the receveived command string
// isn't in the list of things with handlers.
void SerialCommand::addDefaultHandler(PFunc function)
{
	defaultHandler = function;
}
