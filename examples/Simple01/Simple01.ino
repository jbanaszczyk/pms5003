// #include <Arduino.h>

#include <pms.h>

PmsAltSerial pmsSerial;
Pms5003 pms(&pmsSerial);

////////////////////////////////////////

// Green 8
// Blue  9
// Brown GND
// Black VCC

// ReSharper disable once CppInconsistentNaming
void setup(void) {
	Serial.begin(115200);
	while (!Serial) {};
	Serial.println("PMS5003");

	pms.begin();
	pms.write(Pms5003::cmdWakeup);
	pms.write(Pms5003::cmdModeActive);
	pms.waitForData(Pms5003::wakeupTime);
}

////////////////////////////////////////

// ReSharper disable once CppInconsistentNaming
void loop(void) {

	static auto lastRead = millis();

	const auto n = Pms5003::Reserved;
	Pms5003::pmsData data[n];

	const Pms5003::PmsStatus status = pms.read(data, n);

	switch (status) {
	case Pms5003::OK: {
		Serial.println("_________________");
		const auto newRead = millis();
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
	case Pms5003::NO_DATA:
		break;
	default:
		Serial.println("_________________");
		Serial.println(Pms5003::errorMsg[status]);
	};
}
