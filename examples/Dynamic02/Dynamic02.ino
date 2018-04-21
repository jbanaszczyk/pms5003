#include <Arduino.h>
#include <pms.h>

////////////////////////////////////////

//
// Please uncomment #define PMS_DYNAMIC in pmsConfig.h file
//

PmsAltSerial pmsSerial;

#if defined PMS_DYNAMIC
Pms* pms_ = nullptr;
#define pms (*pms_)
#else
pmsx::Pms pms(&pmsSerial);
#endif

// Green 8
// Blue  9
// Brown GND
// Black VCC

////////////////////////////////////////

// ReSharper disable once CppInconsistentNaming
void setup(void) {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("PMS5003");

    #if defined PMS_DYNAMIC
    pms_ = new Pms(&pmsSerial);
    #else
    // ReSharper disable once CppExpressionWithoutSideEffects
    if (! pms.begin() ) {
        Serial.println("Serial communication with PMS sensor failed");
        return;
    }
    #endif

    pms.write(pmsx::PmsCmd::CMD_WAKEUP);
    pms.write(pmsx::PmsCmd::CMD_MODE_ACTIVE);
    pms.waitForData(pmsx::Pms::WAKEUP_TIME);
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
            for (auto i = 0; i < view.SIZE; ++i) {
                Serial.print(view[i]);
                Serial.print("\t");
                Serial.print(view.names[i]);
                Serial.print("\t");
                Serial.print(view.diameters[i]);
                Serial.print(" [");
                Serial.print(view.metrics[i]);
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
