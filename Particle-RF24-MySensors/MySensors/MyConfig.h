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

#ifndef MyConfig_h
#define MyConfig_h

/**********************************
*  Message Signing Settings
***********************************/
// Disable to completly disable signing functionality in library
//#define MY_SIGNING_FEATURE

// Define a suitable timeout for a signature verification session
// Consider the turnaround from a nonce being generated to a signed message being received
// which might vary, especially in networks with many hops. 5s ought to be enough for anyone.
#define MY_VERIFICATION_TIMEOUT_MS 5000

// Enable to turn on whitelisting
// When enabled, a signing node will salt the signature with it's unique signature and nodeId.
// The verifying node will look up the sender in a local table of trusted nodes and
// do the corresponding salting in order to verify the signature.
// For this reason, if whitelisting is enabled on one of the nodes in a sign-verify pair, both
// nodes have to implement whitelisting for this to work.
// Note that a node can still transmit a non-salted message (i.e. have whitelisting disabled)
// to a node that has whitelisting enabled (assuming the receiver does not have a matching entry
// for the sender in it's whitelist)
//#define MY_SECURE_NODE_WHITELISTING

// Key to use for HMAC calculation in MySigningAtsha204Soft (32 bytes)
//#define MY_HMAC_KEY 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

/**********************************
*  NRF24L01 Driver Defaults
***********************************/
#define RF24_CE_PIN		   		A0
#define RF24_CS_PIN		   	 	A2
#define RF24_PA_LEVEL 	   	RF24_PA_MAX
#define RF24_PA_LEVEL_GW   	RF24_PA_LOW
// RF channel for the sensor net, 0-127
#define RF24_CHANNEL	   		71
// RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
#define RF24_DATARATE 	   	RF24_1MBPS
// This is also act as base value for sensor nodeId addresses.
#define RF24_BASE_RADIO_ID ((uint64_t)0x4454495400LL)

#endif
