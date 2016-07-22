/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */


#include "MyMessage.h"
#include "ArduinoJson.h"

MyMessage::MyMessage() {
	msg.header.destination = 0; // Gateway is default destination
}

MyMessage::MyMessage(uint8_t _sensor, uint8_t _type) {
	msg.header.destination = 0; // Gateway is default destination
	msg.header.sensor = _sensor;
	msg.header.type = _type;
}

bool MyMessage::isAck() const {
	return miGetAck();
}

bool MyMessage::isReqAck() const {
	return miGetRequestAck();
}

uint8_t MyMessage::getSender() const {
	return msg.header.sender;
}

uint8_t MyMessage::getLast() const {
	return msg.header.sender;
}

uint8_t MyMessage::getType() const {
	return msg.header.type;
}

uint8_t MyMessage::getSensor() const {
	return msg.header.sensor;
}

uint8_t MyMessage::getDestination() const {
	return msg.header.destination;
}

uint8_t MyMessage::getCommand() const {
	return miGetCommand();
}

uint8_t MyMessage::getLength() const {
	return miGetLength();
}

uint8_t MyMessage::getVersion() const {
	return miGetVersion();
}

uint8_t MyMessage::getSigned() const {
	return miGetSigned();
}

/* Getters for payload converted to desired form */
void* MyMessage::getCustom() const {
	return (void *)(msg.payload.data);
}

const char* MyMessage::getString() const {
	uint8_t payloadType = miGetPayloadType();
	if (payloadType == P_STRING) {
		return msg.payload.data;
	} else {
		return NULL;
	}
}

// handles single character hex (0 - 15)
char MyMessage::i2h(uint8_t i) const {
	uint8_t k = i & 0x0F;
	if (k <= 9)
		return '0' + k;
	else
		return 'A' + k - 10;
}

char* MyMessage::getCustomString(char *buffer) const {
	for (uint8_t i = 0; i < miGetLength(); i++)
	{
		buffer[i * 2] = i2h(msg.payload.data[i] >> 4);
		buffer[(i * 2) + 1] = i2h(msg.payload.data[i]);
	}
	buffer[miGetLength() * 2] = '\0';
	return buffer;
}

char* MyMessage::getStream(char *buffer) const {
	uint8_t cmd = miGetCommand();
	if ((cmd == C_STREAM) && (buffer != NULL)) {
		return getCustomString(buffer);
	} else {
		return NULL;
	}
}

char* MyMessage::getString(char *buffer) const {
	uint8_t payloadType = miGetPayloadType();
	if (buffer != NULL) {
		if (payloadType == P_STRING) {
			strncpy(buffer, msg.payload.data, miGetLength());
			buffer[miGetLength()] = 0;
		} else if (payloadType == P_BYTE) {
			//itoa(msg.payload.bValue, buffer, 10);
			sprintf(buffer, "%d", msg.payload.bValue);
		} else if (payloadType == P_INT16) {
			//itoa(msg.payload.iValue, buffer, 10);
			sprintf(buffer, "%d", msg.payload.iValue);
		} else if (payloadType == P_UINT16) {
			//utoa(msg.payload.uiValue, buffer, 10);
			sprintf(buffer, "%u", msg.payload.uiValue);
		} else if (payloadType == P_LONG32) {
			//ltoa(msg.payload.lValue, buffer, 10);
			sprintf(buffer, "%ld", msg.payload.lValue);
		} else if (payloadType == P_ULONG32) {
			if( miGetLength() == 8 ) {
				// SBS added 2016-07-21
				PrintUint64(buffer, msg.payload.ui64Value);
			} else {
				//ultoa(msg.payload.ulValue, buffer, 10);
				sprintf(buffer, "%lu", msg.payload.ulValue);
			}
		} else if (payloadType == P_FLOAT32) {
			//dtostrf(fValue,2,fPrecision,buffer);
			sprintf(buffer, "%0.2f", msg.payload.fValue);
		} else if (payloadType == P_CUSTOM) {
			return getCustomString(buffer);
		}
		return buffer;
	} else {
		return NULL;
	}
}

uint8_t MyMessage::getByte() const {
	if (miGetPayloadType() == P_BYTE) {
		return msg.payload.data[0];
	} else if (miGetPayloadType() == P_STRING) {
		return atoi(msg.payload.data);
	} else {
		return 0;
	}
}

bool MyMessage::getBool() const {
	return getInt();
}

float MyMessage::getFloat() const {
	if (miGetPayloadType() == P_FLOAT32) {
		return msg.payload.fValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atof(msg.payload.data);
	} else {
		return 0;
	}
}

long MyMessage::getLong() const {
	if (miGetPayloadType() == P_LONG32) {
		return msg.payload.lValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atol(msg.payload.data);
	} else {
		return 0;
	}
}

unsigned long MyMessage::getULong() const {
	if (miGetPayloadType() == P_ULONG32) {
		return msg.payload.ulValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atol(msg.payload.data);
	} else {
		return 0;
	}
}

