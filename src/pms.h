#pragma once

#include <Arduino.h>
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
#endif

template <class T>
inline const T& __attribute__((always_inline)) min(const T& a, const T& b);

template <class T>
inline const T& __attribute__((always_inline)) min(const T& a, const T& b) { return !(b < a) ? a : b; }

#if defined max
#undef max
#endif

template <class T>
inline const T& __attribute__((always_inline)) max(const T& a, const T& b);

template <class T>
inline const T& __attribute__((always_inline)) max(const T& a, const T& b) { return !(b > a) ? a : b; }

#endif

#ifndef _countof

template <typename _CountofType, size_t _SizeOfArray>
char (* __countof_helper(_CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) (sizeof(*__countof_helper(_Array)) + 0)

#endif

////////////////////////////////////////

class Pms {
private:
	const uint8_t sig[2]{ 0x42, 0x4D };
	// TODO add getState
	// TODO jeszcze jeden stan: working
	tribool passive;
	tribool sleep;
	unsigned long timeout;
	// Transfer time of 1start + 32data + 1stop using 9600bps is 33 usec. TIMEOUT_PASSIVE could be at least 34, Value of 68 is an arbitrary doubled
	static constexpr decltype(timeout) TIMEOUT_PASSIVE = 68;
	static constexpr auto TIMEOUT_ACK = 30U; // Time to complete response after write command
	static constexpr auto BAUD_RATE = 9600; // used during begin()
	IPmsSerial* pmsSerial;

public:
	static constexpr auto WAKEUP_TIME = 2500U; // Experimentally, time to get ready after reset/wakeup

	Pms() : passive(tribool(unknown)), sleep(tribool(unknown)), timeout(TIMEOUT_PASSIVE), pmsSerial(nullptr) { };

	Pms(IPmsSerial* pmsSerial) : Pms() {
		addSerial(pmsSerial);
#if defined PMS_DYNAMIC
		begin();
#endif
	}

	~Pms() {
#if defined PMS_DYNAMIC
		end();
#endif
	}

	void addSerial(IPmsSerial* pmsSerial) { this->pmsSerial = pmsSerial; }

	bool begin(void) const {
		pmsSerial->setTimeout(TIMEOUT_PASSIVE);
		return pmsSerial->begin(BAUD_RATE);
	}

	void end(void) const { pmsSerial->end(); }

	class PmsStatus {
	public:
		using pmsStatus_t = uint8_t;
		explicit PmsStatus(pmsStatus_t value) : value(value) {}

		operator pmsStatus_t() { return value; }

		static constexpr pmsStatus_t OK = 0;
		static constexpr pmsStatus_t NO_DATA = 1 + OK;
		static constexpr pmsStatus_t READ_ERROR = 1 + NO_DATA;
		static constexpr pmsStatus_t FRAME_LENGTH_MISMATCH = 1 + READ_ERROR;
		static constexpr pmsStatus_t SUM_ERROR = 1 + FRAME_LENGTH_MISMATCH;
	private:
		static constexpr pmsStatus_t MAX = 1 + SUM_ERROR;

	public:
		const char* getErrorMsg() {
			static const char* errorMsg[]{
				"OK",
				"No data",
				"Read error",
				"Frame length mismatch",
				"CRC Error",
				"Status:unknown"
			};
			return errorMsg[min(value, static_cast<decltype(value)> _countof(errorMsg))];
		}

	private:
		pmsStatus_t value;
		static const char* errorMsg[MAX + 1];
	};

#if ! defined __INT24_MAX__
	// ReSharper disable once CppInconsistentNaming
	using __uint24 = uint32_t;
#endif

	enum class PmsCmd : __uint24 {
		CMD_READ_DATA = 0x0000e2L,
		CMD_MODE_PASSIVE = 0x0000e1L,
		CMD_MODE_ACTIVE = 0x0100e1L,
		CMD_SLEEP = 0x0000e4L,
		CMD_WAKEUP = 0x0100e4L
	};

	typedef uint16_t pmsData_t;

	class PmsData {
	public:
		typedef uint8_t pmsIdx_t;
		static constexpr pmsIdx_t DATA_SIZE = 13;
	private:
		static const float DIAMETERS[DATA_SIZE];
		static const char* const METRICS[DATA_SIZE];
	public:
		static constexpr size_t getFrameSize() { return (DATA_SIZE + 3) * sizeof(pmsData_t); } // usefull for waitForData()
		template <pmsIdx_t Size, pmsIdx_t Ofset>
		class PmsConcentrationData {
			pmsData_t data[Size];
		public:
			pmsData_t& operator[](pmsIdx_t index) { return data[index]; }
			pmsData_t getValue(pmsIdx_t index) const { return data[index]; }
			static constexpr pmsIdx_t getSize() { return Size; };

			static const char* getName(pmsIdx_t index) {
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
				return NAMES[min(static_cast<decltype(index)>(Ofset + index), static_cast<decltype(index)>(_countof(NAMES)))];
			}

			static const char* getMetric(pmsIdx_t index) {
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
				return METRICS[min(static_cast<decltype(index)>(Ofset + index), static_cast<decltype(index)>(_countof(METRICS)))];
			}

			static float getDiameter(pmsIdx_t index) {
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
				return DIAMETERS[min(static_cast<decltype(index)>(Ofset + index),
				                     static_cast<decltype(index)>(_countof(DIAMETERS)))];
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

	////////////////////////////////////////

	void __attribute__((always_inline)) swapEndianBig16(uint16_t* x) {
		constexpr union {
			// endian.test16 == 0x0001 for low endian
			// endian.test16 == 0x0100 for big endian
			// should be properly optimized by compiler
			uint16_t test16;
			uint8_t test8[2];
		} endian = {.test8 = {1, 0}};

		if (endian.test16 != 0x0100) {
			uint8_t hi = (*x & 0xff00) >> 8;
			uint8_t lo = *x & 0xff;
			*x = lo << 8 | hi;
		}
	}

	////////////////////////////////////////

	void sumBuffer(uint16_t* sum, const uint8_t* buffer, uint16_t cnt) {
		for (; cnt > 0; --cnt, ++buffer) { *sum += *buffer; }
	}

	static void sumBuffer(uint16_t* sum, const uint16_t data) { *sum += (data & 0xFF) + (data >> 8); }

	////////////////////////////////////////

	void setTimeout(decltype(timeout) timeout) {
		this->timeout = timeout;
		pmsSerial->setTimeout(timeout);
	}

	decltype(timeout) getTimeout(void) const { return timeout; }

	size_t available(void) {
		skipGarbage();
		return pmsSerial->available();
	}

	void flushInput(void) const { pmsSerial->flushInput(); }

	bool waitForData(unsigned int maxTime, size_t nData = 0) {
		const auto t0 = millis();
		if (nData == 0) {
			for (; millis() - t0 < maxTime; delay(1)) { if (pmsSerial->available()) { return true; } }
			return pmsSerial->available();
		}

		for (; millis() - t0 < maxTime; delay(1)) { if (available() >= nData) { return true; } }
		return available() >= nData;
	}

	PmsStatus read(PmsData& data) { return read((pmsData_t *)&data.raw, data.raw.getSize()); }

	PmsStatus read(pmsData_t* data, size_t nData) {
		skipGarbage();

		if (available() < (nData + 2) * sizeof(*data) + sizeof(sig)) { return PmsStatus{PmsStatus::NO_DATA}; }

		pmsSerial->read();
		// Value is equal to sig[0]. There is no need to check the value, it was checked by prior skipGarbage()

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

		if (toRead != nData * sizeof(*data)) { return PmsStatus{PmsStatus::FRAME_LENGTH_MISMATCH}; }

		if (data == nullptr) { toRead = 0; }

		if (toRead) {
			if (pmsSerial->read((uint8_t*)data, toRead) != toRead) { return PmsStatus{PmsStatus::READ_ERROR}; }
			sumBuffer(&sum, (uint8_t*)data, toRead);

			for (size_t i = 0; i < nData; ++i) { swapEndianBig16(&data[i]); }
		}

		pmsData_t crc;
		for (; toRead < thisFrameLen; toRead += 2) {
			if (pmsSerial->read((uint8_t*)&crc, sizeof crc) != sizeof crc) { return PmsStatus{PmsStatus::READ_ERROR}; }

			if (toRead < thisFrameLen - 2) { sumBuffer(&sum, crc); }
		}

		swapEndianBig16(&crc);

		if (sum != crc) { return PmsStatus{PmsStatus::SUM_ERROR}; }

		return PmsStatus{PmsStatus::OK};
	}

	bool write(PmsCmd cmd) {
		static_assert(sizeof cmd >= 3, "Wrong definition of PmsCmd (too short)");

		if (pmsSerial->write(sig, sizeof(sig)) != sizeof(sig)) { return false; }
		const size_t cmdSize = 3;
		if (pmsSerial->write((uint8_t*)&cmd, cmdSize) != cmdSize) { return false; }

		uint16_t sum{0};
		sumBuffer(&sum, sig, sizeof(sig));
		sumBuffer(&sum, (uint8_t*)&cmd, cmdSize);
		swapEndianBig16(&sum);

		if (pmsSerial->write((uint8_t*)&sum, sizeof sum) != sizeof sum) { return false; }

		if (cmd != PmsCmd::CMD_READ_DATA && cmd != PmsCmd::CMD_WAKEUP) { flushInput(); }

		switch (cmd) {
		case PmsCmd::CMD_MODE_PASSIVE:
			passive = tribool(true);
			break;
		case PmsCmd::CMD_MODE_ACTIVE:
			passive = tribool(false);
			break;
		case PmsCmd::CMD_SLEEP:
			sleep = tribool(true);
			break;
		case PmsCmd::CMD_WAKEUP:
			sleep = tribool(false);
			passive = tribool(false);
			break;
		default:
			break;
		}

		if (cmd != PmsCmd::CMD_READ_DATA && cmd != PmsCmd::CMD_WAKEUP) {
			const auto responseFrameSize = 8;
			if (!waitForData(TIMEOUT_ACK, responseFrameSize)) {
				pmsSerial->flushInput();
				return true;
			}
			pmsData_t response = 0xCCCC;
			read(&response, 1);
		}

		/*
		if ((cmd != CMD_READ_DATA) && (cmd != CMD_WAKEUP)) {
		const auto responseFrameSize = 8;
		if (!waitForData(TIMEOUT_ACK, responseFrameSize)) {
		pmsSerial->flushInput();
		return false;
		}
		Pms::pmsData_t response = 0xCCCC;
		if (read(&response, 1, 1) != PmsStatus::OK) {
		return false;
		}
		if ((response >> 8) != (cmd & 0xFF)) {
		return false;
		}
		}
		*/

		return true;
	}

private:
	void skipGarbage() { while ((pmsSerial->available()) && (pmsSerial->peek() != sig[0])) { pmsSerial->read(); } }

};
