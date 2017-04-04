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

Let's use [DrDiettrich' fork of AltSerial Library](https://github.com/DrDiettrich/AltSoftSerial.git). Install it.

Make some connections:
* Important: pms5003 uses 3.3V logic. Make sure your Arduino board uses 3.3V logic too, use converters if required.
* PMS5003 Pin 1: Vcc
* PMS5003 Pin 2: GND
* PMS5003 Pin 4: Digital pin 9 (there is no choice, forced by AltSerial)
* PMS5003 Pin 5: Digital pin 8 (there is no choice)

## Applications

### Hello. The Basic scenario.

Use the code:

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

	const int n = Pms5003::Reserved;
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
361	Particles < 0.5 micron [/0.1L]
43	Particles < 1.0 micron [/0.1L]
1	Particles < 2.5 micron [/0.1L]
0	Particles < 5.0 micron [/0.1L]
0	Particles < 10. micron [/0.1L]
```
