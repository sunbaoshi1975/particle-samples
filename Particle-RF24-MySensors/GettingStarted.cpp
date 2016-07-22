/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

//
// Hardware configuration
// from https://github.com/mshoemaker/SparkCore-RF24

/*
  PINOUTS
  http://docs.spark.io/#/firmware/communication-spi
  http://maniacbug.wordpress.com/2011/11/02/getting-started-rf24/

  SPARK CORE    SHIELD SHIELD    NRF24L01+
  GND           GND              1 (GND)
  3V3 (3.3V)    3.3V             2 (3V3)
  *A0 (CSN)      9  (D6)          3 (CE)
  A2 (SS)       10 (SS)          4 (CSN)
  A3 (SCK)      13 (SCK)         5 (SCK)
  A5 (MOSI)     11 (MOSI)        6 (MOSI)
  A4 (MISO)     12 (MISO)        7 (MISO)
  *A1(IRQ)      PC3(IRQ)          8(IRQ)

  NOTE: Also place a 10-100uF cap across the power inputs of
        the NRF24L01+.  I/O o fthe NRF24 is 5V tolerant, but
        do NOT connect more than 3.3V to pin 2(3V3)!!!
 */

#include "xliCommon.h"
#include "MyParserSerial.h"
#include "MyTransportNRF24.h"
#include "xlxSerialConsole.h"

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
//bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins D6 & A2 */
//RF24 radio(D6,A2);
//RF24 radio(A0,A2);
//MyTransportNRF24 radio(D6,A2);
MyTransportNRF24 radio(A0,A2);
/**********************************************************/

uint64_t networkID;

// The role of the current running sketch
//role_e role = role_node; // Start as a Node
role_e role = role_idle;
UC nodeID = 0;
bool bGatewayAvailable = false;

String mLastMessage;
MyMessage msg;
MyParserSerial msgParser;
UC *msgData = (UC *)&(msg.msg);

unsigned long times = 0;
unsigned long succ = 0;
unsigned long received = 0;

bool ProcessSend();
bool ProcessReceive();
UC GetNextAvailableNodeId();
uint64_t GetNetworkID();

int CloudCommand(String data) {
  char strDisplay[128];

  data.toUpperCase();
  if ( data == "T" && role != role_node )
  {
    mLastMessage = "Transmit Role (Node)";
    SERIAL_LN("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK");

    // Become the primary transmitter (ping out)
    ChangeRole(role_auto);
    //radio.openWritingPipe(pipes[0]);
    //radio.openReadingPipe(1,pipes[1]);
    //radio.stopListening();
  }
  else if ( data == "R" && role != role_gw )
  {
    mLastMessage = "Receive Role (Gateway)";
    SERIAL_LN("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK");

    // Become the primary receiver (pong back)
    ChangeRole(role_gw);
    //radio.openWritingPipe(pipes[1]);
    //radio.openReadingPipe(1,pipes[0]);
    //radio.startListening();
  }
  else if ( data == "Q" && role != role_idle )
  {
    ChangeRole(role_idle);
    mLastMessage = "Idle mode";
    SERIAL_LN("**STOP WORKING - PRESS 'T' or 'R' TO RESTORE");

    //radio.stopListening();
  }
  else if( data == "D" )
  {
    if( times > 0 ) {
      mLastMessage = String::format("Succ sent %lu in %lu times. Succ-rate: %.2f%%, received %lu",
      succ, times, (float)succ * 100 / times, received);
    } else {
      mLastMessage = "No transition";
    }
    SERIAL_LN(mLastMessage);
  }
  else if( data == "N" )
  {
    mLastMessage = String::format("Current NetworkID: %s", PrintUint64(strDisplay, radio.getCurrentNetworkID()));
    mLastMessage += String::format(", Private NetworkID: %s", PrintUint64(strDisplay, radio.getMyNetworkID()));
    SERIAL_LN(mLastMessage);
  }
  return 0;
}

// Make NetworkID with the right 4 bytes of device MAC address
uint64_t GetNetworkID()
{
  uint64_t netID = 0;

  byte mac[6];
  WiFi.macAddress(mac);
  for (int i=2; i<6; i++) {
    netID += mac[i];
    netID <<= 8;
  }

  return netID;
}

