#pragma once

#include <pmsSerial.h>
#include <SoftwareSerial.h>

class PmsSoftwareSerial : public IPmsSerial {
	SoftwareSerial* serial;
	unsigned long int timeout = 0;

public:
	PmsSoftwareSerial(SoftwareSerial* iSerial) : serial(iSerial) {}

	void setTimeout(const unsigned long int iTimeout) override {
		timeout = iTimeout;
	}

	size_t available() override {
		return serial->available();
	}

	bool begin(const uint32_t baudRate) override {
		serial->begin(baudRate);
		return true;
	}

	void end() override {
		serial->end();
	}

	void flushInput() override {
		serial->flush();
	}

	uint8_t peek() override {
		return serial->peek();
	}

	uint8_t read() override {
		return serial->read();
	}

	size_t read(uint8_t *buffer, const size_t length) override {
		unsigned long int readStart = millis();
		size_t byteRead = 0;

		while (byteRead < length) {
			int datum = serial->read();

			if (datum == -1){
				if (millis() - readStart > timeout) {
					break;
				}
				delay(1);
				continue;
			}

			buffer[byteRead] = datum;
			byteRead++;
		}

		return byteRead;
	}

	size_t write(const uint8_t *buffer, const size_t size) override {
		return serial->write(buffer, size);
	}
};
