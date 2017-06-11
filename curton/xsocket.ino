#include "application.h"
#include "MICSensor.h"

// Uncomment this line if key works on high output
//#define KEY_HIGH_WORK

#define PIN_KEY_S1         D4
#define PIN_KEY_S2         D5
#define PIN_KEY_S3         D6
#define PIN_SEN_MIC        A2

#define SEN_MIC_MIN               0		       // 0 volt
#define SEN_MIC_MAX               3720       // 3 volt -> (3 / 3.3 * 4096)

#define CLT_NAME_SensorData     "xlc-data-sensor"
#define CLT_TTL_MotionData      5

String mLastMessage;

MicSensor senMIC(PIN_SEN_MIC);

void ReadSensors();

void setup()
{
    Serial.begin(115200);

    Particle.variable("LastMessage", &mLastMessage, STRING);

    Particle.function("PressKey", PressKey);

    pinMode(PIN_KEY_S1, OUTPUT);
    pinMode(PIN_KEY_S2, OUTPUT);
    pinMode(PIN_KEY_S3, OUTPUT);

    delay(200);

    // Reset keys
#ifdef KEY_HIGH_WORK
    digitalWrite(PIN_KEY_S1, LOW);
    digitalWrite(PIN_KEY_S2, LOW);
    digitalWrite(PIN_KEY_S3, LOW);
#else
    digitalWrite(PIN_KEY_S1, HIGH);
    digitalWrite(PIN_KEY_S2, HIGH);
    digitalWrite(PIN_KEY_S3, HIGH);
#endif

    // Init MIC
    senMIC.begin(SEN_MIC_MIN, SEN_MIC_MAX);

    debug("Smart Socket started.");
}

void loop()
{
  // ToDo: status synchronize

  // ToDo: check timers

  // Read sensors
  ReadSensors();

  delay(10);
}

void PinUpDown(uint8_t _pin)
{
#ifdef KEY_HIGH_WORK
  digitalWrite(_pin, HIGH);
#else
  digitalWrite(_pin, LOW);
#endif
  delay(200);
#ifdef KEY_HIGH_WORK
  digitalWrite(_pin, LOW);
#else
  digitalWrite(_pin, HIGH);
#endif
}

// Cloud function
// Simulate keypress
// strKey: "up" ("s1"), "down" ("s3"), "stop" ("s2")
int PressKey(String strKey)
{
  uint8_t lv_pin;
  strKey.toLowerCase();
  if( strKey == "up" || strKey == "s1" ) {
    lv_pin = PIN_KEY_S1;
  } else if( strKey == "down" || strKey == "s3" ) {
    lv_pin = PIN_KEY_S3;
  } else if( strKey == "stop" || strKey == "s2" ) {
    lv_pin = PIN_KEY_S2;
  } else {
    lv_pin = 0;
  }

  if( lv_pin > 0 ) {
    PinUpDown(lv_pin);
    debug(String::format("Smart Key %s pressed.", strKey.c_str()));
  }

  return lv_pin;
}

void PublishMicValue(uint16_t value)
{
  if( Particle.connected() ) {
    String strTemp = String::format("{'nd':0,'NOISE':%d}", value);
    Particle.publish(CLT_NAME_SensorData, strTemp, CLT_TTL_MotionData, PRIVATE);
  }
}

void ReadSensors() {
  // MIC input (tone detection)
  uint16_t lv_micSample = 0;
  static uint16_t lv_preValue = 0;
  if( senMIC.getSample(&lv_micSample) ) {
    if( senMIC.isDataReady() ) {
        lv_micSample = senMIC.getValue();
        if( lv_preValue != lv_micSample ) {
          lv_preValue = lv_micSample;
          Serial.printlnf("MIC: %d", lv_micSample);
          PublishMicValue(lv_micSample);
        }
    }
  }
}

// Log message to cloud, message is a printf-formatted string
void debug(String message) {
  char msg [50];
  sprintf(msg, message.c_str());
  Particle.publish("DEBUG", msg);
  mLastMessage = msg;
  Serial.println(msg);
}
