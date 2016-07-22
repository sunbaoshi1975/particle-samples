//  xliConfig.h - Xlight SmartController configuration header

#ifndef xliConfig_h
#define xliConfig_h

#ifndef SERIAL
#define SERIAL        Serial.printf
#endif

#ifndef SERIAL_LN
#define SERIAL_LN     Serial.printlnf
#endif

/*** USER DEFINES:  ***/
#define FAILURE_HANDLING
//#define SERIAL_DEBUG

/**********************/

#define PRIPSTR "%s"
#define pgm_read_word(p) (*(p))
#ifdef SERIAL_DEBUG
  #define IF_SERIAL_DEBUG(x) ({x;})
#else
  #define IF_SERIAL_DEBUG(x)
#endif

#endif /* xliConfig_h */