void setup() {
  char strDisplay[128];

  Serial.begin(115200);
  SERIAL_LN(F("Particle-RF24/MySensorTest"));

  Particle.variable("LastMessage", &mLastMessage, STRING);
  Particle.function("CloudCmd", CloudCommand);

  // Initialize Serial Console
  theConsole.Init();

  networkID = RF24_BASE_RADIO_ID;
  bGatewayAvailable = false;
  //networkID = GetNetworkID();
  //SERIAL_LN("Got NetworkID: %s", PrintUint64(strDisplay, networkID));
  if( !radio.init() ) {
    mLastMessage = "Radio is not valid!";
    SERIAL_LN(mLastMessage);
    return;
  }

  //radio.setRetries(3, 3);
  //radio.setPALevel(RF24_PA_MAX);
  //radio.setAddress(AUTO, networkID);

  while( Time.now() < 2000 ) {
    Particle.process();
  }

  SERIAL_LN("Particle-RF24 MySensorTest Application started.");
  mLastMessage = "Working as Node, PRESS 'R' to become Gateway";
  theConsole.doHelp("");
}

void loop()
{
  if( radio.isValid() )
  {
    switch(role)
    {
    case role_node:
    case role_gw:
    case role_auto:
      ProcessReceive();
      break;

    case role_idle:
      //SERIAL_LN("idle in loop...");
      delay(1000);
      break;
    }
  }

  // Process Console Command
  theConsole.processCommand();

  delay(10);
} // Loop

void ChangeRole(role_e _role, bool _force)
{
  if( role != _role || _force ) {
    role = _role;
    if( _role == role_gw ) {
      networkID = GetNetworkID();
      radio.setAddress(GATEWAY_ADDRESS, networkID);
      bGatewayAvailable = true;
      //radio.switch2BaseNetwork();
      //SERIAL_LN("Switched to base network\n\r");
    } else if( _role == role_node && nodeID <= MAX_NODE_ID ){
      bGatewayAvailable = false;
      radio.setAddress(nodeID, networkID);
    } else if( _role == role_auto ) {
      bGatewayAvailable = false;
      nodeID = AUTO;
      radio.setAddress(AUTO, networkID);
      radio.switch2BaseNetwork();
      SERIAL_LN("Switched to base network\n\r");
    }

    SERIAL_LN(F("Role changed to %d NodeID: %d"), _role, nodeID);
  }
}

bool ProcessSend(String &strMsg)
{
  bool sentOK = false;
  bool bMsgReady = false;
  int iValue;
  float fValue;
  char strBuffer[64];

  int nPos = strMsg.indexOf(':');
  uint8_t lv_nNodeID;
  uint8_t lv_nMsgID;
  if( nPos > 0 ) {
    // Extract NodeID & MessageID
    lv_nNodeID = (uint8_t)(strMsg.substring(0, nPos).toInt());
    lv_nMsgID = (uint8_t)(strMsg.substring(nPos+1).toInt());
  } else {
    // Parse serial message
    lv_nMsgID = 0;
  }

  switch( lv_nMsgID )
  {
  case 0: // Free style
    iValue = min(strMsg.length(), 63);
    strncpy(strBuffer, strMsg.c_str(), iValue);
    strBuffer[iValue] = 0;
    // Serail format to MySensors message structure
    bMsgReady = msgParser.parse(msg, strBuffer);
    if( bMsgReady ) {
      SERIAL("Now sending message...");
    }
    break;

  case 1:   // Request new node ID
    if( role == role_gw) {
      SERIAL_LN("Controller can not request node ID\n\r");
    } else {
      ChangeRole(role_auto);
      msg.build(AUTO, BASESERVICE_ADDRESS, NODE_SENSOR_ID, C_INTERNAL, I_ID_REQUEST, false);
      msg.set("DTIT-is-great");     // Optional Key
      bMsgReady = true;
      SERIAL("Now sending request node id message...");
    }
    break;

  case 2:   // Lamp present, req ack
    msg.build(nodeID, lv_nNodeID, NODE_SENSOR_ID, C_PRESENTATION, S_LIGHT, true);
    msg.set("Found Sunny");
    bMsgReady = true;
    SERIAL("Now sending lamp present message...");
    break;

  case 3:   // Temperature sensor present with sensor id 1, req no ack
    msg.build(nodeID, lv_nNodeID, 1, C_PRESENTATION, S_TEMP, false);
    msg.set("");
    bMsgReady = true;
    SERIAL("Now sending DHT11 present message...");
    break;

  case 4:   // Temperature set to 23.5, req no ack
    msg.build(nodeID, lv_nNodeID, 1, C_SET, V_TEMP, false);
    fValue = 23.5;
    msg.set(fValue, 2);
    bMsgReady = true;
    SERIAL("Now sending set temperature message...");
    break;

  case 5:   // Humidity set to 45, req no ack
    msg.build(nodeID, lv_nNodeID, 1, C_SET, V_HUM, false);
    iValue = 45;
    msg.set(iValue);
    bMsgReady = true;
    SERIAL("Now sending set humidity message...");
    break;
  }

  if( bMsgReady ) {
    times++;

    // Determine the receiver addresse
    /// If I'm the gateway or the gateway is not available, send to the destination directly.
    /// Otherwise, send to the gateway, and let the gateway take care of the rest
    uint8_t lv_sendTo = GATEWAY_ADDRESS;;
    if( role == role_gw || role == role_auto ||
        (role == role_node && !bGatewayAvailable) ) {
      lv_sendTo = msg.getDestination();
    }
    SERIAL("to %d...", lv_sendTo);

    sentOK = radio.send(lv_sendTo, msg);
    if( sentOK )
    {
      succ++;
      SERIAL_LN("OK");
      /*
      if( lv_nMsgID == 1 ) {
        radio.switch2MyNetwork();
        SERIAL_LN("Switched to private network");
      }*/
    } else {
      SERIAL_LN("failed");
    }
  }

  return sentOK;
}

