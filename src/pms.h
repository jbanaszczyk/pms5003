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
template <class T> inline const T& __attribute__((always_inline)) min(const T& a, const T& b);
template <class T> inline const T& __attribute__((always_inline)) min(const T& a, const T& b) {
	return !(b < a) ? a : b;
}

#endif

#ifndef _countof

template <typename _CountofType, size_t _SizeOfArray>
char(*__countof_helper(_CountofType(&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) (sizeof(*__countof_helper(_Array)) + 0)

#endif

////////////////////////////////////////

class Pms5003 {
private:
	const uint8_t sig[2]{0x42, 0x4D};                        // Frame signature

	// TODO add getState
	// TODO jeszcze jeden stan: working
	tribool passive;
	tribool sleep;
	unsigned long timeout;
	static constexpr decltype(timeout) TIMEOUT_PASSIVE = 68; // Transfer time of 1start + 32data + 1stop using 9600bps is 33 usec. TIMEOUT_PASSIVE could be at least 34, Value of 68 is an arbitrary doubled
	static constexpr auto TIMEOUT_ACK = 30U;                 // Time to complete response after write command
	static constexpr auto BAUD_RATE = 9600;                  // used during begin()
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
	private:
		static constexpr pmsStatus_t MAX = 1 + SUM_ERROR;

	public:
		const char* getErrorMsg() {
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
		CMD_READ_DATA    = 0x0000e2L,
		CMD_MODE_PASSIVE = 0x0000e1L,
		CMD_MODE_ACTIVE  = 0x0100e1L,
		CMD_SLEEP        = 0x0000e4L,
		CMD_WAKEUP       = 0x0100e4L
	};

	typedef uint16_t pmsData_t;
	typedef uint8_t pmsIdx_t;

	class PmsData {
	public:
		static constexpr pmsIdx_t DATA_SIZE = 13;
// TODO Uncomment
//		typedef uint16_t pmsData_t;
//		typedef uint8_t pmsIdx_t;
	private:
		static const char * const NAMES[DATA_SIZE];
		static const float DIAMETERS[DATA_SIZE];
		static const char * const METRICS[DATA_SIZE];
	public:
		template <size_t Size, size_t ofset>
		class PmsConcentrationData {
			pmsData_t data[Size];
		public:
			pmsData_t & operator[] (pmsIdx_t index) {
				return data[index];
			}
			pmsData_t getValue(pmsIdx_t index) const {
				return data[index];
			}
			static constexpr pmsIdx_t getSize() { return Size; };
			const char * getName(pmsIdx_t index) const { return PmsData::NAMES[ofset + index]; }
			float getDiameter(pmsIdx_t index) const { return PmsData::DIAMETERS[ofset + index]; }
			const char * getMetrics(pmsIdx_t index) const { return PmsData::METRICS[ofset + index]; }
		};

		union {
			PmsConcentrationData<13, 0> raw;
			struct {
				PmsConcentrationData<3, 0> concentrationCf;
				PmsConcentrationData<3, 3> concentration;
				PmsConcentrationData<6, 6> particles;
				PmsConcentrationData<1, 12> reserved;
			};
		} data;

		static_assert(sizeof(data) == DATA_SIZE * sizeof(pmsData_t), "Unexpected union 'data' size");
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

	static constexpr auto WAKEUP_TIME = 2500U;    // Experimentally, time to get ready after reset/wakeup

	bool begin(void);
	void end(void);
	size_t available(void);
	void flushInput(void);
	PmsStatus read(PmsData& data);
	PmsStatus read(pmsData_t *data, size_t nData, uint8_t dataSize);
	bool write(PmsCmd cmd);
	bool waitForData(unsigned int maxTime, size_t nData = 0);
private:
	void skipGarbage();
};
