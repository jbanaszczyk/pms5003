#ifndef _pmsserial_h_
#define _pmsserial_h_

class IPmsSerial {
public:
	virtual bool begin(const uint32_t baud);
	virtual void end();

	virtual void setTimeout(const unsigned long timeout);
	virtual size_t available();

	virtual void flushInput();
	virtual uint8_t peek();
	virtual uint8_t read();
	virtual size_t read(uint8_t *buffer, const size_t length);
	virtual size_t write(const uint8_t *buffer, const size_t size);
};

#endif
