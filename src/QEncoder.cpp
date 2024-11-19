#include <Arduino.h>
#include "QEncoder.h"

QEncoder::QEncoder(uint8_t pinA, uint8_t pinB)
    : _pinA(pinA), _pinB(pinB), _previousState(0), _count(0), _errorCount(0), 
      _previousCount(0), _lastUpdateTime(0), _speed(0), _bufferIndex(0) {}

void QEncoder::init() {
    pinMode(_pinA, INPUT_PULLUP);
    pinMode(_pinB, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(_pinA), [this]() { this->decode(); }, CHANGE);
    attachInterrupt(digitalPinToInterrupt(_pinB), [this]() { this->decode(); }, CHANGE);

    _count = 0;

    _previousState = readPins();
}

int32_t QEncoder::getCount() const {
    return _count;
}

void QEncoder::setCount(int32_t EncoderVal) {
    noInterrupts();
    _count = EncoderVal;
    interrupts();
}

int32_t QEncoder::getErrorCount() const {
    return _errorCount;
}

float QEncoder::getSpeed() const {
    return _speed;
}

void QEncoder::updateSpeed() {
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

uint8_t QEncoder::readPins() {
    return (digitalRead(_pinA) << 1) | digitalRead(_pinB);
}

void QEncoder::decode() {
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