int MyMessage::getInt() const {
	if (miGetPayloadType() == P_INT16) {
		return msg.payload.iValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atoi(msg.payload.data);
	} else {
		return 0;
	}
}

unsigned int MyMessage::getUInt() const {
	if (miGetPayloadType() == P_UINT16) {
		return msg.payload.uiValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atol(msg.payload.data);
	} else {
		return 0;
	}

}

// Sun added 2016-07-20
uint64_t MyMessage::getUInt64() const {
	if (miGetPayloadType() == P_ULONG32 && miGetLength() == 8) {
		return msg.payload.ui64Value;
	} else if (miGetPayloadType() == P_STRING) {
		return StringToUInt64(msg.payload.data);
	} else {
		return 0;
	}

}

MyMessage& MyMessage::setSender(uint8_t _sender) {
	msg.header.sender = _sender;
	return *this;
}

MyMessage& MyMessage::setLast(uint8_t _last) {
	msg.header.last = _last;
	return *this;
}

MyMessage& MyMessage::setType(uint8_t _type) {
	msg.header.type = _type;
	return *this;
}

MyMessage& MyMessage::setSensor(uint8_t _sensor) {
	msg.header.sensor = _sensor;
	return *this;
}

MyMessage& MyMessage::setDestination(uint8_t _destination) {
	msg.header.destination = _destination;
	return *this;
}

MyMessage& MyMessage::setVersion(uint8_t _version) {
	miSetVersion(_version);
	return *this;
}

MyMessage& MyMessage::setSigned(uint8_t _signed) {
	miSetSigned(_signed);
	return *this;
}

// Set payload
MyMessage& MyMessage::set(void* value, uint8_t length) {
	miSetPayloadType(P_CUSTOM);
	miSetLength(length);
	memcpy(msg.payload.data, value, min(length, MAX_PAYLOAD));
	return *this;
}

MyMessage& MyMessage::set(const char* value) {
	uint8_t length = min(strlen(value), MAX_PAYLOAD);
	miSetLength(length);
	miSetPayloadType(P_STRING);
	strncpy(msg.payload.data, value, length);
	msg.payload.data[length] = 0; // SBS added 2016-07-19
	return *this;
}

MyMessage& MyMessage::set(uint8_t value) {
	miSetLength(1);
	miSetPayloadType(P_BYTE);
	msg.payload.data[0] = value;
	return *this;
}

MyMessage& MyMessage::set(float value, uint8_t decimals) {
	miSetLength(5); // 32 bit float + persi
	miSetPayloadType(P_FLOAT32);
	msg.payload.fValue=value;
	msg.payload.fPrecision = decimals;
	return *this;
}

MyMessage& MyMessage::set(unsigned long value) {
	miSetPayloadType(P_ULONG32);
	miSetLength(4);
	msg.payload.ulValue = value;
	return *this;
}

MyMessage& MyMessage::set(long value) {
	miSetPayloadType(P_LONG32);
	miSetLength(4);
	msg.payload.lValue = value;
	return *this;
}

MyMessage& MyMessage::set(unsigned int value) {
	miSetPayloadType(P_UINT16);
	miSetLength(2);
	msg.payload.uiValue = value;
	return *this;
}

MyMessage& MyMessage::set(int value) {
	miSetPayloadType(P_INT16);
	miSetLength(2);
	msg.payload.iValue = value;
	return *this;
}

// Sun added 2016-07-20
MyMessage& MyMessage::set(uint64_t value) {
	miSetPayloadType(P_ULONG32);
	miSetLength(8);
	msg.payload.ui64Value = value;
	return *this;
}

// Sun added 2016-05-18
char* MyMessage::getSerialString(char *buffer) const {
	if (buffer != NULL) {
		char payl[MAX_PAYLOAD*2+1];

		sprintf(buffer, "%d;%d;%d;%d;%d;%s\n",
		  msg.header.destination, msg.header.sensor, miGetCommand(),
			miGetRequestAck(), msg.header.type, getString(payl));
		return buffer;
	}

	return NULL;
}

// Sun added 2016-05-26
char* MyMessage::getJsonString(char *buffer) const {
	if (buffer != NULL) {
		char payl[MAX_PAYLOAD*2+1];
		StaticJsonBuffer<256> jBuf;
		JsonObject *jroot;
		jroot = &(jBuf.createObject());
		if( jroot->success() ) {
	    (*jroot)["nd"] = msg.header.destination;
			(*jroot)["sen"] = msg.header.sensor;
			(*jroot)["cmd"] = miGetCommand();
			(*jroot)["ack"] = miGetRequestAck();
			(*jroot)["typ"] = msg.header.type;
			(*jroot)["payl"] = getString(payl);

			jroot->printTo(buffer, 256);
	    return buffer;
	  }
	}

	return NULL;
}
