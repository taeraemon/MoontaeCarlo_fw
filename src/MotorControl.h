#pragma once

#include <Arduino.h>
#include "QEncoder.h"

class MotorControl {
public:
    MotorControl(uint8_t pwmPin, uint8_t dirPin, QEncoder* encoder, float kp, float ki, float kd, bool isReversed = false);

    void init(); // 핀 초기화 메서드
    void setTargetSpeed(float speed);
    void update();
    void stop();
    float _targetSpeed;
    float _currentSpeed;
    int _outputPWM;

private:
    uint8_t _pwmPin;
    uint8_t _dirPin;
    QEncoder* _encoder;
    bool _isReversed; // 방향 반전 플래그


    float _kp, _ki, _kd;
    float _previousError, _integral;

};
