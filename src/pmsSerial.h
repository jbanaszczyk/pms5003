#pragma once

class IPmsSerial {
public:
	virtual ~IPmsSerial() = default;
	virtual bool begin(uint32_t baudRate);
	virtual void end();

	virtual void setTimeout(unsigned long int timeout);
	virtual size_t available();

	virtual void flushInput();
	virtual uint8_t peek();
	virtual uint8_t read();
	virtual size_t read(uint8_t *buffer, size_t length);
	virtual size_t write(const uint8_t *buffer, size_t size);
};
