/**
 * MyParserJson.cpp - Xlight extention for MySensors message lib
 *
 * Created by Baoshi Sun <bs.sun@datatellit.com>
 * Copyright (C) 2015-2016 DTIT
 * Full contributor list:
 *
 * Documentation:
 * Support Forum:
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Created by Baoshi Sun <bs.sun@datatellit.com>
 *
 * DESCRIPTION
 * Parse Json string into message object
 *
 * ToDo:
 * 1.
**/

#include "MyParser.h"
#include "MyParserJson.h"
#include "ArduinoJson.h"
#include "MyTransport.h"

MyParserJson::MyParserJson() : MyParser() {}

bool MyParserJson::parse(MyMessage &message, char *inputString) {
	const char *str, *p, *value=NULL;
	uint8_t bvalue[MAX_PAYLOAD];
	uint8_t blen = 0;
	int i = 0;
	uint8_t command = 0;
	uint8_t ack = 0;

	// Extract command data coming with the JSON string
  StaticJsonBuffer<MAX_MESSAGE_LENGTH*2> jsonBuf;
  JsonObject& root = jsonBuf.parseObject((char *)inputString);
  if( !root.success() )
    return false;
  if( root.size() < 5 )  // Check for invalid input
    return false;

  message.setDestination((uint8_t)atoi(root["nd"]));
  message.setSensor((uint8_t)atoi(root["sen"]));
  command = atoi(root["cmd"]);
  mSetCommand(message.msg, command);
  ack = atoi(root["ack"]);
  message.setType((uint8_t)atoi(root["typ"]));

  // Payload
  str = root["payl"].asString();
  if (command == C_STREAM) {
    blen = 0;
    uint8_t val;
    while (*str) {
      val = h2i(*str++) << 4;
      val += h2i(*str++);
      bvalue[blen] = val;
      blen++;
    }
  } else {
    value = str;
  }

	message.setSender( GATEWAY_ADDRESS );
	message.setLast( GATEWAY_ADDRESS );
  mSetRequestAck(message.msg, ack?1:0);
  mSetAck(message.msg, false);
	if (command == C_STREAM)
		message.set(bvalue, blen);
	else
		message.set(value);
	return true;
}

// Sun added 2016-05-26
char* MyParserJson::getJsonString(MyMessage &message, char *buffer) const {
	if (buffer != NULL) {
		char payl[MAX_PAYLOAD*2+1];
		StaticJsonBuffer<256> jBuf;
		JsonObject *jroot;
		jroot = &(jBuf.createObject());
		if( jroot->success() ) {
	    (*jroot)["nd"] = message.getDestination();
			(*jroot)["sen"] = message.getSensor();
			(*jroot)["cmd"] = mGetCommand(message.msg);
			(*jroot)["ack"] = mGetRequestAck(message.msg);
			(*jroot)["typ"] = message.getType();
			(*jroot)["payl"] = message.getString(payl);

			jroot->printTo(buffer, 256);
	    return buffer;
	  }
	}

	return NULL;
}
