#include <Arduino.h>
#include <pms.h>

////////////////////////////////////////

//
// Please uncomment #define PMS_DYNAMIC in pmsConfig.h file
//

PmsAltSerial pmsSerial;

#if defined PMS_DYNAMIC
Pms *pms_ = nullptr;
#define pms (*pms_)
#else
Pms pms(&pmsSerial);
#endif

////////////////////////////////////////

// ReSharper disable CppInconsistentNaming
void setup(void) {
	// ReSharper restore CppInconsistentNaming
	Serial.begin(115200);
	while (!Serial) {}
	Serial.println("PMS5003");


#if defined PMS_DYNAMIC
	pms_ = new Pms(&pmsSerial);
#else
	// ReSharper disable once CppExpressionWithoutSideEffects
	pms.begin();
#endif

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
