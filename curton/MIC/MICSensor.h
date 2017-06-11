//  MICSensor_h.h - Xlight Microphone sensor lib

#ifndef MICSensor_h
#define MICSensor_h

#include "application.h"

#define SUM_SAMPLE_LENGTH       50

class MicSensor {
	private:
		uint8_t _pin;
    uint8_t _type;
    uint16_t _lowVal;
    uint16_t _upVal;
    uint32_t _sumVal;
    uint16_t _sampleData;
    uint8_t _dataLen;
    bool _dataReady;

	public:
    MicSensor(uint8_t pin, uint8_t type = 0);
		void begin(uint16_t lval=0, uint16_t uval=4095);
    bool getSample(uint16_t *sample);
    uint16_t getValue();
    bool isDataReady();
};

#endif /* MICSensor_h */
