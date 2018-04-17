// #include <Arduino.h>

#include <pms.h>

PmsAltSerial pmsSerial;
Pms pms(&pmsSerial);

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
	pms.write(Pms::PmsCmd::CMD_WAKEUP);
	pms.write(Pms::PmsCmd::CMD_MODE_ACTIVE);
	pms.waitForData(Pms::WAKEUP_TIME);
}

////////////////////////////////////////

// ReSharper disable once CppInconsistentNaming
void loop(void) {

	static auto lastRead = millis();

	Pms::PmsData data;
	auto status = pms.read(data);

	switch (status) {
	case Pms::PmsStatus::OK: {
		Serial.println("_________________");
		const auto newRead = millis();
		Serial.print("Wait time ");
		Serial.println(newRead - lastRead);
		lastRead = newRead;

		auto view = data.particles;
		for (auto i = 0; i < view.getSize(); ++i) {
			Serial.print(view[i]);
			Serial.print("\t");
			Serial.print(view.getName(i));

			Serial.print(" [");
			Serial.print(view.getMetric(i));
			Serial.print("] ");
			Serial.print(view.getLevel(i));
			Serial.println();
		}
		break;
	}
	case Pms::PmsStatus::NO_DATA:
		break;
	default:
		Serial.print("!!! Pms error: ");
		Serial.println(status.getErrorMsg());
	}
}
