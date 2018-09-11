#include <pms.h>

using namespace pmsx;

PmsAltSerial pmsSerial;
Pms pms(&pmsSerial);

////////////////////////////////////////

// * PMS5003 Pin 1 : VCC +5V
// * PMS5003 Pin 2 : GND
// Important: pms5003 uses 3.3V logic.Use converters if required or make sure your Arduino board uses 3.3V logic too.
// * PMS5003 Pin 4 : Digital pin 9 (there is no choice, forced by AltSerial)
// * PMS5003 Pin 5 : Digital pin 8 (there is no choice, forced by AltSerial)
// * Optional
//   * PMS5003 Pin 3 : Digital pin 7 (can be changed or not connected at all)
//   * PMS5003 Pin 6 : Digital pin 6 (can be changed or not connected at all)

// if PMS5003 Pin 3  and PMS5003 Pin 3 are not connected
// constexpr uint8_t pinReset = pinNone;
// constexpr uint8_t pinSleepMode = pinNone;

// if PMS5003 Pin 3  and PMS5003 Pin 3 are connected
constexpr uint8_t pinReset = 6;
constexpr uint8_t pinSleepMode = 7;

////////////////////////////////////////

void setup(void) {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println(pmsxApiVersion);

    if (!pms.begin()) {
        Serial.println("PMS sensor: communication failed");
        return;
    }

	pms.setPinReset(pinReset);
	pms.setPinSleepMode(pinSleepMode);

    if (!pms.write(PmsCmd::CMD_RESET)) {
        pms.write(PmsCmd::CMD_SLEEP);
        pms.write(PmsCmd::CMD_WAKEUP);
    }
    pms.write(PmsCmd::CMD_MODE_PASSIVE);
    pms.write(PmsCmd::CMD_READ_DATA);
    pms.waitForData(Pms::TIMEOUT_PASSIVE, PmsData::FRAME_SIZE);
    PmsData data;
    auto status = pms.read(data);
    if (status != PmsStatus::OK) {
        Serial.print("PMS sensor: ");
        Serial.println(status.getErrorMsg());
    }
    pms.write(PmsCmd::CMD_MODE_ACTIVE);
    if (!pms.isWorking()) {
        Serial.println("PMS sensor failed");
    }

    Serial.print("Time of setup(): ");
    Serial.println(millis());
}

////////////////////////////////////////

void loop(void) {

    static auto lastRead = millis();

    PmsData data;
    auto status = pms.read(data);

    switch (status) {
    case PmsStatus::OK: {
        Serial.println("_________________");
        const auto newRead = millis();
        Serial.print("Wait time ");
        Serial.println(newRead - lastRead);
        lastRead = newRead;

        auto view = data.particles;
        for (PmsData::pmsIdx_t i = 0; i < view.getSize(); ++i) {
            Serial.print(view.getValue(i));
            Serial.print("\t");
            Serial.print(view.getName(i));
            Serial.print(" [");
            Serial.print(view.getMetric(i));
            Serial.print("] ");
            Serial.print(" Level: ");
            Serial.print(view.getLevel(i));
            Serial.print(" | diameter: ");
            Serial.print(view.getDiameter(i));
            Serial.println();
        }
        break;
    }
    case PmsStatus::NO_DATA:
        break;
    default:
        Serial.print("!!! Pms error: ");
        Serial.println(status.getErrorMsg());
    }
}
