// QEncoder.h
#pragma once
#include <Arduino.h>

class QEncoder {
public:
    QEncoder(uint8_t pinA, uint8_t pinB)
        : _pinA(pinA), _pinB(pinB), _previousState(0), _count(0), _errorCount(0) {}

    void init() {
        pinMode(_pinA, INPUT_PULLUP);
        pinMode(_pinB, INPUT_PULLUP);
        
        attachInterrupt(digitalPinToInterrupt(_pinA), [this]() { this->decode(); }, CHANGE);
        attachInterrupt(digitalPinToInterrupt(_pinB), [this]() { this->decode(); }, CHANGE);

        _previousState = readPins();
    }

    int32_t getCount() {
        return _count;
    }

    void setCount(int32_t EncoderVal) {
        noInterrupts();
        _count = EncoderVal;
        interrupts();
    }

    int32_t getErrorCount() {
        return _errorCount;
    }

private:
    uint8_t _pinA;
    uint8_t _pinB;
    uint8_t _previousState;
    volatile int32_t _count;
    volatile uint32_t _errorCount;

    uint8_t readPins() {
        return (digitalRead(_pinA) << 1) | digitalRead(_pinB);
    }

    void decode() {
        uint8_t newState = readPins();
        switch((_previousState << 2) | newState) {
            case 0b0001: // 0x01 PREV|CURR
            case 0b0111: // 0x07
            case 0b1110: // 0x0E
            case 0b1000: // 0x08
                _count--;
                break;
            case 0b0010: // 0x02
            case 0b1011: // 0x0B
            case 0b1101: // 0x0D
            case 0b0100: // 0x04
                _count++;
                break;
            default:
                _errorCount++;
                break;
        }
        _previousState = newState;
    }
};
