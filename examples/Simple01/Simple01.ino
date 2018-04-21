// #include <Arduino.h>

#include <pms.h>

PmsAltSerial pmsSerial;
pmsx::Pms pms(&pmsSerial);

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

    if (!pms.begin() ) {
        Serial.println("Serial communication with PMS sensor failed");
        return;
    }
    
    pms.write(pmsx::PmsCmd::CMD_WAKEUP);
    pms.write(pmsx::PmsCmd::CMD_MODE_ACTIVE);
    pms.waitForData(pmsx::Pms::WAKEUP_TIME);

    pms.write(pmsx::PmsCmd::CMD_MODE_PASSIVE);

    for ( auto i = 0; i < 100; ++i ) {
        pms.write(pmsx::PmsCmd::CMD_SLEEP);
        delay(50);
        pms.write(pmsx::PmsCmd::CMD_WAKEUP);
        delay(50);

    }

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

            auto view   = data.particles;
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
