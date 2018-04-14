#include <Arduino.h>
#include <pms.h>

////////////////////////////////////////

//
// Please uncomment #define PMS_DYNAMIC in pmsConfig.h file
//

PmsAltSerial pmsSerial;

#if defined PMS_DYNAMIC
Pms5003 *pms_ = nullptr;
#define pms (*pms_)
#else
Pms5003 pms(&pmsSerial);
#endif

////////////////////////////////////////

// ReSharper disable CppInconsistentNaming
void setup(void) {
	// ReSharper restore CppInconsistentNaming
	Serial.begin(115200);
	while (!Serial) {}
	Serial.println("PMS5003");

#if defined PMS_DYNAMIC
	pms_ = new Pms5003(&pmsSerial);
#else
	pms.begin();
#endif

	pms.write(Pms5003::CMD_WAKEUP);
	pms.write(Pms5003::CMD_MODE_ACTIVE);
	pms.waitForData(Pms5003::WAKEUP_TIME);
}

////////////////////////////////////////

auto lastRead = millis();

// ReSharper disable once CppInconsistentNaming
void loop(void) {

	const Pms5003::pmsIdx_t n = Pms5003::N_VALUES_PMSDATANAMES;
	Pms5003::pmsData_t data[n];

	const auto t0Read = millis();
	const Pms5003::PmsStatus status = pms.read(data, n);
	const auto t1Read = millis();

	switch (status) {
	case Pms5003::PmsStatus::OK: {
		Serial.print("_________________ time of read(): ");
		Serial.print(t1Read - t0Read);
		Serial.println(" msec");
		const auto newRead = millis();
		Serial.print("Wait time ");
		Serial.println(newRead - lastRead);
		lastRead = newRead;

		for (Pms5003::pmsIdx_t i = 0; i < n; ++i) {
			Serial.print(data[i]);
			Serial.print("\t");
			Serial.print(Pms5003::getDataNames(i));
			Serial.print(" [");
			Serial.print(Pms5003::getMetrics(i));
			Serial.print("]");
			Serial.println();
		}
		break;
	}
	case Pms5003::PmsStatus::NO_DATA:
		break;
	default:
		Serial.println("_________________");
		Serial.println(Pms5003::errorMsg[status]);
	}
}
