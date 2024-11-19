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

#define WINDOW_SIZE 10 // 슬라이딩 윈도우 크기 (100ms / 10ms)

class QEncoder {
public:
    QEncoder(uint8_t pinA, uint8_t pinB);

    void init();
    int32_t getCount() const;
    void setCount(int32_t EncoderVal);
    int32_t getErrorCount() const;
    float getSpeed() const;
    void updateSpeed();

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
    static const int _windowSize = WINDOW_SIZE;
    float _speedBuffer[_windowSize] = {0};
    int _bufferIndex;

    uint8_t readPins();
    void decode();
};
