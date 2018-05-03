#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <tribool.h>
#include <pmsConfig.h>
#include <pmsSerial.h>

// Important: Use 3.3V logic
// Pin 1: Vcc
// Pin 2: GND

// Using AltSoftSerial:
//   Pin 4: Digital pin 9 (there is no choice)
//   Pin 5: Digital pin 8 (there is no choice)
//   in file pmsConfig.h: Enable #define PMS_ALTSOFTSERIAL
//   Install DrDiettrich' fork of AltSoftSerial Library: https://github.com/DrDiettrich/AltSoftSerial.git

////////////////////////////////////////

// Some C++11 stuff

#if defined NOMINMAX

#if defined min
#undef min
#define min _min11
#endif

#if defined max
#undef max
#define max _max11
#endif

#endif


template <class T>
inline const T& __attribute__((always_inline))_min11(const T& a, const T& b);

template <class T>
inline const T& __attribute__((always_inline))_min11(const T& a, const T& b) {
    return !(b < a) ? a : b;
}

template <class T>
inline const T& __attribute__((always_inline))_max11(const T& a, const T& b);

template <class T>
inline const T& __attribute__((always_inline))_max11(const T& a, const T& b) {
    return !(b > a) ? a : b;
}

#ifndef _countof
template <typename _CountofType, size_t _SizeOfArray>
char (* __countof_helper(_CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) (sizeof(*__countof_helper(_Array)) + 0)

#endif

#if ! defined __INT24_MAX__

using __uint24 = uint32_t;

#endif

////////////////////////////////////////

namespace pmsx {

    typedef uint16_t pmsData_t;

    class PmsStatus {
    public:
        using pmsStatus_t = uint8_t;
        pmsStatus_t value;
        explicit PmsStatus(pmsStatus_t value) : value(value) {}

        operator pmsStatus_t() {
            return value;
        }

        static constexpr pmsStatus_t OK                    = 0;
        static constexpr pmsStatus_t NO_DATA               = 1 + OK;
        static constexpr pmsStatus_t READ_ERROR            = 1 + NO_DATA;
        static constexpr pmsStatus_t FRAME_LENGTH_MISMATCH = 1 + READ_ERROR;
        static constexpr pmsStatus_t SUM_ERROR             = 1 + FRAME_LENGTH_MISMATCH;
        static constexpr pmsStatus_t NO_SERIAL             = 1 + SUM_ERROR;

    public:
        const char* getErrorMsg() {
            static const char* errorMsg[]{
                "OK",
                "No data",
                "Read error",
                "Frame length mismatch",
                "CRC Error",
                "Serial port not initialized",
                "Status:unknown"
            };
            return errorMsg[_min11(value, static_cast<decltype(value)> _countof(errorMsg))];
        }
    };

    class PmsData {
    public:
        typedef uint8_t pmsIdx_t;
        static constexpr pmsIdx_t DATA_SIZE = 13;
    private:
        static const float DIAMETERS[DATA_SIZE];
        static const char* const METRICS[DATA_SIZE];
    public:
        static const size_t RESPONSE_FRAME_SIZE = (1 + 3) * sizeof(pmsData_t); // Frame size for single pmsData_t response (after write command)
        static constexpr size_t FRAME_SIZE      = (DATA_SIZE + 3) * sizeof(pmsData_t); // useful for waitForData()
        static constexpr size_t getFrameSize() {
            return FRAME_SIZE;
        } // useful for waitForData()
    private:
        template <pmsIdx_t Ofset>
        class Names_ {
        public:
            const char* & operator[](pmsIdx_t index) {
                static const char* NAMES[]{
                    "PM1.0, CF=1",
                    "PM2.5, CF=1",
                    "PM10.  CF=1",
                    "PM1.0",
                    "PM2.5",
                    "PM10.",
                    "Particles > 0.3 micron",
                    "Particles > 0.5 micron",
                    "Particles > 1.0 micron",
                    "Particles > 2.5 micron",
                    "Particles > 5.0 micron",
                    "Particles > 10. micron",
                    "Reserved_0"
                };
                return NAMES[_min11(static_cast<decltype(Ofset + index)>(Ofset + index), static_cast<decltype(Ofset + index)>(_countof(NAMES)))];
            }
        };

        template <pmsIdx_t Ofset>
        class Metrics_ {
        public:
            const char* & operator[](pmsIdx_t index) {
                static const char* METRICS[]{
                    "micro g/m3",
                    "micro g/m3",
                    "micro g/m3",

                    "micro g/m3",
                    "micro g/m3",
                    "micro g/m3",

                    "/0.1L",
                    "/0.1L",
                    "/0.1L",
                    "/0.1L",
                    "/0.1L",
                    "/0.1L",

                    "???"
                };
                return METRICS[_min11(static_cast<decltype(Ofset + index)>(Ofset + index), static_cast<decltype(Ofset + index)>(_countof(METRICS)))];
            }
        };

        template <pmsIdx_t Ofset>
        class Diameters_ {
        public:
            const float& operator[](pmsIdx_t index) {
                static const float DIAMETERS[]{
                    1.0f,
                    2.5f,
                    10.0f,

                    1.0f,
                    2.5f,
                    10.0f,

                    0.3f,
                    0.5f,
                    1.0f,
                    2.5f,
                    5.0f,
                    10.0f,

                    0.0f
                };
                return DIAMETERS[_min11(static_cast<decltype(Ofset + index)>(Ofset + index), static_cast<decltype(Ofset + index)>(_countof(DIAMETERS)))];
            }
        };

    public:
        template <pmsIdx_t Size, pmsIdx_t Ofset>
        class PmsConcentrationData {
            pmsData_t data[Size];
        public:

            static constexpr pmsIdx_t SIZE = Size;

            static constexpr pmsIdx_t getSize() {
                return SIZE;
            };

            pmsData_t& operator[](pmsIdx_t index) {
                return data[index];
            }

            pmsData_t getValue(pmsIdx_t index) const {
                return data[index];
            }

            static Names_<Ofset> names;
            static Metrics_<Ofset> metrics;
            static Diameters_<Ofset> diameters;

            static const char* getName(pmsIdx_t index) {
                return names[index];
            }

            static const char* getMetric(pmsIdx_t index) {
                return metrics[index];
            }

            static float getDiameter(pmsIdx_t index) {
                return diameters[index];
            }
        };

        class Cleanliness : public PmsConcentrationData<6, 6> {
        public:
            // according to ISO 14644-1:2002 http://www.instalacje-sanitarne.waw.pl/poradnik.html
            float getLevel(pmsIdx_t index) const {
                return (getValue(index) == 0) ? 0.0 : 4.0 + log10(getValue(index) * pow(getDiameter(index) * 10.0, 2.08));
            }
        };

        union {
            PmsConcentrationData<13, 0> raw;

            struct {
                PmsConcentrationData<3, 0> concentrationCf;
                PmsConcentrationData<3, 3> concentration;
                Cleanliness particles;
                PmsConcentrationData<1, 12> reserved;
            };
        };
    };

    static_assert(sizeof(PmsData) == PmsData::DATA_SIZE * sizeof(pmsData_t), "PmsData: wrong sizeof()");

    enum class PmsCmd : __uint24 {
        CMD_READ_DATA = 0x0000e2L,
        CMD_MODE_PASSIVE = 0x0000e1L,
        CMD_MODE_ACTIVE = 0x0100e1L,
        CMD_SLEEP = 0x0000e4L,
        CMD_WAKEUP = 0x0100e4L,
        CMD_RESET = 0xffffffL,
    };

    class Pms {
    private:

        template <typename T, T NullValue>
        class compact_optional {
        private:
            T storage;
        public:
            compact_optional() : storage(NullValue) { }
            explicit compact_optional(T storage) : storage(storage) { }

            void unSet() {
                storage = NullValue;
            }

            T& operator=(const T& other) {
                storage = other;
                return storage;
            }

            bool hasValue() const {
                return storage != NullValue;
            }

            explicit operator bool() const {
                return hasValue();
            }

            T getValue() {
                return storage;
            }

            operator T() const {
                return storage;
            }

            const T& operator->() const {
                return storage;
            }

        };

        bool dataReceived;
        bool dataSent;

        tribool modeActive;
        tribool modeSleep;

        void clrState() {
            dataReceived = false;
            dataSent     = false;
            modeActive   = tribool(unknown);
            modeSleep    = tribool(unknown);
            pinSleepMode.unSet();
            pinReset.unSet();
        }

    public:
        tribool isModeActive() const {
            return modeActive;
        }

        tribool isModeSleep() const {
            return modeSleep;
        }

        bool isWorking() const {
            return dataReceived && dataSent;
        }

    private:
        using optionalPin_t = compact_optional<uint8_t, UINT8_MAX>;
        optionalPin_t pinSleepMode;
        optionalPin_t pinReset;

        void setupHardwarePin(uint8_t value) {
            digitalWrite(value, HIGH);
            pinMode(value, OUTPUT);
            digitalWrite(value, HIGH);
        }

    public:
        void setPinSleepMode(uint8_t value) {
            pinSleepMode = value;
            setupHardwarePin(value);
        }

        void setPinReset(uint8_t value) {
            pinReset = value;
            setupHardwarePin(value);
        }

    private:
        const uint8_t sig[2]{0x42, 0x4D};
        unsigned long timeout;
        static constexpr auto TIMEOUT_ACK             = 30U;  // Time to complete response after write command
        static constexpr auto BAUD_RATE               = 9600U; // used during begin()
        static constexpr unsigned long RESET_DURATION = 33U; // See doHwReset()

        compact_optional<IPmsSerial*, nullptr> pmsSerial;

    public:
        static constexpr decltype(timeout) TIMEOUT_PASSIVE = 68U;  // Transfer time of 1start + 32data + 1stop using 9600bps is 33 usec. TIMEOUT_PASSIVE could be at least 34, Value of 68 is an arbitrary doubled
        static constexpr auto WAKEUP_TIME                  = 2500U; // Experimentally, time to get ready after reset/wakeup

        Pms() : modeActive(tribool(unknown)), modeSleep(tribool(unknown)), timeout(TIMEOUT_PASSIVE) {
            addSerial(nullptr);
        };

        explicit Pms(IPmsSerial* pmsSerial) : Pms() {
            addSerial(pmsSerial);
            #if defined PMS_DYNAMIC
            begin();
            #endif
        }

        ~Pms() {
            #if defined PMS_DYNAMIC
            end();
            #endif
            pmsSerial.unSet();
        }

        void addSerial(IPmsSerial* pmsSerial) {
            clrState();
            this->pmsSerial = pmsSerial;
        }

        bool begin(void) {
            if (!pmsSerial) {
                return false;
            }

            pmsSerial->setTimeout(TIMEOUT_PASSIVE);
            if (pmsSerial->begin(BAUD_RATE)) {
                return true;
            }
            addSerial(nullptr);
            return false;
        }

        void end(void) const {
            if (pmsSerial) {
                pmsSerial->end();
            }
        }

        bool initialized() {
            return pmsSerial;
        }

        ////////////////////////////////////////

    private:
        void __attribute__((always_inline))swapEndianBig16(uint16_t* value) {
            constexpr union {
                // endian.test16 == 0x0001 for low endian
                // endian.test16 == 0x0100 for big endian
                // should be properly optimized by compiler
                uint16_t test16;
                uint8_t test8[2];
            } endian = {.test8 = {1, 0}};

            if (endian.test16 != 0x0100) {
                const uint16_t hi = (*value & 0xff00) >> 8;
                const uint16_t lo = (*value & 0x00ff) << 8;
                *value            = lo | hi;
            }
        }

        ////////////////////////////////////////

        void sumBuffer(uint16_t* sum, const uint8_t* buffer, uint16_t cnt) {
            for (; cnt > 0; --cnt, ++buffer) {
                *sum += *buffer;
            }
        }

        static void sumBuffer(uint16_t* sum, const uint16_t data) {
            *sum += (data & 0xFF) + (data >> 8);
        }

        ////////////////////////////////////////

    public:
        void setTimeout(decltype(timeout) timeout) {
            this->timeout = timeout;
            if (pmsSerial) {
                pmsSerial->setTimeout(timeout);
            }
        }

        decltype(timeout) getTimeout(void) const {
            return timeout;
        }

        size_t available(void) {
            if (!pmsSerial) {
                return 0;
            }
            skipGarbage();
            return pmsSerial->available();
        }

        bool waitForData(unsigned int maxTime, size_t nData = 0) {
            if (!pmsSerial) {
                return false;
            }

            const auto t0 = millis();
            if (nData == 0) {
                for (; millis() - t0 < maxTime; delay(1)) {
                    if (pmsSerial->available()) {
                        return true;
                    }
                }
                return pmsSerial->available();
            }

            for (; millis() - t0 < maxTime; delay(1)) {
                if (available() >= nData) {
                    return true;
                }
            }
            return available() >= nData;
        }

        PmsStatus read(PmsData& data) {
            return read(reinterpret_cast<pmsData_t *>(&data.raw), data.raw.getSize());
        }

        PmsStatus read(pmsData_t* data, size_t nData) {
            if (!pmsSerial) {
                return PmsStatus{PmsStatus::NO_SERIAL};
            }

            skipGarbage();

            if (available() < (nData + 2) * sizeof*data + sizeof(sig)) {
                return PmsStatus{PmsStatus::NO_DATA};
            }

            pmsSerial->read(); // Due to previous skipGarbage(): value is equal to sig[0]. There is no need to check the value

            if (pmsSerial->read() != sig[1]) {
                return PmsStatus{PmsStatus::READ_ERROR}; // The rest of the buffer will be invalidated during the next read attempt
            }

            uint16_t sum{0};
            sumBuffer(&sum, (uint8_t *)&sig, sizeof(sig));

            pmsData_t thisFrameLen{0};

            if (pmsSerial->read((uint8_t*)&thisFrameLen, sizeof thisFrameLen) != sizeof thisFrameLen) {
                return PmsStatus{PmsStatus::READ_ERROR};
            }

            sumBuffer(&sum, thisFrameLen);
            swapEndianBig16(&thisFrameLen);

            size_t toRead{thisFrameLen - sizeof thisFrameLen};
            if (toRead > nData * sizeof*data) {
                return PmsStatus{PmsStatus::FRAME_LENGTH_MISMATCH};
            }

            if (data != nullptr) {
                if (pmsSerial->read((uint8_t*)data, toRead) != toRead) {
                    return PmsStatus{PmsStatus::READ_ERROR};
                }
                sumBuffer(&sum, (uint8_t*)data, toRead);

                for (size_t i = 0; i < nData; ++i) {
                    swapEndianBig16(&data[i]);
                }
            }

            pmsData_t crc;
            for (; toRead < thisFrameLen; toRead += 2) {

                if (pmsSerial->read((uint8_t*)&crc, sizeof crc) != sizeof crc) {
                    return PmsStatus{PmsStatus::READ_ERROR};
                }

                if (toRead < thisFrameLen - 2) {
                    sumBuffer(&sum, crc);
                }
            }

            swapEndianBig16(&crc);

            if (sum != crc) {
                return PmsStatus{PmsStatus::SUM_ERROR};
            }

            dataReceived = true;
            return PmsStatus{PmsStatus::OK};
        }

    private:
        void setNewMode(const PmsCmd cmd) {
            switch (cmd) {
                case PmsCmd::CMD_MODE_PASSIVE:
                    modeActive = tribool(false);
                    break;
                case PmsCmd::CMD_MODE_ACTIVE:
                    modeActive = tribool(true);
                    break;
                case PmsCmd::CMD_SLEEP:
                    modeSleep = tribool(true);
                    break;
                case PmsCmd::CMD_WAKEUP:
                case PmsCmd::CMD_RESET:
                    modeSleep  = tribool(false);
                    modeActive = tribool(true);
                    break;
                default:
                    break;
            }
        }

        bool doHwReset(const unsigned int wakeupTime) {
            if (!pinReset) {
                return false;
            }
            digitalWrite(pinReset, LOW);
            delay(RESET_DURATION);
            pmsSerial->flushInput();
            digitalWrite(pinReset, HIGH);
            setNewMode(PmsCmd::CMD_RESET);
            dataReceived = false;
            dataSent     = false;
            if (wakeupTime > 0) {
                waitForData(wakeupTime, wakeupTime);
                skipGarbage();
            }
            return true;
        }

        bool doHwSleep(const PmsCmd cmd, const unsigned int wakeupTime) {
            if (!pinSleepMode) {
                return false;
            }

            digitalWrite(pinSleepMode, cmd == PmsCmd::CMD_SLEEP ? LOW : HIGH);
            delay(RESET_DURATION);
            pmsSerial->flushInput();
            setNewMode(cmd);
            if ( cmd == PmsCmd::CMD_WAKEUP  && wakeupTime > 0 ) {
                waitForData(wakeupTime, wakeupTime);
                skipGarbage();
            }
            return true;
        }

    public:
        bool write(PmsCmd cmd, unsigned int wakeupTime = Pms::WAKEUP_TIME) {
            static_assert(sizeof cmd >= 3, "Wrong definition of PmsCmd (too short)");

            if (cmd == PmsCmd::CMD_RESET) {
                return doHwReset(wakeupTime);
            }

            if ( ( cmd == PmsCmd::CMD_SLEEP || cmd == PmsCmd::CMD_WAKEUP ) && doHwSleep(cmd, wakeupTime) ) {
                return true;
            }

            if (!pmsSerial) {
                return false;
            }

            if (pmsSerial->write(sig, sizeof(sig)) != sizeof(sig)) {
                return false;
            }
            const size_t cmdSize = 3;
            if (pmsSerial->write((uint8_t*)&cmd, cmdSize) != cmdSize) {
                return false;
            }

            uint16_t sum{0};
            sumBuffer(&sum, sig, sizeof(sig));
            sumBuffer(&sum, (uint8_t*)&cmd, cmdSize);
            swapEndianBig16(&sum);

            if (cmd != PmsCmd::CMD_READ_DATA && cmd != PmsCmd::CMD_MODE_ACTIVE) {
                pmsSerial->flushInput();
            }

            if (pmsSerial->write((uint8_t*)&sum, sizeof sum) != sizeof sum) {
                return false;
            }

            if (cmd != PmsCmd::CMD_READ_DATA && cmd != PmsCmd::CMD_MODE_ACTIVE) {
                // sensor sometimes tries to send response frame, containing original command (2 bytes)
                skipGarbage();
                waitForData(TIMEOUT_ACK, PmsData::RESPONSE_FRAME_SIZE);
                pmsSerial->flushInput();
            }

            if ((cmd == PmsCmd::CMD_WAKEUP) && (wakeupTime > 0)) {
                waitForData(wakeupTime, PmsData::RESPONSE_FRAME_SIZE);
                skipGarbage();
            }

            setNewMode(cmd);

            dataSent = true;
            return true;
        }

    private:
        void skipGarbage() {
            while ((pmsSerial->available()) && (pmsSerial->peek() != sig[0])) {
                pmsSerial->read();
            }
        }

        void serialMonitor(unsigned long int duration) {
            if (!Serial) {
                return;
            }

            const auto t0 = millis();
            Serial.println("==[ Port monitor ]==");

            for (; millis() - t0 < duration; delay(1)) {
                while (pmsSerial->available()) {
                    Serial.print("==  ");
                    Serial.print(millis() - t0);
                    Serial.print(" : ");
                    Serial.println(pmsSerial->read());
                }
            }
            Serial.println("==[              ]==");
        }

    };
}
