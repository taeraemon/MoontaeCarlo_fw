/**
 * QEncoder.h
 * 
 * 원본 소스 코드 출처: https://blog.naver.com/alexpark0922/221638739710
 * 해당 링크의 코드에 위치 추적 및 에러 카운트 기능이 포함된 엔코더 클래스에 기반.
 * 
 * 수정 사항:
 * 1. 속도 계산 기능 추가: 100Hz 주기로 위치 변화를 기반으로 속도를 계산하도록 수정.
 * 2. 클래스의 모듈화 개선: updateSpeed(), getSpeed() 메서드 추가.
 * 
 * @version Modified by [taeraemon]
 */

#pragma once
#include <Arduino.h>

class QEncoder {
public:
    QEncoder(uint8_t pinA, uint8_t pinB)
        : _pinA(pinA), _pinB(pinB), _previousState(0), _count(0), _errorCount(0), 
          _previousCount(0), _lastUpdateTime(0), _speed(0) {}

    void init() {
        pinMode(_pinA, INPUT_PULLUP);
        pinMode(_pinB, INPUT_PULLUP);
        
        attachInterrupt(digitalPinToInterrupt(_pinA), [this]() { this->decode(); }, CHANGE);
        attachInterrupt(digitalPinToInterrupt(_pinB), [this]() { this->decode(); }, CHANGE);

        _previousState = readPins();
    }

    int32_t getCount() const {
        return _count;
    }

    void setCount(int32_t EncoderVal) {
        noInterrupts();
        _count = EncoderVal;
        interrupts();
    }

    int32_t getErrorCount() const {
        return _errorCount;
    }

    float getSpeed() const {
        return _speed;
    }

    void updateSpeed() {
        unsigned long currentTime = millis();
        if (currentTime - _lastUpdateTime >= 10) { // 100Hz 주기
            int32_t currentPosition = _count;
            int32_t positionChange = currentPosition - _previousCount;

            // 속도 계산 (단위: count/ms)
            _speed = static_cast<float>(positionChange) / (currentTime - _lastUpdateTime);

            // 상태 업데이트
            _previousCount = currentPosition;
            _lastUpdateTime = currentTime;
        }
    }

private:
    uint8_t _pinA;
    uint8_t _pinB;
    uint8_t _previousState;
    volatile int32_t _count;
    volatile uint32_t _errorCount;
    int32_t _previousCount;
    unsigned long _lastUpdateTime;
    float _speed;

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

// QEncoder 객체 생성
QEncoder encoder1(M1_HLA, M1_HLB);
QEncoder encoder2(M2_HLA, M2_HLB);
QEncoder encoder3(M3_HLA, M3_HLB);
QEncoder encoder4(M4_HLA, M4_HLB);
