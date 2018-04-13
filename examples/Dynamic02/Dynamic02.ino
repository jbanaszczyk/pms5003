#include <Arduino.h>
#include <pms.h>

////////////////////////////////////////

//
// Please uncomment #define PMS_DYNAMIC in pmsConfig.h file
//

#if defined PMS_DYNAMIC
Pms5003 *pms_ = nullptr;
#define pms (*pms_)
#else
Pms5003 pms;
#endif

////////////////////////////////////////

void setup(void) {
	Serial.begin(115200);
	while (!Serial) {};
	Serial.println("PMS5003");

#if defined PMS_DYNAMIC
	pms_ = new Pms5003();
#else
	pms.begin();
#endif 

	pms.waitForData(Pms5003::wakeupTime);
	pms.write(Pms5003::cmdModeActive);
}

////////////////////////////////////////

auto lastRead = millis();

void loop(void) {

	const Pms5003::pmsIdx n = Pms5003::nValues_PmsDataNames;
	Pms5003::pmsData data[n];

	auto t0Read = millis();
	Pms5003::PmsStatus status = pms.read(data, n);
	auto t1Read = millis();

	switch (status) {
	case Pms5003::OK:
	{
		Serial.print("_________________ time of read(): ");
		Serial.print(t1Read - t0Read);
		Serial.println(" msec");
		auto newRead = millis();
		Serial.print("Wait time ");
		Serial.println(newRead - lastRead);
		lastRead = newRead;

		for (Pms5003::pmsIdx i = 0; i < n; ++i) {
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
	case Pms5003::noData:
		break;
	default:
		Serial.println("_________________");
		Serial.println(Pms5003::errorMsg[status]);
	};
}
