#pragma once

#include <pmsSerial.h>
#include <AltSoftSerial.h>

class PmsAltSerial : public IPmsSerial {
	AltSoftSerial serial;
public:
	void setTimeout(const unsigned long int timeout) override {
		serial.setTimeout(timeout);
	}

	size_t available() override {
		return serial.available();
	}

	bool begin(const uint32_t baudRate) override {
		return serial.begin(baudRate);
	}

	void end() override {
		serial.end();
	}

	void flushInput() override {
		serial.flushInput();
	}

	uint8_t peek() override {
		return serial.peek();
	}

	uint8_t read() override {
		return serial.read();
	}

	size_t read(uint8_t *buffer, const size_t length) override {
		return serial.readBytes(buffer, length);
	}

	size_t write(const uint8_t *buffer, const size_t size) override {
		return serial.write(buffer, size);
	}
};
