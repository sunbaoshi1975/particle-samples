//  xlxSerialConsole.h - Xlight Device Management Console via Serial Port

#ifndef xlxSerialConsole_h
#define xlxSerialConsole_h

#include "SerialCommand.h"
#include "xliCommon.h"

#include "MyParserSerial.h"
#include "MyTransportNRF24.h"
extern MyTransportNRF24 radio;

class SerialConsoleClass : public SerialCommand
{
protected:
  virtual bool callbackCommand(const char *cmd);
  virtual bool callbackDefault(const char *cmd);

public:
  SerialConsoleClass();

  void Init();
  bool processCommand();

  //--------------------------------------------------
  // Command Functions
  //--------------------------------------------------
  // help: Help commands
  bool showThisHelp(String &strTopic);
  bool doHelp(const char *cmd);

  // check - Check commands
  bool doCheck(const char *cmd);

  // show - Show commands: config, status, variable, statistic data, etc.
  bool doShow(const char *cmd);

  // ping - ping IP address or hostname, shortcut of test ping
  bool doPing(const char *cmd);

  // do - execute action, e.g. turn on the lamp
  bool doAction(const char *cmd);

  // test - Test commands, e.g. ping, send
  bool doTest(const char *cmd);

  // send - Send message, shortcut of test send
  bool doSend(const char *cmd);

  // set - Config command
  bool doSet(const char *cmd);

  // sys - System control
  bool doSys(const char *cmd);
  bool doSysSub(const char *cmd);

  bool SetupWiFi(const char *cmd);
  bool SetWiFiCredential(const char *cmd);
  bool PingAddress(const char *sAddress);
  bool String2IP(const char *sAddress, IPAddress &ipAddr);
};

//------------------------------------------------------------------
// Function & Class Helper
//------------------------------------------------------------------
extern SerialConsoleClass theConsole;

#endif /* xlxSerialConsole_h */
