
#include <pms.h>

////////////////////////////////////////

#if defined NOMINMAX

#if defined min
#undef min
#endif
template <class T> inline const T& __attribute__((always_inline)) min(const T& a, const T& b);
template <class T> inline const T& __attribute__((always_inline)) min(const T& a, const T& b) {
	return !(b < a) ? a : b;
}

#endif

////////////////////////////////////////

namespace {

	inline void __attribute__((always_inline)) swapEndianBig16(uint16_t *x) {
		constexpr union {
			// endian.test16 == 0x0001 for low endian
			// endian.test16 == 0x0100 for big endian
			// should be properly optimized by compiler
			uint16_t test16;
			uint8_t test8[2];
		} endian = { .test8 = { 1,0 } };

		if (endian.test16 != 0x0100) {
			uint8_t hi = (*x & 0xff00) >> 8;
			uint8_t lo = (*x & 0xff);
			*x = lo << 8 | hi;
		}
	}

	////////////////////////////////////////

	void sumBuffer(uint16_t *sum, const uint8_t *buffer, uint16_t cnt) {
		for (; cnt > 0; --cnt, ++buffer) {
			*sum += *buffer;
		}
	}

	inline void sumBuffer(uint16_t *sum, const uint16_t data) {
		*sum += (data & 0xFF) + (data >> 8);
	}

}

////////////////////////////////////////

Pms5003::Pms5003() : passive(tribool(unknown)), sleep(tribool(unknown)), pmsSerial(nullptr) {
};

Pms5003::Pms5003(IPmsSerial* pmsSerial) : Pms5003() {
	addSerial(pmsSerial);
#if defined PMS_DYNAMIC
	begin();
#endif
}

Pms5003::~Pms5003() {
#if defined PMS_DYNAMIC
	end();
#endif
}

void Pms5003::addSerial(IPmsSerial* pmsSerial) {
	this->pmsSerial = pmsSerial;
}

bool Pms5003::begin(void) {
	pmsSerial->setTimeout(Pms5003::timeoutPassive);
	return pmsSerial->begin(Pms5003::baud);
}

void Pms5003::end(void) {
	pmsSerial->end();
}

void Pms5003::setTimeout(const decltype(timeout) timeout) {
	this->timeout = timeout;
	pmsSerial->setTimeout(timeout);
}

decltype(Pms5003::timeout) Pms5003::getTimeout(void) const {
	return timeout;
}

size_t Pms5003::available(void) {
	while (pmsSerial->available()) {
		if (pmsSerial->peek() != sig[0]) {
			pmsSerial->read();
		}
		else {
			break;
		}
	}
	return pmsSerial->available();
}

Pms5003::PmsStatus Pms5003::read(pmsData *data, const size_t nData, const uint8_t dataSize) {

	if (available() < (dataSize + 2) * sizeof(pmsData) + sizeof(sig)) {
		return NO_DATA;
	}

	pmsSerial->read(); // Value is equal to sig[0]. There is no need to check the value, it was checked by prior peek()

	if (pmsSerial->read() != sig[1]) // The rest of the buffer will be invalidated during the next read attempt
		return READ_ERROR;

	uint16_t sum{ 0 };
	sumBuffer(&sum, (uint8_t *)&sig, sizeof(sig));

	pmsData thisFrameLen{ 0x1c };
	if (pmsSerial->read((uint8_t*)&thisFrameLen, sizeof(thisFrameLen)) != sizeof(thisFrameLen)) {
		return READ_ERROR;
	};

	if (thisFrameLen % 2 != 0) {
		return FRAME_LENGTH_MISMATCH;
	}
	sumBuffer(&sum, thisFrameLen);

	const decltype(thisFrameLen) maxFrameLen{ 2 * 0x1c };    // arbitrary

	swapEndianBig16(&thisFrameLen);
	if (thisFrameLen > maxFrameLen) {
		return FRAME_LENGTH_MISMATCH;
	}

	size_t toRead{ min(thisFrameLen - 2, nData * sizeof(pmsData)) };
	if (data == nullptr) {
		toRead = 0;
	}

	if (toRead) {
		if (pmsSerial->read((uint8_t*)data, toRead) != toRead) {
			return READ_ERROR;
		}
		sumBuffer(&sum, (uint8_t*)data, toRead);

		for (size_t i = 0; i < nData; ++i) {
			swapEndianBig16(&data[i]);
		}
	}

	pmsData crc;
	for (; toRead < thisFrameLen; toRead += 2) {
		if (pmsSerial->read((uint8_t*)&crc, sizeof(crc)) != sizeof(crc)) {
			return READ_ERROR;
		};

		if (toRead < thisFrameLen - 2) {
			sumBuffer(&sum, crc);
		}
	}

	swapEndianBig16(&crc);

	if (sum != crc) {
		return SUM_ERROR;
	}

	return OK;
}

