# pms5003

I'm proud to present my Arduino library supporting PMS5003 Air Quality Sensor.

## Features

* Supports all Plantover PMS5003 features (sleep/wake up, passive/active modes),
* Probably works fine with PMS7003 and PMS3003,
* Highly customizable:
  * Uses any serial communication library,
  * You have a choice to use or not to use: global variables or class instances.
* Written from scratch,
* Written in modern C++.

## Preparation

Install pms5003 library.

Let's use [DrDiettrich' fork of AltSoftSerial Library](https://github.com/DrDiettrich/AltSoftSerial.git). Install it.
* pms5003 will not compile using original AltSoftSerial lib.

Make some connections:
* **Important**: pms5003 uses 3.3V logic. Make sure your Arduino board uses 3.3V logic too, use converters if required.
* PMS5003 Pin 1: Vcc
* PMS5003 Pin 2: GND
* PMS5003 Pin 4: Digital pin 9 (there is no choice, forced by AltSerial)
* PMS5003 Pin 5: Digital pin 8 (there is no choice)

## Applications

### Hello. The Basic scenario.<a name="Hello"></a>

Use the code: https://github.com/jbanaszczyk/pms5003/tree/master/examples/01_Simple

```C++
#include <Arduino.h>
#include <pms.h>

Pms5003 pms;

////////////////////////////////////////

void setup(void) {
	Serial.begin(115200);
	while (!Serial) {};
	Serial.println("PMS5003");

	pms.begin();
	pms.waitForData(Pms5003::wakeupTime);
	pms.write(Pms5003::cmdModeActive);
}

////////////////////////////////////////

auto lastRead = millis();

void loop(void) {

	const auto n = Pms5003::Reserved;
	Pms5003::pmsData data[n];

	Pms5003::PmsStatus status = pms.read(data, n);

	switch (status) {
		case Pms5003::OK:
		{
			Serial.println("_________________");
			auto newRead = millis();
			Serial.print("Wait time ");
			Serial.println(newRead - lastRead);
			lastRead = newRead;

			// For loop starts from 3
			// Skip the first three data (PM1dot0CF1, PM2dot5CF1, PM10CF1)
			for (size_t i = Pms5003::PM1dot0; i < n; ++i) {
				Serial.print(data[i]);
				Serial.print("\t");
				Serial.print(Pms5003::dataNames[i]);
				Serial.print(" [");
				Serial.print(Pms5003::metrics[i]);
				Serial.print("]");
				Serial.println();
			}
			break;
		}
		case Pms5003::noData:
			break;
		default:
			Serial.println("_________________");
			Serial.println(Pms5003::errorMsg[status]);
	};
}

```

And the result is (something like this):

```
_________________
Wait time 836
7	PM1.0 [mcg/m3]
8	PM2.5 [mcg/m3]
8	PM10. [mcg/m3]
1368	Particles < 0.3 micron [/0.1L]
361	Particles > 0.5 micron [/0.1L]
43	Particles > 1.0 micron [/0.1L]
1	Particles > 2.5 micron [/0.1L]
0	Particles > 5.0 micron [/0.1L]
0	Particles > 10. micron [/0.1L]
```

# API<a name="API"></a>

## Classes

### Pms5003<a name="API_Pms5003"></a>
```C++
class Pms5003 {...}
```
Pms5003 provides all methods, data type, enums to provide support for PMS5003 sensor. In most cases there will be used single object of that class.

_Shown in_: [Basic scenario](#Hello)

#### ctor/dtor: Pms5003(), ~Pms5003()

See: [Config: PMS_DYNAMIC](#Cfg_PMS_DYNAMIC)

## Data types

### pmsData<a name="API_pmsData"></a>
```C++
typedef uint16_t pmsData;
```
Type of single data received from the sensor.

_Shown in_: [Basic scenario](#Hello)

### pmsIdx<a name="API_pmsIdx"></a>
```C++
typedef uint8_t pmsIdx;
```
Underlying type of [PmsDataNames](#API_PmsDataNames), suitable for declaring size of array receiving data from the sensor or to iterate over it.

_Shown in_: [Second example](https://github.com/jbanaszczyk/pms5003/blob/master/examples/Dynamic02/Dynamic02.ino)

You can use any unsigned int type instead. _As shown in_: [Basic scenario](#Hello)

## Enums

### PmsStatus<a name="API_PmsStatus"></a>
```
enum PmsStatus : uint8_t {
	OK = 0,
	noData,
	readError,
	frameLenMismatch,
	sumError,
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
enum PmsDataNames : pmsIdx {
	PM1dot0CF1 = 0,       //  0
	PM2dot5CF1,           //  1
	PM10dot0CF1,          //  2
	PM1dot0,              //  3
	PM2dot5,              //  4
	PM10dot0,             //  5
	Particles0dot3,       //  6
	Particles0dot5,       //  7
	Particles1dot0,       //  8
	Particles2dot5,       //  9
	Particles5dot0,       // 10
	Particles10,          // 11
	Reserved,             // 12
	nValues_PmsDataNames  // 13
};
```

Names (indexes) of particular data received from the sensor.

Sensor transmits array of data (each of type [pmsData](#pmsData). If you are interested in particular data - use index. For example: ```data[Particles0dot3]```.

_Shown in_: [Basic scenario](#Hello)

### PmsCmd<a name="API_PmsCmd"></a>
```C++
enum PmsCmd : __uint24 {
	cmdReadData    = ...
	cmdModePassive = ...
	cmdModeActive  = ...
	cmdSleep       = ...
	cmdWakeup      = ...
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
const char *Pms5003::getDataNames(const uint8_t idx);
Serial.print(Pms5003::getDataNames(i)); // instead of Serial.print(Pms5003::dataNames[i]);
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
const char *Pms5003::getMetrics(const uint8_t idx);
Serial.print(Pms5003::getMetrics(i)); // instead of Serial.print(Pms5003::metrics[i]);
```

There is provided range-safe function to access metrics values: getMetrics();

_Shown in_: [Second example](https://github.com/jbanaszczyk/pms5003/blob/master/examples/Dynamic02/Dynamic02.ino)

## Methods

### begin()<a name="API_begin"></a>

```C++
bool begin(void);
```

Initializes Pms5003 object.

If defined ```PMS_DYNAMIC```: automatically executed by ctor: ```Pms5003()```

Should be executed by global ```setup()``` otherwise.

Initializes internal serial interface object, initializes *this fields.

_Shown in_: [Basic scenario](#Hello)

See: [Config: PMS_DYNAMIC](#Cfg_PMS_DYNAMIC)

### end()<a name="API_end"></a>

```C++
void end(void);
```

Destroys Pms5003 object.

If defined ```PMS_DYNAMIC```: automatically executed by dtor: ```~Pms5003()```

Not needed otherwise.

Shuts down internal serial interface object (if possible).


See: [Config: PMS_DYNAMIC](#Cfg_PMS_DYNAMIC)

### setTimeout, getTimeout<a name="API_setTimeout"></a><a name="API_getTimeout"></a>

```C++
void setTimeout(const unsigned long timeout);
unsigned long getTimeout(void) const;
```

By default - the most important method: ```read()``` does not block (it does not wait for data, just returns ```Pms5003::noData```).

```write()``` in case of data transfer errors may block.

```setTimeout()```, ```getTimeout()``` deals with serial port timeouts.

Default timeout set by [begin()](#API_begin) equals to ```private: timeoutPassive```, currently: 68 (twice time required to transfer 1start + 32data + 1stop using 9600bps).

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

waitForData(maxTime) works like a delay(maxTime), but can be terminated by Pms5003 sensor activity.

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
PmsStatus read(pmsData *data, const size_t nData, const uint8_t dataSize = 13);
```

**read() should not block.**

The most important function of the library. It receives, transforms and verifies data provided by PMS5003 sensor.

Arguments:
* data: pointer to an array containing _nData_ elements of type [pmsData](#pmsData).
* nData: number of data, that can be received and stored.
* dataSize: In general: don't use.

Returns [Pms5003::PmsStatus](#API_PmsStatus):
* ```Pms5003::OK```: whole, not malformed data frame was received from the sensor, up to nData elements of *data was filled according to received data.
* ```Pms5003::noData```: There is not enough data to read.
* Otherwise: refer to [errorMsg](#API_errorMsg)

_Typical usage_: [Basic scenario](#Hello)

_nData_:
* It is safe to specify nData larger or smaller than number of data provided by the sensor.
* Values from [PmsDataNames](#API_PmsDataNames) can be helpful.

_dataSize_:
* It specifies expected size of data frame: dataFrameSize = ( dataSize + 3 ) * 2;
* If there is not enough data to complete the whole frame - read() returns ```Pms5003::noData``` and does not block.
* Typical frame sent by the sensor contains 32 bytes. Appropriate dataSize value is 13 (the default).

### write<a name="API_write"></a>

```C++
bool write(const PmsCmd cmd);
```
**write() can block up to [ackTimeout](#API_ackTimeout) (currently 30milliseconds), typically about 10milliseconds.**

It sends a command to PMS5003 sensor. Refer to [commands section](#Commands).

PMS5003 responds to some [commands](#commands). The response is gathered and verified be the write() internally. That is the reason, that write() can block.

Arguments:
* cmd: one of [PmsCmd](#API_PmsCmd)

Returns:
* true: if there was no error.

_Shown in_: [Basic scenario](#Hello)

## Consts

### ackTimeout<a name="API_ackTimeout"></a>

```C++
private: static const auto ackTimeout = 30U;
```

Used internally (inside write()). Defines timeout for response read after write command. 

write() can block up to ackTimeout.

### wakeupTime<a name="API_wakeupTime"></a>

```C++
static const auto wakeupTime = 2500U;
```

wakeupTime defines time after power on, reset or write(cmdWakeup) when the sensor is blind for any commands. 

_Shown in_: [Basic scenario](#Hello)

## Commands and states<a name="commands"></a>

PMS5003 accepts a few commands. They are not fully documented. 

You can send commands to the PMS5003 sensor using [write()](#API_write).

|From State|input                |To State  |Output                                |
|----------|---------------------|----------|--------------------------------------|
|[Any]     |Power on             |Active    |Spontaneously sends data frames       |
|[Any]     |write(cmdWakeup)     |Active    |Spontaneously sends data frames       |
|[Any]     |write(cmdSleep)      |Sleep     |None (waits for cmdWakeup)            |
|Active    |write(cmdModePassive)|Passive   |None                                  |
|Passive   |write(cmdModeActive) |Active    |Spontaneously sends data frames       |
|Passive   |write(cmdReadData)   |Passive   |Sends single data frame               |




# Configuration <a name="Cfg_PMS_DYNAMIC"></a>