bool ProcessReceive()
{
  bool sentOK = false;
  uint8_t to = 0;
  uint8_t pipe;
  if (!radio.available(&to, &pipe))
    return false;

  uint8_t len = radio.receive(msgData);
  if( len < HEADER_SIZE )
  {
    SERIAL_LN("got corrupt dynamic payload!");
    return false;
  } else if( len > MAX_MESSAGE_LENGTH )
  {
    SERIAL_LN("message length exceeded: %d", len);
    return false;
  }

  char strDisplay[SENSORDATA_JSON_SIZE];
  received++;
  SERIAL_LN("Received from pipe %d msg-len=%d, from:%d to:%d dest:%d cmd:%d type:%d sensor:%d payl-len:%d",
        pipe, len, msg.getSender(), to, msg.getDestination(), msg.getCommand(),
        msg.getType(), msg.getSensor(), msg.getLength());
  memset(strDisplay, 0x00, sizeof(strDisplay));
  msg.getJsonString(strDisplay);
  SERIAL_LN("  JSON: %s, len: %d", strDisplay, strlen(strDisplay));
  memset(strDisplay, 0x00, sizeof(strDisplay));
  SERIAL_LN("  Serial: %s, len: %d", msg.getSerialString(strDisplay), strlen(strDisplay));

  switch( msg.getCommand() )
  {
    case C_INTERNAL:
      if( msg.getType() == I_ID_REQUEST && msg.getSender() == AUTO ) {
        // On ID Request message
        /// Get new ID
        UC newID = GetNextAvailableNodeId();
        UC replyTo = msg.getSender();
        /// Send response message
        msg.build(nodeID, replyTo, newID, C_INTERNAL, I_ID_RESPONSE, false);
        msg.set(networkID);
        SERIAL("Now sending NodeId response message to %d with new NodeID:%d, NetworkID:%s...", replyTo, newID, PrintUint64(strDisplay, networkID));
        times++;
        sentOK = radio.send(replyTo, msg, pipe);
        if( sentOK ) {
          succ++;
          SERIAL_LN("OK");
        } else {
          SERIAL_LN("failed");
        }
      } else if( msg.getType() == I_ID_RESPONSE ) {
        if( msg.getSensor() > MAX_NODE_ID ) {
            SERIAL_LN("Node Table is full!");
        } else {
          networkID = msg.getUInt64();
          bool bForce = (nodeID != msg.getSensor());
          if( bForce ) { nodeID = msg.getSensor(); }
          bGatewayAvailable = true;
          SERIAL_LN("Get NodeId: %d, networkId: %s", nodeID, PrintUint64(strDisplay, networkID));
          ChangeRole(role_node, bForce);
        }
      }
      break;

    default:
      break;
  }

  return true;
}

// Just for testing now
UC GetNextAvailableNodeId()
{
  // ToDo: maintain a ID table (e.g. DevStatus_t[n])
  static UC nextID = 0;
  if( ++nextID > MAX_NODE_ID ) {
    nextID = 1;
  }

  return nextID;
}
