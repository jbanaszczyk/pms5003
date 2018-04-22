// #include <Arduino.h>

#include <pms.h>

PmsAltSerial pmsSerial;
pmsx::Pms pms(&pmsSerial);

////////////////////////////////////////

// Brown GND
// Black VCC +5V
// All signal pins: 3.3 V logic
// Green 8
// Blue  9
// optional
//   white  7
//   fiolet 6

// ReSharper disable once CppInconsistentNaming
void setup(void) {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("PMS5003");

    if (!pms.begin()) {
        Serial.println("PMS sensor: communication failed");
        return;
    }    

    pms.setPinReset(6);
    pms.setPinSleepMode(7);

    if (!pms.write(pmsx::PmsCmd::CMD_RESET)) {
        pms.write(pmsx::PmsCmd::CMD_SLEEP);
        pms.write(pmsx::PmsCmd::CMD_WAKEUP);
    }
    pms.write(pmsx::PmsCmd::CMD_MODE_PASSIVE);
    pms.write(pmsx::PmsCmd::CMD_READ_DATA);
    pms.waitForData(pmsx::Pms::TIMEOUT_PASSIVE, pmsx::PmsData::FRAME_SIZE);
    pmsx::PmsData data;
    auto status = pms.read(data);
    if (status != pmsx::PmsStatus::OK) {
        Serial.print("PMS sensor: ");
        Serial.println(status.getErrorMsg());
    }
    pms.write(pmsx::PmsCmd::CMD_MODE_ACTIVE);
    if (!pms.isWorking()) {
        Serial.println("PMS sensor failed");
    }

    Serial.print("Time of setup(): ");
    Serial.println(millis());
}

////////////////////////////////////////

// ReSharper disable once CppInconsistentNaming
void loop(void) {

    static auto lastRead = millis();

    pmsx::PmsData data;
    auto status = pms.read(data);

    switch (status) {
    case pmsx::PmsStatus::OK: {
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
    case pmsx::PmsStatus::NO_DATA:
        break;
    default:
        Serial.print("!!! Pms error: ");
        Serial.println(status.getErrorMsg());
    }
}
