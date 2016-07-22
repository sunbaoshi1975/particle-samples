/*******************************************************************************
SerialCommand - An Arduino library to tokenize and parse commands received over
a serial port.
Copyright (C) 2011-2013 Steven Cogswell  <steven.cogswell@gmail.com>
http://awtfy.com

Version 20131021A.

Version History:
May 11 2011 - Initial version
May 13 2011 -	Prevent overwriting bounds of SerialCommandCallback[] array in addCommand()
			defaultHandler() for non-matching commands
Mar 2012 - Some const char * changes to make compiler happier about deprecated warnings.
           Arduino 1.0 compatibility (Arduino.h header)
Oct 2013 - SerialCommand object can be created using a SoftwareSerial object, for SoftwareSerial
           support.  Requires #include <SoftwareSerial.h> in your sketch even if you don't use
           a SoftwareSerial port in the project.  sigh.   See Example Sketch for usage.
Oct 2013 - Conditional compilation for the SoftwareSerial support, in case you really, really
           hate it and want it removed.
Jul 2016 - By Baoshi Sun
           1. Rewriten to support Particle Photon
           2. Added argument suport to command callback function
					 3. Apply FSM

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
#ifndef SerialCommand_h
#define SerialCommand_h

#include <application.h>

// If you want to use SerialCommand with the hardware serial port only, and want to disable
// SoftwareSerial support, and thus don't have to use "#include <SoftwareSerial.h>" in your
// sketches, then uncomment this define for SERIALCOMMAND_HARDWAREONLY, and comment out the
// corresponding #undef line.
//
// You don't have to use SoftwareSerial features if this is not defined, you can still only use
// the Hardware serial port, just that this way lets you get out of having to include
// the SoftwareSerial.h header.
#define SERIALCOMMAND_HARDWAREONLY 1
//#undef SERIALCOMMAND_HARDWAREONLY

#ifdef SERIALCOMMAND_HARDWAREONLY
#warning "Warning: Building SerialCommand without SoftwareSerial Support"
#endif

#ifndef SERIALCOMMAND_HARDWAREONLY
#include <SoftwareSerial.h>
#endif

#define SERIALCOMMANDBUFFER 32          // Maximum length of a command
#define MAXDELIMETER 2

typedef bool (*PFunc) (const char *cmd);
typedef struct {
	uint8_t state;												// Current State
	uint8_t next;													// Next State
	char event[SERIALCOMMANDBUFFER];			// Event
	PFunc function;												// Action
} StateMachine_t;            // Data structure to hold Command/Handler function key-value pairs

class SerialCommand
{
	public:
		SerialCommand();      // Constructor
		#ifndef SERIALCOMMAND_HARDWAREONLY
		SerialCommand(SoftwareSerial &SoftSer);  // Constructor for using SoftwareSerial objects
		#endif

		void clearBuffer();   // Sets the command buffer to all '\0' (nulls)
		char *first();        // returns pointer to at start of buffer (for getting arguments to commands)
		char *next();         // returns pointer to next token found in command buffer (for getting arguments to commands)
		bool readSerial();    // Main entry point.
		void addDefaultHandler(PFunc = NULL);    			// A handler to call when no valid command received.
		void SetStateMachine(const StateMachine_t *newSM, int sizeSM, uint8_t initState = 0);

  private:
		char inChar;          // A character read from the serial stream
		char buffer[SERIALCOMMANDBUFFER];   // Buffer of stored characters while waiting for terminator character
		int  bufPos;                        // Current position in the buffer
		char delim[MAXDELIMETER];           // null-terminated list of character to be used as delimeters for tokenizing (default " ")
		char *token;                        // Returned token from the command buffer as returned by strtok_r
		char *last;                         // State variable used by strtok_r during processing
		PFunc defaultHandler;               // Pointer to the default handler function
		bool usingSoftwareSerial;           // Used as boolean to see if we're using SoftwareSerial object or not
		#ifndef SERIALCOMMAND_HARDWAREONLY
		SoftwareSerial *SoftSerial;         // Pointer to a user-created SoftwareSerial object
		#endif

  protected:
		virtual bool callbackCommand(const char *cmd) = 0;
		virtual bool callbackDefault(const char *cmd) = 0;

		bool scanStateMachine();
		int findFirstCommand(uint8_t state);

    int numCommand;
    int currentCommand;
		uint8_t currentState;
		uint8_t prevState;
		const StateMachine_t *CommandList;
};

#endif //SerialCommand_h
