#pragma once

////////////////////////////////////////////

// Use one of:
// it depends on Serial Library (and serial pin connection)

#define PMS_ALTSOFTSERIAL

#if defined PMS_ALTSOFTSERIAL
// Install https://github.com/DrDiettrich/AltSoftSerial.git)
#include <pmsSerialAltSoftSerial.h>
#else
#error "At least one of: [ PMS_ALTSOFTSERIAL ] have to be defined in pmsConfig.h"
#endif

////////////////////////////////////////////

// Use PMS_DYNAMIC to be C++ strict
// Without PMS_DYNAMIC:
//   con: PMS5003 related object should be defined as global variable (C style): Pms pms;
//   con: It can not be initialized inside constructor. It is too early, serial ports and other pins will be redefined by Arduino bootloader
//   con: It has to be initialized within setup() - see examples: uses of begin()
// With PMS_DYNAMIC:
//   pro: PMS5003 related object instance should be created (C++ style) using _pms = new Pms();
//   pro: It can be created within setup(), it is good time to initialize class instance in the constructor. Arduino board was initialized here.
//   con: If you are not using heap: it uses heap, it adds meaningful code overhead - malloc, new() and memory management should be included. ( 0.5kb of program memory, a few memory bytes)

// #define PMS_DYNAMIC

////////////////////////////////////////////
