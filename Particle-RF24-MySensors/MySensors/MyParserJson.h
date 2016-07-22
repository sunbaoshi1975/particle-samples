/**
 * MyParserJson.h - Xlight extention for MySensors message lib
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#ifndef MyParserJson_h
#define MyParserJson_h

#include "MyParser.h"

class MyParserJson : public MyParser
{
public:
  MyParserJson();
	bool parse(MyMessage &message, char *inputString);
  char* getJsonString(MyMessage &message, char *buffer) const;
};

#endif
