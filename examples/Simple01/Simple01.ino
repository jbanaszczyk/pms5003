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
	while (!Serial) {}
	Serial.println("PMS5003");

	pms.begin();
	pms.write(Pms5003::PmsCmd::CMD_WAKEUP);
	pms.write(Pms5003::PmsCmd::CMD_MODE_ACTIVE);
	pms.waitForData(Pms5003::WAKEUP_TIME);

}

////////////////////////////////////////

// ReSharper disable once CppInconsistentNaming
void loop(void) {

	static auto lastRead = millis();

	Pms5003::PmsData buffer;
	auto status = pms.read(buffer);

	switch (status) {
	case Pms5003::PmsStatus::OK: {
		Serial.println("_________________");
		const auto newRead = millis();
		Serial.print("Wait time ");
		Serial.println(newRead - lastRead);
		lastRead = newRead;

		for (auto i = 0; i < buffer
			.raw.getSize(); ++i) {
			Serial.print(buffer.data.raw[i]);
			Serial.print("\t");
			Serial.print(buffer.data.raw.getName(i));

			Serial.print(" [");
			Serial.print(buffer.data.raw.getMetrics(i));
			Serial.print("]");
			Serial.println();
		}
		break;
	}
	case Pms5003::PmsStatus::NO_DATA:
		break;
	default:
		Serial.print("!!! Pms5003 error: ");
		Serial.println(status.getErrorMsg());
	}



/*
	const auto n = Pms5003::RESERVED;
	Pms5003::pmsData_t data[n];

	auto status = pms.read(data, n);

	switch (status) {
	case Pms5003::PmsStatus::OK: {
		Serial.println(status.getErrorMsg());	

		Serial.println("_________________");
		const auto newRead = millis();
		Serial.print("Wait time ");
		Serial.println(newRead - lastRead);
		lastRead = newRead;

		// For loop starts from 3
		// Skip the first three data (PM_1_DOT_0_CF1, PM_2_DOT_5_CF1, PM10CF1)
//		for (auto i = static_cast<unsigned int>(Pms5003::PmsDataNames::PM_1_DOT_0); i < n; ++i) {
		for (auto i = 0; i < n; ++i) {
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
	case Pms5003::PmsStatus::NO_DATA:
		break;
	default:
		Serial.print("!!! Pms5003 error: ");
		Serial.println(status.getErrorMsg());
	}

*/
}
