[![download](https://img.shields.io/badge/current-download-brightgreen.svg)](https://github.com/jbanaszczyk/pms5003/archive/master.zip)
[![Version](https://img.shields.io/badge/release-2.1-brightgreen.svg)](https://github.com/jbanaszczyk/pms5003/releases)
[![Platform](https://img.shields.io/badge/platform-arduino-blue.svg)](https://www.arduino.cc/)
[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-11-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-BSL-blue.svg)](https://www.boost.org/LICENSE_1_0.txt)

# pms5003

I'm proud to present my Arduino library supporting PMS5003 Air Quality Sensor.

## License

pms5003 library is distributed under [Boost Software License 1.0 (BSL-1.0)](https://www.boost.org/LICENSE_1_0.txt).

## Status

### Current revision: 2.1

[release: 2.1](https://github.com/jbanaszczyk/pms5003/releases/tag/2.1)

* There are some [TODOs](/../../issues)  for next revisions

### What is new?

Release 2.1 brings a lot of changes and improvements:

* API contract (class names and methods) is completely rewritten. It is not compatible with v1.0. Sorry :(
* Minor bugs are fixed (nothing important, release 1.0 should be assumed as stable).
* Added support for sensor hardware pins (pin 3 - SET, pin 6 - RESET).
* Added support for more serial port libraries (inversion of control).
* Added support for unit tests (soon).
* Added support for ISO 14644-1: Classification of air cleanliness.
* Added more diagnostic checks
* Added support for "views" (will be described later) - the most exciting new feature.

### Good, stable revision: 1.0

Previous [release: 1.0](https://github.com/jbanaszczyk/pms5003/releases/tag/1.0) is still available.

There is one interesting fork supporting ESP8266: https://github.com/riverscn/pmsx003

## Other sensors

My library supports PMS5003 (checked)

For most [Plantover](http://www.plantower.com) sensors probably it is an easy task to add support.

[list of compatible sensors](Compatibility.md) is available as a separate document.

## Features

* Library supports all Plantover PMS5003 features (sleep & wake up, passive / active modes, hardware pins),
* Highly customizable:
  * It uses almost any serial communication library,
  * You have a choice to use or not to use: (C style) global variables or (C++ style) class instances.
* Written from scratch,
* Written in modern C++11 (please do not afraid - works fine with Arduino IDE).
* It is "headers only" library.
* __The main goal__: Reading data from the sensor does not block. Your process receives the status `OK` or `NO_DATA` or some kinds of errors but your process never waits for the data.
* Serial port is not managed by the library, it is possible to shut serial driver down, enter sleep mode and so on independently on the PMS5003 sensor.
* Provides support for ISO 14644-1 classification of air cleanliness levels.

## API

[pms5003 API description](API.md) is available as a separate document.

## TODO

* New methods: some more checks
  * [#14](/../../issues/14) `checkResetPin()` - check if declared reset pin works fine (check if it resets the sensor)
  * [#15](/../../issues/15) `checkSleepPin()` - check if declared sleep/wake up pin is properly connected
* [#16](/../../issues/16) `write()` will return `PmsStatus` instead of `bool`
* [#17](/../../issues/17) add iterators `begin()` and `end()` for views
* [#18](/../../issues/18) `isWorking()` should return `tribool`
* [#19](/../../issues/19) `write(CMD_WAKEUP)` should not delay if already awoken
* [#20](/../../issues/20) `write()` multiple commands sequentially
* Support for platforms
  * [#22](/../../issues/22) PlatformIO
  * [#23](/../../issues/23) CLion
* Support for boards:
  * [#24](/../../issues/24) ESP8266
* [#25](/../../issues/25) Add unit tests

## Preparation

### IDE & OS

pms5003 library is developed using:
* Visual Studio Community 2017 (Windows)
* [Arduino IDE for Visual Studio](http://www.visualmicro.com)

pms5003 library was successfully checked using:
* Arduino 1.8.5 (Windows)

### Dependencies

* Current version uses [DrDiettrich' fork of AltSoftSerial Library](https://github.com/DrDiettrich/AltSoftSerial.git). Install it.
  * pms5003 will not compile using original AltSoftSerial lib.
* pms5003 will not compile using __very old__ Arduino IDE. Please upgrade.

### Library

Install pms5003 library.

### Connections

* PMS5003 Pin 1 (violet): VCC +5V
* PMS5003 Pin 2 (brown): GND

**Important**: PMS5003 sensor uses 3.3V logic. Use converters if required or make sure your Arduino board uses 3.3V logic too.
* PMS5003 Pin 4 (blue): Arduino pin 9 (there is no choice, it is forced by AltSerial)
* PMS5003 Pin 5 (green): Arduino pin 8 (there is no choice, it is forced by AltSerial)
* Optional
  * PMS5003 Pin 3 (white): Arduino pin 7 (can be changed or not connected at all)
  * PMS5003 Pin 6 (yellow): Arduino pin 6 (can be changed or not connected at all)
* PMS5003 pin 7 (black) and pin 7 (red) leave not connected

# Applications

## Hello. The Basic scenario.<a name="Hello"></a>

Use the code:  [examples\p01basic\p01basic.ino](https://github.com/jbanaszczyk/pms5003/blob/master/examples/p01basic/p01basic.ino)

```C++
#include <pms.h>

PmsAltSerial pmsSerial;
pmsx::Pms pms(&pmsSerial);

////////////////////////////////////////

void setup(void) {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println(pmsx::pmsxApiVersion);

    if (!pms.begin()) {
        Serial.println("PMS sensor: communication failed");
        return;
    }

    pms.setPinReset(6);
    pms.setPinSleepMode(7);

    if (!pms.write(pmsx::PmsCmd::CMD_RESET)) {
        pms.write(pmsx::PmsCmd::CMD_SLEEP);
        pms.write(pmsx::PmsCmd::CMD_WAKEUP);
    }
    pms.write(pmsx::PmsCmd::CMD_MODE_PASSIVE);
    pms.write(pmsx::PmsCmd::CMD_READ_DATA);
    pms.waitForData(pmsx::Pms::TIMEOUT_PASSIVE, pmsx::PmsData::FRAME_SIZE);
    pmsx::PmsData data;
    auto status = pms.read(data);
    if (status != pmsx::PmsStatus::OK) {
        Serial.print("PMS sensor: ");
        Serial.println(status.getErrorMsg());
    }
    pms.write(pmsx::PmsCmd::CMD_MODE_ACTIVE);
    if (!pms.isWorking()) {
        Serial.println("PMS sensor failed");
    }

    Serial.print("Time of setup(): ");
    Serial.println(millis());
}

////////////////////////////////////////

void loop(void) {

    static auto lastRead = millis();

    pmsx::PmsData data;
    auto status = pms.read(data);

    switch (status) {
    case pmsx::PmsStatus::OK: {
        Serial.println("_________________");
        const auto newRead = millis();
        Serial.print("Wait time ");
        Serial.println(newRead - lastRead);
        lastRead = newRead;

        auto view = data.particles;
        for (auto i = 0; i < view.getSize(); ++i) {
            Serial.print(view.getValue(i));
            Serial.print("\t");
            Serial.print(view.getName(i));
            Serial.print(" [");
            Serial.print(view.getMetric(i));
            Serial.print("] ");
//            Serial.print(" Level: ");
//            Serial.print(view.getLevel(i));
            Serial.print(" | diameter: ");
            Serial.print(view.getDiameter(i));
            Serial.println();
        }
        break;
    }
    case pmsx::PmsStatus::NO_DATA:
        break;
    default:
        Serial.print("!!! Pms error: ");
        Serial.println(status.getErrorMsg());
    }
}
```

And the result is (something like this):
```
Port open
pms5003 2.1
Time of setup(): 2589
_________________
Wait time 906
27	PM1.0, CF=1 [micro g/m3]  | diameter: 1.00
34	PM2.5, CF=1 [micro g/m3]  | diameter: 2.50
35	PM10.  CF=1 [micro g/m3]  | diameter: 10.00
23	PM1.0 [micro g/m3]  | diameter: 1.00
31	PM2.5 [micro g/m3]  | diameter: 2.50
35	PM10. [micro g/m3]  | diameter: 10.00
8760	Particles > 0.3 micron [/0.1L]  | diameter: 0.30
1780	Particles > 0.5 micron [/0.1L]  | diameter: 0.50
70	Particles > 1.0 micron [/0.1L]  | diameter: 1.00
18	Particles > 2.5 micron [/0.1L]  | diameter: 2.50
1	Particles > 5.0 micron [/0.1L]  | diameter: 5.00
1	Particles > 10. micron [/0.1L]  | diameter: 10.00
37120	Reserved_0 [???]  | diameter: 0.00
```

## More about the example

### Before

To use the library:
* install (download and "Add .ZIP library") [DrDiettrich' fork of AltSoftSerial Library](https://github.com/DrDiettrich/AltSoftSerial/archive/master.zip)
* install (download and "Add .ZIP library") [pms5003 library](https://github.com/jbanaszczyk/pms5003/archive/master.zip)
* include `pms.h` in your code:

```C++
#include <pms.h>
```

Create instance of serial driver

```C++
PmsAltSerial pmsSerial;
```

Create instance of pmsx::Pms object:
* library namespace name is `pmsx`
* class name of the object is `Pms`
* object is named `pms`
* object uses previously created driver `pmsSerial`

```C++
pmsx::Pms pms(&pmsSerial);
```

### setup()

Initialize serial library. If Arduino can't communicate with PMS5003 - there is no sense to perform the next steps.

pms5003 takes care on protocol details (speed, data length, parity and so on).

```C++
    if (!pms.begin()) {
        Serial.println("PMS sensor: communication failed");
        return;
    }
```

The next step is to define Arduino pins connected to pms5003:
* SET (pms5003 pin 3, white) (sleep/wakeup)
* RESET (pms5003 pin 6, violet) (sensor reset)

This step is optional.
* If SET pin is not connected - sleep/wakeup commands are executed using serial connection
* If RESET pin is not connected - sleep and then wakeup works like reset

If pins are not connected - just remove appropriate `setPinReset`/`setPinSleepMode` lines.

```C++
    pms.setPinReset(6);
    pms.setPinSleepMode(7);
```

The next task is to put sensor in a well known state.
There are two aspects of PMS5003 state:
* sleeping/awoken
* passive/active

Both can be examined using `isModeActive()`/`isModeSleep()`. Please note, that result value is a tristate logic `tribool`: Yes / No / I don't know.  
Please refer to [myArduinoLibrary](https://github.com/jbanaszczyk/ArduinoLibraries). It is Arduino port of [boost.tribool library description](https://www.boost.org/doc/libs/1_67_0/doc/html/tribool.html)

Please note, that it is possible, that Arduino was restarted for any reason, but PMS5003 was set in a strange state and it was not restarted. It is the reason, that initial states of PMS5003 is "I don't know"

Well known state (awoken and active) can be achieved after sensor hardware reset or sleep+wakeup sequence

```C++
    if (!pms.write(pmsx::PmsCmd::CMD_RESET)) {
        pms.write(pmsx::PmsCmd::CMD_SLEEP);
        pms.write(pmsx::PmsCmd::CMD_WAKEUP);
    }
```

The next task is to make sure, that Arduino can communicate with PMS5003. To accomplish the task we are:
* forcing passive mode (PMS5003 sends data only if asked),
* ask for data,
* wait for the response
* and check the response

```C++
    pms.write(pmsx::PmsCmd::CMD_MODE_PASSIVE);
    pms.write(pmsx::PmsCmd::CMD_READ_DATA);
    pms.waitForData(pmsx::Pms::TIMEOUT_PASSIVE, pmsx::PmsData::FRAME_SIZE);
    pmsx::PmsData data;
    auto status = pms.read(data);
    if (status != pmsx::PmsStatus::OK) {
        Serial.print("PMS sensor: ");
        Serial.println(status.getErrorMsg());
    }
    if (!pms.isWorking()) {
        Serial.println("PMS sensor failed");
    }
```

Finally we put back PMS5003 in active mode - it sends data periodically and automatically.

```C++
    pms.write(pmsx::PmsCmd::CMD_MODE_ACTIVE);
```

### loop()

First of all: __pms5003 does not block on data read__

Try to read the data :

```C++
    pmsx::PmsData data;
    auto status = pms.read(data);

    switch (status) {
```

If there is something interesting: display it:

```C++
    case pmsx::PmsStatus::OK: {
        ....
```

If there are no data: do something else:

```C++
    case pmsx::PmsStatus::NO_DATA:
        break;
```

In case of error: show the error message:

```C++
    default:
        Serial.print("!!! Pms error: ");
        Serial.println(status.getErrorMsg());
    }
```

Lets go back to the situation where there is something interesting:

```C++
    case pmsx::PmsStatus::OK: {
```

#### views

Data received from PMS5003 (see [Appendix I](https://github.com/jbanaszczyk/pms5003/blob/master/doc/pms5003-manual_v2-3.pdf)) may be worth attention:
* as a whole (13 `pmsx::pmsData_t` numbers, that is 13 `unsigned int` numbers)
* in groups:
  * (3 numbers) PM 1.0/2.5/10.0 concentration unit µ g/m3,  standard particle, (Compensation Factory) CF=1, (TSI: Technical Specifications for Interoperability))
  * (3 numbers) PM 1.0/2.5/10.0 concentration unit µ g/m3 (under atmospheric environment) (_looks good for everyday use_)
  * (6 numbers) the number of particles with diameter beyond 0.3/0.5/1.0/2.5/5.0/10.0 um in 0.1 L of air (_very tasty data, it fits into ISO 14644-1 classification of air cleanliness levels_)
  * (1 number) reserved data, without any real meaning

To get access to them:

```C++
        auto view = data.raw;
```

or

```C++
        auto view = data.concentrationCf;
```

or

```C++
        auto view = data.concentration;
```

or

```C++
        auto view = data.particles;
```

Each "view" provides similar interface:
* use `getSize()` to get counter of data in a view:
  * `for (auto i = 0; i < view.getSize(); ++i)`
* use `.getValue()` to get particular data from index
  * `Serial.print(view.getValue(i));`
* use `.getName()` to get description of particular data; for example "Particles > 1.0 micron"
  * `Serial.print(view.getName(i));`
* use `.getMetric()` to get unit of measure for particular data; for example "/0.1L"
  * `Serial.print(view.getMetric(i));`
* use `.getDiameter()` to get particle diameter corresponding to particular data; for example 1.0F
  * `Serial.print(view.getDiameter(i));`
* additionally: `particles` "view" provides `.getLevel()` - ISO classification of air cleanliness
  * `Serial.print(view.getLevel(i));`

Such a "views" (data partitions) are implemented with no execution time nor memory overhead.

```C++
        auto view = data.particles;
        for (auto i = 0; i < view.getSize(); ++i) {
            Serial.print(view.getValue(i));
            Serial.print("\t");
            Serial.print(view.getName(i));
            Serial.print(" [");
            Serial.print(view.getMetric(i));
            Serial.print("] ");
//            Serial.print(" Level: ");
//            Serial.print(view.getLevel(i));
            Serial.print(" | diameter: ");
            Serial.print(view.getDiameter(i));
            Serial.println();
        }
        break;
    }
}
```

#### views: C style

If you prefer C style: constants and arrays instead of method calls - please note [examples\p02cStyle\p02cStyle.ino](https://github.com/jbanaszczyk/pms5003/blob/master/examples/p02cStyle/p02cStyle.ino)

```C++
        auto view = data.particles;
        for (auto i = 0; i < view.SIZE; ++i) {
            Serial.print(view[i]);
            Serial.print("\t");
            Serial.print(view.names[i]);
            Serial.print(" [");
            Serial.print(view.metrics[i]);
            Serial.print("] ");
//            Serial.print(" Level: ");
//            Serial.print(view.getLevel(i));
            Serial.print(" | diameter: ");
            Serial.print(view.diameters[i]);
            Serial.println();
        }
```

* use `SIZE` to get counter of data in a view:
  * `for (auto i = 0; i < view.SIZE; ++i)`
* use array index `[]` to get particular data
  * `Serial.print(view[i]);`
* use `.names[]` array to get description of particular data; for example "Particles > 1.0 micron"
  * `Serial.print(view.names[i]);`
* use `.metrics[]` array to get unit of measure for particular data; for example "/0.1L"
  * `Serial.print(view.metrics[i]);`
* use `.diameters[]` to get particle diameter corresponding to particular data; for example 1.0F
  * `Serial.print(view.diameters[i]);`
* additionally: `particles` "view" provides `.getLevel()` - ISO classification of air cleanliness. It is not implemented as array - it is a function (a method)
  * `Serial.print(view.getLevel(i));`

**Which one is better?** It doesn't matter - code size, memory usage and resulting code is exactly the same using both approaches.

#### Initialization: C++ style

Common pattern in C++ is "initialization in constructor". Unfortunately Arduino breaks that rule.

There is a code from: hardware\arduino\avr\cores\arduino\main.cpp modified for simplicity

```C
// global variables constructors are executed before main()
int main(void) {
        init();
        initVariant();
        setup();         // our setup() procedure
        for (;;) {
                loop();  // our loop()  procedure
        }
        return 0;
}
```

Lets imagine:
1. If there would be is a global variable `pms` of type `Pms`.
2. It would be a good place to initialize serial communication `pms.begin()` within `pms` constructor
3. Global variables constructors are executed before main()
4. After that Arduino initializes all the hardware
5. And than our `setup()` is executed.

Our serial connection started in step 3) is destroyed during Arduino initialization in step 4.

There are at least two possible solutions:

##### initialization: begin()

By the way: if you are not sure if everything was properly initialized - execute `begin()` manually and check the result.

##### C/Arduino way

* [examples\p01basic\p01basic.ino](https://github.com/jbanaszczyk/pms5003/blob/master/examples/p01basic/p01basic.ino)
* Create static variable of type `Pms`, do nothing in constructor
* Initialize it during `setup()`: call `pms.begin()`
* Use it: call `pms.` methods

```C++
PmsAltSerial pmsSerial;
pmsx::Pms pms(&pmsSerial);

void setup(void) {
    if (!pms.begin()) {
```

##### C++ way

* Edit pmsConfig.h file, uncomment line `#define PMS_DYNAMIC`
* [examples\p03cppStyle\p03cppStyle.ino](https://github.com/jbanaszczyk/pms5003/blob/master/examples/p03cppStyle/p03cppStyle.ino)
* Create static variable of type `*Pms` (reference to `Pms`)
* Do nothing prior to main()
* During `setup()` create new object of type `Pms`, assign created object to the reference from previous step
  * `Pms()` constructor is executed automatically
  * `Pms()` constructor executes `begin()`
  * It executes `begin()` of the serial port driver
* Use it: call `pms->` methods

This approach adds some code size - compiler adds dynamic memory management.

```C++
PmsAltSerial pmsSerial;
pmsx::Pms* pms = nullptr;

void setup(void) {
    pms = new pmsx::Pms(&pmsSerial);
    if (!pms->initialized()) {
```

#####  **Which one is better?** 

C/Arduino way using `begin()` is closer to Arduino programming style.  
In my opinion: C++ way is closer to modern programming style.

#### ISO cleanliness levels

`particles` view provides support for ISO 14644-1 classification of air cleanliness levels.

Please refer to `p03cppStyle.ino`

The code (`loop()` function only):
```C++
void loop(void) {

    static auto lastRead = millis();
    pmsx::PmsData data;
    auto status = pms->read(data);

    switch (status) {
    case pmsx::PmsStatus::OK: {
        Serial.println("_________________");
        const auto newRead = millis();
        Serial.print("Wait time ");
        Serial.println(newRead - lastRead);
        lastRead = newRead;

        auto view = data.particles;     
        for (auto i = decltype(view.SIZE){0}; i < view.getSize(); ++i) {
            Serial.print(view.getValue(i));
            Serial.print("\t");
            Serial.print(view.getName(i));
            Serial.print(" [");
            Serial.print(view.getMetric(i));
            Serial.print("] ");
            Serial.print(" Level: ");
            Serial.print(view.getLevel(i));
            Serial.print(" | diameter: ");
            Serial.print(view.getDiameter(i));
            Serial.println();
        }
        break;
    }
    case pmsx::PmsStatus::NO_DATA:
        break;
    default:
        Serial.print("!!! Pms error: ");
        Serial.println(status.getErrorMsg());
    }}
```

and the example of the result:

```
_________________
Wait time 906
1875	Particles > 0.3 micron [/0.1L]  Level: 8.27 | diameter: 0.30
505	Particles > 0.5 micron [/0.1L]  Level: 8.16 | diameter: 0.50
62	Particles > 1.0 micron [/0.1L]  Level: 7.87 | diameter: 1.00
7	Particles > 2.5 micron [/0.1L]  Level: 7.75 | diameter: 2.50
1	Particles > 5.0 micron [/0.1L]  Level: 7.53 | diameter: 5.00
0	Particles > 10. micron [/0.1L]  Level: 0.00 | diameter: 10.00
```

# Final notes

## API

[pms5003 API description](API.md) is available as a separate document.

## Operations on serial port

Serial interface **is not managed** by `Pms`. You can suspend data transfer, enter sleep mode, even replace serial port. Just remember to execute `pms.begin()` to reinitialize the connection.

## namespace pmsx{}

`pms5003` library is designed to avoid namespace pollution. All classes are located in the namespace `pmsx`.

Examples use the fully qualified names like `pmsx::Pms pms(&pmsSerial);`

To reduce typing it is OK to add `using namespace pmsx;` at the beginning and not to type _pmsx::_ anymore as in [examples\p04usingPmsx\p04usingPmsx.ino](https://github.com/jbanaszczyk/pms5003/blob/master/examples/p04usingPmsx/p04usingPmsx.ino)

It does not change resulting code size.
