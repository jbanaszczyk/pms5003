
# #include <pms.h>

To use pms5003 library just install the library and `#include <pms.h>`

Nothing more. All described files are included automatically.

# Serial driver

To use another communication library:

* Implement interface `IPmsSerial` defined in the pmsSerial.h 
* It defines kind of proxy between available serial communications libraries and pms5003 operations
  * My implementation for clone of AltSoftSerial is located in pmsSerialAltSoftSerial.h
* Modify pmsConfig.h
  * somewhere near `PMS_ALTSOFTSERIAL` occurrences
* Create object `yourDriver` of your implementation.
  * Use that object in constructor `Pms(&yourDriver)` or `.addSerial(&yourDriver)`

# pmsConfig.h

## #define PMS_DYNAMIC

Described in [Initialization C++ Style](README.md#initialization-c-style)

# tribool.h

It is a port of [boost.tribool](http://www.boost.org/doc/libs/1_63_0/doc/html/tribool.html) library - tristate logic: yes/no/unknown. It is used as a type of return value of `isModeActive()` and `isModeSleep()`. Sample code:
```C++
    if ( pms.isModeActive()) {
        Serial.println("Active");
    } else if ( !pms.isModeActive()) {
        Serial.println("Passive");
    } else {
        Serial.println("Unknown");        
    }
```

Please note, that the goal of the code within `setup()` is to enter `isModeActive()` and `isModeSleep()` into any known mode (true or false). Than (till `setSerial()`) state of `pms` is always known.
```C++
    if (!pms.write(pmsx::PmsCmd::CMD_RESET)) {
        pms.write(pmsx::PmsCmd::CMD_SLEEP);
        pms.write(pmsx::PmsCmd::CMD_WAKEUP);
    }
```

tribool.h defines both:
* third state
* tribool operators: `&&`, `||`, `==`, `!=`

# pms.h

## namespace pmsx

Almost all entities defined by <pms.h> are located in `pmsx` namespace.

Please refer to [namespace pmsx{}](README.md#namespace-pmsx) description how to save typing _pmsx::_

## pmsxApiVersion

* `constexpr char pmsxApiVersion[]`
  * defines some kind of pms5003 API version

## PmsStatus

* class PmsStatus
  * status of read() operations. PmsStatus looks like enum class, but there is additional method getErrorMsg()
  * values:
    * PmsStatus::OK
    * PmsStatus::NO_DATA
    * PmsStatus::READ_ERROR
    * PmsStatus::FRAME_LENGTH_MISMATCH
    * PmsStatus::SUM_ERROR
    * PmsStatus::NO_SERIAL
  * getErrorMsg()
    * returns description of the status

## pmsData_t

* `typedef ... pmsData_t`
  * type of data transmitted from PMS5003 sensor





















---
---
# Description below is not updated yet since version 1.0
---
---





## TODO

```

    


    class PmsData {
        typedef uint8_t pmsIdx_t;
        static constexpr pmsIdx_t DATA_SIZE = 13;
        static const size_t RESPONSE_FRAME_SIZE = (1 + 3) * sizeof(pmsData_t); // Frame size for single pmsData_t response (after write command)
        static constexpr size_t FRAME_SIZE      = (DATA_SIZE + 3) * sizeof(pmsData_t); // useful for waitForData()
        static constexpr size_t getFrameSize() {

    public:
        template <pmsIdx_t Size, pmsIdx_t Ofset>
        class PmsConcentrationData {
            pmsData_t data[Size];
        public:

            static constexpr pmsIdx_t SIZE = Size;

            static constexpr pmsIdx_t getSize() {
            pmsData_t& operator[](pmsIdx_t index) {
            pmsData_t getValue(pmsIdx_t index) const {


            static Names_<Ofset> names;
            static Metrics_<Ofset> metrics;
            static Diameters_<Ofset> diameters;

            static const char* getName(pmsIdx_t index) {
                return names[index];
            }

            static const char* getMetric(pmsIdx_t index) {
                return metrics[index];
            }

            static float getDiameter(pmsIdx_t index) {
                return diameters[index];
            }
        };

        class Cleanliness : public PmsConcentrationData<6, 6> {
        public:
            // according to ISO 14644-1:2002 http://www.instalacje-sanitarne.waw.pl/poradnik.html
            float getLevel(pmsIdx_t index) const {
                return (getValue(index) == 0) ? 0.0 : 4.0 + log10(getValue(index) * pow(getDiameter(index) * 10.0, 2.08));
            }
        };

        union {
            PmsConcentrationData<13, 0> raw;

            struct {
                PmsConcentrationData<3, 0> concentrationCf;
                PmsConcentrationData<3, 3> concentration;
                Cleanliness particles;
                PmsConcentrationData<1, 12> reserved;
            };
        };
    };


    enum class PmsCmd : __uint24 {
        CMD_READ_DATA = 0x0000e2L,
        CMD_MODE_PASSIVE = 0x0000e1L,
        CMD_MODE_ACTIVE = 0x0100e1L,
        CMD_SLEEP = 0x0000e4L,
        CMD_WAKEUP = 0x0100e4L,
        CMD_RESET = 0xffffffL,
    };

    class Pms {
    public:
        tribool isModeActive() const {
        tribool isModeSleep() const {
        bool isWorking() const {
        void setPinSleepMode(uint8_t value) {
        void setPinReset(uint8_t value) {
    public:
        static constexpr decltype(timeout) TIMEOUT_PASSIVE = 68U;  // Transfer time of 1start + 32data + 1stop using 9600bps is 33 usec. TIMEOUT_PASSIVE could be at least 34, Value of 68 is an arbitrary doubled
        static constexpr auto WAKEUP_TIME                  = 2500U; // Experimentally, time to get ready after reset/wakeup

        Pms() : modeActive(tribool(unknown)), modeSleep(tribool(unknown)), timeout(TIMEOUT_PASSIVE) {
        explicit Pms(IPmsSerial* pmsSerial) : Pms() {
        ~Pms() {
        void addSerial(IPmsSerial* pmsSerial) {
        bool begin(void) {
        void end(void) const {
        bool initialized() {
    public:
        void setTimeout(decltype(timeout) timeout) {
        decltype(timeout) getTimeout(void) const {
        size_t available(void) {
        bool waitForData(unsigned int maxTime, size_t nData = 0) {
        PmsStatus read(PmsData& data) {
        PmsStatus read(pmsData_t* data, size_t nData) {
        bool write(PmsCmd cmd, unsigned int wakeupTime = Pms::WAKEUP_TIME) {
```









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

