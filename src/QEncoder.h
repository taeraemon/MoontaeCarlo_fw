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

#define WINDOW_SIZE 10 // 슬라이딩 윈도우 크기 (100ms / 10ms)

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

            // 현재 측정 속도 계산 (단위: count/10ms)
            float currentSpeed = static_cast<float>(positionChange);

            // 슬라이딩 윈도우 업데이트
            _speedBuffer[_bufferIndex] = currentSpeed; // 최신 값을 윈도우에 추가
            _bufferIndex = (_bufferIndex + 1) % _windowSize; // 순환 버퍼 인덱스 계산

            // 가중 평균 계산
            float weightedSum = 0;
            float weightTotal = 0;
            for (int i = 0; i < _windowSize; i++) {
                int weight = i + 1; // 가중치: 오래된 데이터보다 최신 데이터가 더 중요
                int index = (_bufferIndex + i) % _windowSize; // 순환 인덱스 계산
                weightedSum += weight * _speedBuffer[index];
                weightTotal += weight;
            }
            float weightedSpeed = weightedSum / weightTotal;

            // 저역 필터(LPF) 적용
            const float alpha = 0.5; // LPF 계수 (0.0 ~ 1.0)
            _speed = alpha * weightedSpeed + (1 - alpha) * _speed; // 필터링된 속도

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

    // 슬라이딩 윈도우 관련 변수
    static const int _windowSize = 10; // 100ms 슬라이딩 윈도우 (10ms × 10)
    float _speedBuffer[_windowSize] = {0}; // 속도 데이터를 저장하는 버퍼
    int _bufferIndex = 0; // 슬라이딩 윈도우의 현재 인덱스

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
