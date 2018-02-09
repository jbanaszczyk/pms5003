
#ifndef _PMS_H_
#define _PMS_H_

#include <Arduino.h>
#include <tribool.h>
#include <pmsConfig.h>

// Important: Use 3.3V logic
// Pin 1: Vcc
// Pin 2: GND

// Using SoftSerial:
//   Pin 4: Digital pin 9 (there is no choice) 
//   Pin 5: Digital pin 8 (there is no choice)

class Pms5003 {
public:
	typedef uint16_t pmsData;
	typedef uint8_t pmsIdx;
private:
	const uint8_t sig[2]{ 0x42, 0x4D };
	tribool passive;
	tribool sleep;
	unsigned long timeout;
	static const decltype(timeout) timeoutPassive = 68;  // Transfer time of 1start + 32data + 1stop using 9600bps is 33 usec. timeoutPassive could be at least 34, Value of 68 is an arbitrary doubled
	static const auto ackTimeout = 30U;                  // Time to complete response after write command

#if defined PMS_ALTSOFTSERIAL
	AltSoftSerial pmsSerial;
#endif  

public:
	enum PmsStatus : uint8_t {
		OK = 0,
		noData,
		readError,
		frameLenMismatch,
		sumError,
		nValues_PmsStatus
	};

	static const char *errorMsg[nValues_PmsStatus];

	enum PmsCmd : __uint24 {
		cmdReadData = 0x0000e2L,
		cmdModePassive = 0x0000e1L,
		cmdModeActive = 0x0100e1L,
		cmdSleep = 0x0000e4L,
		cmdWakeup = 0x0100e4L
	};

	enum PmsDataNames : pmsIdx {
		PM1dot0CF1 = 0,		 //  0
		PM2dot5CF1,			 //  1
		PM10dot0CF1,		 //  2
		PM1dot0,			 //  3
		PM2dot5,			 //  4
		PM10dot0,			 //  5
		Particles0dot3,		 //  6
		Particles0dot5,		 //  7
		Particles1dot0,		 //  8
		Particles2dot5,		 //  9
		Particles5dot0,		 // 10
		Particles10,		 // 11
		Reserved,			 // 12
		nValues_PmsDataNames // 13
	};

	static const char *dataNames[nValues_PmsDataNames];
	static const char *metrics[nValues_PmsDataNames];

	static const char *getMetrics(const pmsIdx idx);
	static const char *getDataNames(const pmsIdx idx);

	void setTimeout(const decltype(timeout) timeout);
	decltype(timeout) getTimeout(void) const;

	static const auto wakeupTime = 2500U;    // Experimentally, time to get ready after reset/wakeup

	Pms5003();
	~Pms5003();
	bool begin(void);
	void end(void);
	size_t available(void);
	void flushInput(void);
	PmsStatus read(pmsData *data, const size_t nData, const uint8_t dataSize = nValues_PmsDataNames);
	bool write(const PmsCmd cmd);
	bool waitForData(const unsigned int maxTime, const size_t nData = 0);
};

#endif
