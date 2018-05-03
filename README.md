# pms5003

I'm proud to present my Arduino library supporting PMS5003 Air Quality Sensor.

## Status

### Current revision: 2.00 RC

It is not published as a release

Code in repository (branch: master) is 2.00 RC
* It looks stable - development is finished
* README have to be updated
* There are some TODOs for next revisions

### What is new?

Release 2.0 brings a lot of changes and improvements:

* API contract (class names and methods) is completely rewritten. It is not compatible with v1.0. Sorry :(
* Minor bugs are fixed (nothing important, release 1.00 should be assumed as stable).
* Added support for sensor hardware pins (pin 3 - SET, pin 6 - RESET).
* Added support for more serial port libraries (inversion of control).
* Added support for unit tests (soon).
* Added support for ISO 14644-1: Classification of air cleanliness.
* Added more diagnostic checks
* Added support for "views" (will be described later) - the most exciting new feature.

### Good, stable revision: 1.00

Previous releases are available: https://github.com/jbanaszczyk/pms5003/releases

There is one interesting fork supporting ESP8266: https://github.com/riverscn/pmsx003

## Other boards

Probably my library supports Plantover PMS700x, PMS300x without any problems.

If you are interested in support of your sensor: feel free to ask.

## Features

* Supports all Plantover PMS5003 features (sleep/wake up, passive/active modes, hardware pins),
* Highly customizable:
  * Uses almost any serial communication library,
  * You have a choice to use or not to use: (C style) global variables or (C++ style) class instances.
* Written from scratch,
* Written in modern C++11.
* It is "headers only" library.
* __The main goal__: Reading data from the sensor does not block. Your process receives the status `OK` or `NO_DATA` or some kinds of errors but your process never waits for the data.
* Provides support for ISO 14644-1 classification of air cleanliness levels.

## TODO

* New methods: some more checks
  * checkResetPin - check if declared reset pin works fine (check if it resets the sensor)
  * checkSleepPin - check if declared sleep/wake up pin is properly connected
* Use PROGMEM to store some static data (mostly strings)
* Support for platforms
  * More platforms:
    * PlatformIO
    * CLion
    * ...
* Support for boards:
  * ESP8266 support
* Add unit tests
  * There are no unit tests yet :(

## Preparation

### IDE & OS

pms5003 library is developed using:
* Visual Studio Community 2017 (Windows)
* Arduino IDE for Visual Studio: http://www.visualmicro.com/

pms5003 library was successfully checked using:
* Arduino 1.8.5 (Windows)

### Dependencies

* Current version uses [DrDiettrich' fork of AltSoftSerial Library](https://github.com/DrDiettrich/AltSoftSerial.git). Install it.
  * pms5003 will not compile using original AltSoftSerial lib.
* pms5003 will not compile using __old__ Arduino IDE. Please upgrade.

### Library

Install pms5003 library.

### Connections

* PMS5003 Pin 1 (black): VCC +5V
* PMS5003 Pin 2 (brown): GND

**Important**: pms5003 uses 3.3V logic. Use converters if required or make sure your Arduino board uses 3.3V logic too.
* PMS5003 Pin 4 (blue): Arduino pin 9 (there is no choice, it is forced by AltSerial)
* PMS5003 Pin 5 (green): Arduino pin 8 (there is no choice, it is forced by AltSerial)
* Optional
  * PMS5003 Pin 3 (white): Arduino pin 7 (can be changed or not connected at all)
  * PMS5003 Pin 6 (violet): Arduino pin 6 (can be changed or not connected at all)

# Applications

## Hello. The Basic scenario.<a name="Hello"></a>

Use the code: https://github.com/jbanaszczyk/pms5003/blob/master/Examples/p01basic/p01basic.ino

```C++
#include <pms.h>

PmsAltSerial pmsSerial;
pmsx::Pms pms(&pmsSerial);

////////////////////////////////////////

void setup(void) {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("PMS5003");

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
PMS5003
Time of setup(): 2589
Wait time 910
0	Particles > 0.3 micron [/0.1L]  Level: 0.00 | diameter: 0.30
0	Particles > 0.5 micron [/0.1L]  Level: 0.00 | diameter: 0.50
0	Particles > 1.0 micron [/0.1L]  Level: 0.00 | diameter: 1.00
0	Particles > 2.5 micron [/0.1L]  Level: 0.00 | diameter: 2.50
0	Particles > 5.0 micron [/0.1L]  Level: 0.00 | diameter: 5.00
0	Particles > 10. micron [/0.1L]  Level: 0.00 | diameter: 10.00
_________________
Wait time 910
0	Particles > 0.3 micron [/0.1L]  Level: 0.00 | diameter: 0.30
0	Particles > 0.5 micron [/0.1L]  Level: 0.00 | diameter: 0.50
0	Particles > 1.0 micron [/0.1L]  Level: 0.00 | diameter: 1.00
0	Particles > 2.5 micron [/0.1L]  Level: 0.00 | diameter: 2.50
0	Particles > 5.0 micron [/0.1L]  Level: 0.00 | diameter: 5.00
0	Particles > 10. micron [/0.1L]  Level: 0.00 | diameter: 10.00
```

## More about the example

### Before

To use the library:
* install the [pms5003 library](https://github.com/jbanaszczyk/pms5003/archive/master.zip)
* install [DrDiettrich' fork of AltSoftSerial Library](https://github.com/DrDiettrich/AltSoftSerial/archive/master.zip)
* include the header `#include <pms.h>`

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
* sleep/waken up
* passive/active

Both can be examined using `isModeActive()`/`isModeSleep()`. Please note, that result value is a tristate logic `tribool`: Yes / No / I don't know. Please refer to [boost `tribool` library description](https://www.boost.org/doc/libs/1_67_0/doc/html/tribool.html)

Please note, that it is possible, that Arduino was restarted, but PMS5003 was set in a strange state and it was not restarted.

Well known state (woken up and active) can be achieved after sensor reset or sleep+wakeup sequence

```C++
    if (!pms.write(pmsx::PmsCmd::CMD_RESET)) {
        pms.write(pmsx::PmsCmd::CMD_SLEEP);
        pms.write(pmsx::PmsCmd::CMD_WAKEUP);
    }
```

The next task is to make sure, that Arduino can communicate with PMS5003. To accomplish the task we are:
* forcing passive mode (PMS5003 sends data if asked),
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

Finally we put PMS5003 in active mode - it sends data periodically and automatically.

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

If there is something interesting - display it:

```C++
    case pmsx::PmsStatus::OK: {
        ....
```

If there is are no data: do something else:

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
* as a whole (12 numbers)
* in groups:
  * (3 numbers) PM 1.0/2.5/10.0 concentration unit µ g/m3 (CF=1,standard particle) (_really? I have no idea what does it mean_)
  * (3 numbers) PM 1.0/2.5/10.0 concentration unit µ g/m3 (under atmospheric environment) (_looks good_)
  * (6 numbers) the number of particles with diameter beyond 0.3/0.5/1.0/2.5/5.0/10.0 um in 0.1 L of air (_very tasty data, it fits into ISO 14644-1 classification of air cleanliness levels_)
  * (1 number) reserved data, without any real meaning

To get access to them use:

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
* use `.getDiameter()` to get particle diameter corresponding to particular data
  * `Serial.print(view.getDiameter(i));`
* additionally: `particles` "view" provides `.getLevel()` - ISO classification of air cleanliness
  * Serial.print(view.getLevel(i));

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

#### views - C style

If you prefer C style: constants and arrays instead of method calls - please note https://github.com/jbanaszczyk/pms5003/blob/master/Examples/p02cStyle/p02cStyle.ino

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
* use `.diameters[]` to get particle diameter corresponding to particular data
  * `Serial.print(view.diameters[i]);`
* additionally: `particles` "view" provides `.getLevel()` - ISO classification of air cleanliness. It is not implemented as array - it is a function (a method)
  * Serial.print(view.getLevel(i));

**Which one is better?** It doesn't matter - code size, memory usage and resulting code is exactly the same using both approaches.










---
---
# Description below is not updated yet
---
---

# API<a name="API"></a>

## Classes

### Pms<a name="API_Pms5003"></a>
```C++
class Pms {...}
```
Pms provides all methods, data type, enums to provide support for PMS5003 sensor. In most cases there will be used single object of that class.

_Shown in_: [Basic scenario](#Hello)

#### ctor/dtor: Pms(), ~Pms()

See: [Config: PMS_DYNAMIC](#Cfg_PMS_DYNAMIC)

## Data types

### pmsData_t<a name="API_pmsData"></a>
```C++
typedef uint16_t pmsData_t;
```
Type of single data received from the sensor.

_Shown in_: [Basic scenario](#Hello)

### pmsIdx_t<a name="API_pmsIdx"></a>
```C++
typedef uint8_t pmsIdx_t;
```
Underlying type of [PmsDataNames](#API_PmsDataNames), suitable for declaring size of array receiving data from the sensor or to iterate over it.

_Shown in_: [Second example](https://github.com/jbanaszczyk/pms5003/blob/master/examples/Dynamic02/Dynamic02.ino)

You can use any unsigned int type instead. _As shown in_: [Basic scenario](#Hello)

## Enums

### PmsStatus<a name="API_PmsStatus"></a>
```
enum PmsStatus : uint8_t {
	PmsStatus::OK = 0,
	PmsStatus::NO_DATA,
	PmsStatus::READ_ERROR,
	PmsStatus::FRAME_LENGTH_MISMATCH,
	PmsStatus::SUM_ERROR,
	...
};
```

status returned by ```read()``` function.
* _OK_: whole data frame was correctly received
* _noData_: there is not enough data to read, try again later
* _readError_: read error reported by serial port supporting library
* _frameLenMismatch_, _sumError_: mallformed data received.

_Shown in_: [Basic scenario](#Hello)

### PmsDataNames<a name="API_PmsDataNames"></a>
```C++
enum PmsDataNames : pmsIdx_t {
	PM_1_DOT_0_CF1 = 0,       //  0
	PM_2_DOT_5_CF1,           //  1
	PM_10_CF1,          //  2
	PM_1_DOT_0,              //  3
	PM_2_DOT_5,              //  4
	PM_10_DOT_0,             //  5
	PARTICLES_0_DOT_3,       //  6
	PARTICLES_0_DOT_5,       //  7
	PARTICLES_1_DOT_0,       //  8
	PARTICLES_2_DOT_5,       //  9
	PARTICLES_5_DOT_0,       // 10
	PARTICLES_10,          // 11
	RESERVED,             // 12
	N_VALUES_PMSDATANAMES  // 13
};
```

Names (indexes) of particular data received from the sensor.

Sensor transmits array of data (each of type [pmsData_t](#pmsData_t). If you are interested in particular data - use index. For example: ```data[PARTICLES_0_DOT_3]```.

_Shown in_: [Basic scenario](#Hello)

### PmsCmd<a name="API_PmsCmd"></a>
```C++
enum PmsCmd : __uint24 {
	CMD_READ_DATA    = ...
	CMD_MODE_PASSIVE = ...
	CMD_MODE_ACTIVE  = ...
	CMD_SLEEP       = ...
	CMD_WAKEUP      = ...
};
```

Commands that can be send to the sensor. Will be described [later](#Commands).

_Shown in_: [Basic scenario](#Hello)

## Static arrays of strings

### errorMsg<a name="API_errorMsg"></a>
```C++
static const char *errorMsg[];
```

Contains statuses returned by ```read()``` function in human readable form. Use returned value as an index: ```Serial.println(errorMsg[readStatus])```

_Shown in_: [Basic scenario](#Hello)

### dataNames<a name="API_dataNames"></a>
```C++
static const char *dataNames[];
```

Human readable names of [PmsDataNames](#API_PmsDataNames). Use PmsDataNames as an index.

_Shown in_: [Basic scenario](#Hello)

#### getDataNames<a name="API_getDataNames"></a>
```Cpp
const char *Pms::getDataNames(const uint8_t idx);
Serial.print(Pms::getDataNames(i)); // instead of Serial.print(Pms::dataNames[i]);
```

There is provided range-safe function to access dataNames values: getDataNames();

_Shown in_: [Second example](https://github.com/jbanaszczyk/pms5003/blob/master/examples/Dynamic02/Dynamic02.ino)

### metrics<a name="API_metrics"></a>
```C++
static const char *metrics[];
```

Metrics associated with PmsDataNames. Use PmsDataNames as an index.

_Shown in_: [Basic scenario](#Hello)

#### getMetrics<a name="API_getMetrics"></a>
```Cpp
const char *Pms::getMetrics(const uint8_t idx);
Serial.print(Pms::getMetrics(i)); // instead of Serial.print(Pms::metrics[i]);
```

There is provided range-safe function to access metrics values: getMetrics();

_Shown in_: [Second example](https://github.com/jbanaszczyk/pms5003/blob/master/examples/Dynamic02/Dynamic02.ino)

## Methods

### begin()<a name="API_begin"></a>

```C++
bool begin(void);
```

Initializes Pms object.

If defined ```PMS_DYNAMIC```: automatically executed by ctor: ```Pms()```

Should be executed by global ```setup()``` otherwise.

Initializes internal serial interface object, initializes *this fields.

_Shown in_: [Basic scenario](#Hello)

See: [Config: PMS_DYNAMIC](#Cfg_PMS_DYNAMIC)

### end()<a name="API_end"></a>

```C++
void end(void);
```

Destroys Pms object.

If defined ```PMS_DYNAMIC```: automatically executed by dtor: ```~Pms()```

Not needed otherwise.

Shuts down internal serial interface object (if possible).


See: [Config: PMS_DYNAMIC](#Cfg_PMS_DYNAMIC)

### setTimeout, getTimeout<a name="API_setTimeout"></a><a name="API_getTimeout"></a>

```C++
void setTimeout(const unsigned long timeout);
unsigned long getTimeout(void) const;
```

By default - the most important method: ```read()``` does not block (it does not wait for data, just returns ```Pms::PmsStatus::NO_DATA```).

```write()``` in case of data transfer errors may block.

```setTimeout()```, ```getTimeout()``` deals with serial port timeouts.

Default timeout set by [begin()](#API_begin) equals to ```private: TIMEOUT_PASSIVE```, currently: 68 (twice time required to transfer 1start + 32data + 1stop using 9600bps).

### flushInput<a name="API_flushInput"></a>

```C++
void flushInput(void);
```

Proxy to serial port` ```flushInput()``` (if supported).

Clears all data received, but not read yet.

### available<a name="API_available"></a>

```C++
size_t available(void);
```

Semi-proxy to serial port ```available()``` (if supported).

It assumes, that we are waiting for the whole frame. Data frame begins with byte 0x42 always. All received, not read yet data, which does not match to 0x42 are discarded.

available() returns: number of received, not read bytes, the first one is 0x42.

### waitForData<a name="API_waitForData"></a>

```C++
bool waitForData(const unsigned int maxTime, const size_t nData = 0);
```
**waitForData() may block.**

waitForData(maxTime) works like a delay(maxTime), but can be terminated by Pms sensor activity.

Arguments:
* maxTime: amount of time to wait,
* nData:
	*	nData == 0: break the delay by any serial port activity,
	* nData > 0: break the delay if there are nData bytes available to read, the first byte is 0x42 (see description of [available()](#API_available)

Returns:
* true: if delay was broken (some data can be read).

_Shown in_: [Basic scenario](#Hello)

### read<a name="API_read"></a>

```C++
PmsStatus read(pmsData_t *data, const size_t nData, const uint8_t dataSize = 13);
```

**read() should not block.**

The most important function of the library. It receives, transforms and verifies data provided by PMS5003 sensor.

Arguments:
* data: pointer to an array containing _nData_ elements of type [pmsData_t](#pmsData_t).
* nData: number of data, that can be received and stored.
* dataSize: In general: don't use.

Returns [Pms::PmsStatus](#API_PmsStatus):
* ```Pms::PmsStatus::OK```: whole, not malformed data frame was received from the sensor, up to nData elements of *data was filled according to received data.
* ```Pms::PmsStatus::NO_DATA```: There is not enough data to read.
* Otherwise: refer to [errorMsg](#API_errorMsg)

_Typical usage_: [Basic scenario](#Hello)

_nData_:
* It is safe to specify nData larger or smaller than number of data provided by the sensor.
* Values from [PmsDataNames](#API_PmsDataNames) can be helpful.

_dataSize_:
* It specifies expected size of data frame: dataFrameSize = ( dataSize + 3 ) * 2;
* If there is not enough data to complete the whole frame - read() returns ```Pms::PmsStatus::NO_DATA``` and does not block.
* Typical frame sent by the sensor contains 32 bytes. Appropriate dataSize value is 13 (the default).

### write<a name="API_write"></a>

```C++
bool write(const PmsCmd cmd);
```
**write() can block up to [TIMEOUT_ACK](#API_ackTimeout) (currently 30milliseconds), typically about 10milliseconds.**

It sends a command to PMS5003 sensor. Refer to [commands section](#Commands).

PMS5003 responds to some [commands](#commands). The response is gathered and verified be the write() internally. That is the reason, that write() can block.

Arguments:
* cmd: one of [PmsCmd](#API_PmsCmd)

Returns:
* true: if there was no error.

_Shown in_: [Basic scenario](#Hello)

## Consts

### TIMEOUT_ACK<a name="API_ackTimeout"></a>

```C++
private: static const auto TIMEOUT_ACK = 30U;
```

Used internally (inside write()). Defines timeout for response read after write command.

write() can block up to TIMEOUT_ACK.

### WAKEUP_TIME<a name="API_wakeupTime"></a>

```C++
static const auto WAKEUP_TIME = 2500U;
```

WAKEUP_TIME defines time after power on, reset or write(CMD_WAKEUP) when the sensor is blind for any commands.

_Shown in_: [Basic scenario](#Hello)

## Commands and states<a name="commands"></a>

PMS5003 accepts a few commands. They are not fully documented.

You can send commands to the PMS5003 sensor using [write()](#API_write).

|From State|input                |To State  |Output                                |
|----------|---------------------|----------|--------------------------------------|
|[Any]     |Power on             |Active    |Spontaneously sends data frames       |
|[Any]     |write(CMD_WAKEUP)     |Active    |Spontaneously sends data frames       |
|[Any]     |write(CMD_SLEEP)      |Sleep     |None (waits for CMD_WAKEUP)            |
|Active    |write(CMD_MODE_PASSIVE)|Passive   |None                                  |
|Passive   |write(CMD_MODE_ACTIVE) |Active    |Spontaneously sends data frames       |
|Passive   |write(CMD_READ_DATA)   |Passive   |Sends single data frame               |




# Configuration <a name="Cfg_PMS_DYNAMIC"></a>

