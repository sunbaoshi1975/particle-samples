#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stm8s.h> //Required for the stdint typedefs
#include "stdio.h"
#include "string.h"
#include "stm8s_conf.h"

/* Exported types ------------------------------------------------------------*/
// Common Data Type
#define UC                        uint8_t
#define US                        uint16_t
#define UL                        uint32_t
#define SHORT                     int16_t
#define LONG                      int32_t

// Device (lamp) type
#define MAX_RING_NUM            3
typedef enum
{
  devtypUnknown = 0,
  devtypCRing3,     // Color ring - Rainbow
  devtypCRing2,
  devtypCRing1,
  devtypWRing3,     // White ring - Sunny
  devtypWRing2,
  devtypWRing1,
  devtypMRing3 = 8, // Color & Motion ring - Mirage
  devtypMRing2,
  devtypMRing1,
  devtypDummy = 255
} devicetype_t;

typedef struct
{
  UC State                    :1;           // Component state
  UC BR                       :7;           // Brightness of white [0..100]
  US CCT                      :16;          // CCT (warm or cold) [2700..6500]
  UC R                        :8;           // Brightness of red
  UC G                        :8;           // Brightness of green
  UC B                        :8;           // Brightness of blue
  UC W                        :8;           // Brightness of white
  UC L1                       :8;           // Length of thread 1
  UC L2                       :8;           // Length of thread 2
  UC L3                       :8;           // Length of thread 3
} Hue_t;

typedef struct
{
  UC version                  :8;           // Data version, other than 0xFF
  UC nodeID;                                // Node ID for this device
  UC NetworkID[6];
  UC present                  :1;           // 0 - not present; 1 - present
  UC reserved                 :3;
  UC filter                   :4;
  UC type;                                  // Type of lamp
  US token;
  Hue_t ring[MAX_RING_NUM];
  char Organization[24];                    // Organization name
  char ProductName[24];                     // Product name
  UC rfPowerLevel             :2;           // RF Power Level 0..3
  UC hasSiblingMCU            :1;           // Whether sibling MCU presents
  UC swTimes                  :3;           // On/Off times
  UC Reserved1                :2;           // Reserved bits
  US senMap                   :16;          // Sensor Map
  US funcMap                  :16;          // Function Map
  UC alsLevel[2];
  UC pirLevel[2];
} Config_t;

extern Config_t gConfig;
extern bool gIsChanged;

#endif /* __GLOBAL_H */