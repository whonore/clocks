#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef DEBUG_PINS
#define DEBUG_PINS 0
#endif

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

typedef uint8_t byte;

#define LOW 0
#define HIGH 1

#define INPUT 0
#define OUTPUT 1

#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define A4 18
#define A5 19
#define SDA 18
#define SCL 19
#define NPINS 20

// Serial
#define SERIAL_OUT 256
class Serial_ {
    private:
        bool init = false;
    public:
        char _out[SERIAL_OUT];
        Serial_() { memset(this->_out, '\0', SERIAL_OUT); }
        void begin(unsigned int baud);
        void print(const char *str);
        operator bool();
};
extern Serial_ Serial;

// Library
uint32_t micros();
uint32_t millis();
void delay(uint32_t ms);
void pinMode(uint8_t pin, uint8_t mode);
uint8_t digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, uint8_t val);

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))

// Program Memory
#define PROGMEM

void *memcpy_P(void *dest, const void *src, size_t n);

// Initialization
void setup();
void loop();

#endif // ARDUINO_H
