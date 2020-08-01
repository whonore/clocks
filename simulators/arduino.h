#ifndef ARDUINO_H
#define ARDUINO_H 1

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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
#define NPINS 20

// Serial
#define SERIAL_OUT 256
struct Serial_t {
    bool Serial;
    void (*print)(const char *str);
    void (*begin)(unsigned int baud);
    char _out[SERIAL_OUT];
} Serial;

// Library
uint32_t micros();
uint32_t millis();
void delay(uint32_t ms);
void pinMode(uint8_t pin, uint8_t mode);
uint8_t digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, uint8_t val);

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

// Initialization
void setup();
void loop();

#endif // ARDUINO_H
