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

class Pms5003 {
public:
	typedef uint16_t pmsData_t;
	typedef uint8_t pmsIdx_t;
private:
	const uint8_t sig[2]{0x42, 0x4D};                    // Frame signature
	tribool passive;
	tribool sleep;
	unsigned long timeout;
	static const decltype(timeout) TIMEOUT_PASSIVE = 68; // Transfer time of 1start + 32data + 1stop using 9600bps is 33 usec. TIMEOUT_PASSIVE could be at least 34, Value of 68 is an arbitrary doubled
	static const auto TIMEOUT_ACK = 30U;                 // Time to complete response after write command
	static const auto BAUD_RATE = 9600;                  // used during begin()
	IPmsSerial* pmsSerial;

public:
	Pms5003();
	Pms5003(IPmsSerial* pmsSerial);
	~Pms5003();
	void addSerial(IPmsSerial* pmsSerial);

	class PmsStatus {
	public:
		using pmsStatus_t = uint8_t;
		explicit PmsStatus(pmsStatus_t value) : value(value) {}
		operator pmsStatus_t() {
			return value;
		}

		static constexpr pmsStatus_t OK = 0;
		static constexpr pmsStatus_t NO_DATA = 1 + OK;
		static constexpr pmsStatus_t READ_ERROR = 1 + NO_DATA;
		static constexpr pmsStatus_t FRAME_LENGTH_MISMATCH = 1 + READ_ERROR;
		static constexpr pmsStatus_t SUM_ERROR = 1 + FRAME_LENGTH_MISMATCH;

		const char* getErrorMsg() {
			return errorMsg[min(value, static_cast<decltype(value)> (sizeof(errorMsg) / sizeof(errorMsg[0])))];
		}

	private:
		pmsStatus_t value;
		static const char* errorMsg[SUM_ERROR + 2];
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

	enum PmsDataNames : pmsIdx_t {
		PM_1_DOT_0_CF1 = 0,   //  0
		PM_2_DOT_5_CF1,       //  1
		PM_10_CF1,            //  2
		PM_1_DOT_0,           //  3
		PM_2_DOT_5,           //  4
		PM_10_DOT_0,          //  5
		PARTICLES_0_DOT_3,    //  6
		PARTICLES_0_DOT_5,    //  7
		PARTICLES_1_DOT_0,    //  8
		PARTICLES_2_DOT_5,    //  9
		PARTICLES_5_DOT_0,    // 10
		PARTICLES_10,         // 11
		RESERVED,             // 12
		N_VALUES_PMSDATANAMES // 13
	};

	static const char *dataNames[N_VALUES_PMSDATANAMES];
	static const char *metrics[N_VALUES_PMSDATANAMES];

	static const char *getMetrics(pmsIdx_t idx);
	static const char *getDataNames(pmsIdx_t idx);

	void setTimeout(decltype(timeout) timeout);
	decltype(timeout) getTimeout(void) const;

	static const auto WAKEUP_TIME = 2500U;    // Experimentally, time to get ready after reset/wakeup

	bool begin(void);
	void end(void);
	size_t available(void);
	void flushInput(void);
	PmsStatus read(pmsData_t *data, size_t nData, uint8_t dataSize = N_VALUES_PMSDATANAMES);
	bool write(PmsCmd cmd);
	bool waitForData(unsigned int maxTime, size_t nData = 0);
};