void Pms5003::flushInput(void) {
	pmsSerial->flushInput();
}

bool Pms5003::waitForData(const unsigned int maxTime, const size_t nData) {
	const auto t0 = millis();
	if (nData == 0) {
		for (; (millis() - t0) < maxTime; delay(1)) {
			if (pmsSerial->available()) {
				return true;
			}
		}
		return pmsSerial->available();
	}

	for (; (millis() - t0) < maxTime; delay(1)) {
		if (available() >= nData) {
			return true;
		}
	}
	return available() >= nData;
}

bool Pms5003::write(const PmsCmd cmd) {
	static_assert(sizeof(cmd) >= 3, "Wrong definition of PmsCmd (too short)");

	if ((cmd != cmdReadData) && (cmd != cmdWakeup)) {
		flushInput();
	}

	if (pmsSerial->write(sig, sizeof(sig)) != sizeof(sig)) {
		return false;
	}
	const size_t cmdSize = 3;
	if (pmsSerial->write((uint8_t*)&cmd, cmdSize) != cmdSize) {
		return false;
	}

	uint16_t sum{ 0 };
	sumBuffer(&sum, sig, sizeof(sig));
	sumBuffer(&sum, (uint8_t*)&cmd, cmdSize);
	swapEndianBig16(&sum);
	if (pmsSerial->write((uint8_t*)&sum, sizeof(sum)) != sizeof(sum)) {
		return false;
	}

	switch (cmd) {
	case cmdModePassive:
		passive = tribool(true);
		break;
	case cmdModeActive:
		passive = tribool(false);
		break;
	case cmdSleep:
		sleep = tribool(true);
		break;
	case cmdWakeup:
		sleep = tribool(false);
		passive = tribool(false);
		// waitForData(wakeupTime);
		break;
	default:
		break;
	}
	if ((cmd != cmdReadData) && (cmd != cmdWakeup)) {
		const auto responseFrameSize = 8;
		if (!waitForData(ackTimeout, responseFrameSize)) {
			pmsSerial->flushInput();
			return true;
		}
		Pms5003::pmsData response = 0xCCCC;
		read(&response, 1, 1);
	}

	/*
		if ((cmd != cmdReadData) && (cmd != cmdWakeup)) {
			const auto responseFrameSize = 8;
			if (!waitForData(ackTimeout, responseFrameSize)) {
				pmsSerial->flushInput();
				return false;
			}
			Pms5003::pmsData response = 0xCCCC;
			if (read(&response, 1, 1) != OK) {
				return false;
			}
			if ((response >> 8) != (cmd & 0xFF)) {
				return false;
			}
		}
	*/

	return true;
}

const char *Pms5003::getMetrics(const pmsIdx idx) {
	return idx < nValues_PmsDataNames ? Pms5003::metrics[idx] : "???";
}

const char *Pms5003::getDataNames(const pmsIdx idx) {
	return idx < nValues_PmsDataNames ? Pms5003::dataNames[idx] : "???";
}

const char * Pms5003::errorMsg[N_VALUES_PMSSTATUS]{
	"OK",
	"NO_DATA",
	"READ_ERROR",
	"FRAME_LENGTH_MISMATCH",
	"SUM_ERROR"
};

const char *Pms5003::metrics[]{
	"mcg/m3",
	"mcg/m3",
	"mcg/m3",

	"mcg/m3",
	"mcg/m3",
	"mcg/m3",

	"/0.1L",
	"/0.1L",
	"/0.1L",
	"/0.1L",
	"/0.1L",
	"/0.1L",

	"???"
};

const char *Pms5003::dataNames[]{
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
