/**
 * MICSensor.cpp - Xlight Microphone sensor lib
 *
 * Created by Baoshi Sun <bs.sun@datatellit.com>
 * Copyright (C) 2015-2017 DTIT
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
 * 1. support 3-Pin and 4-pin Microphone
 *
 * ToDo:
 * 1.
**/

#include "MICSensor.h"

MicSensor::MicSensor(uint8_t pin, uint8_t type)
{
	_pin = pin;
	_type = type;
  _sumVal = 0;
  _dataLen = 0;
  _sampleData = 0;
  _dataReady = false;
}

// lval & uval: map voltages [0, 3.3] into integer values [0, 4095].
/// Therefore, need to check the light sensor output voltage scale.
void MicSensor::begin(uint16_t lval, uint16_t uval)
{
	pinMode(_pin, INPUT);
  _lowVal = lval;
  _upVal = uval;
  _sumVal = 0;
  _dataLen = 0;
  _sampleData = 0;
  _dataReady = false;
}

bool MicSensor::isDataReady()
{
  return _dataReady;
}

uint16_t MicSensor::getValue()
{
  return _sampleData;
}

bool MicSensor::getSample(uint16_t *sample)
{
  uint16_t _lastValue = analogRead(_pin);
  if( _lastValue < _lowVal || _upVal > _upVal ) {
    // Discard extreme value
    return false;
  }
  *sample = _lastValue;
  _sumVal += _lastValue;
  if( ++_dataLen >= SUM_SAMPLE_LENGTH ) {
    _sampleData = _sumVal / _dataLen;
    _dataLen = 0;
    _sumVal = 0;
    _dataReady = true;
  } else {
    _dataReady = false;
  }
  return true;
}
