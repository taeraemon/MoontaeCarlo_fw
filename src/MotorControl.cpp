#include "MotorControl.h"

MotorControl::MotorControl(uint8_t pwmPin, uint8_t dirPin, QEncoder* encoder, float kp, float ki, float kd, bool isReversed)
    : _pwmPin(pwmPin), _dirPin(dirPin), _encoder(encoder), _kp(kp), _ki(ki), _kd(kd), 
      _isReversed(isReversed), _targetSpeed(0), _currentSpeed(0), _previousError(0), _integral(0), _outputPWM(0) {}

void MotorControl::init() {
    // 핀 초기화
    pinMode(_pwmPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);
    analogWrite(_pwmPin, 0); // 초기 PWM 값 0
    digitalWrite(_dirPin, LOW); // 초기 방향 설정
}

void MotorControl::setTargetSpeed(float speed) {
    _targetSpeed = speed;
}

void MotorControl::update() {
    // 현재 속도 읽기
    _currentSpeed = _encoder->getSpeed();

    // PID 계산
    float error = _targetSpeed - _currentSpeed;
    _integral += error;
    _integral = constrain(_integral, -20, 20); // 적분 리미트
    float derivative = error - _previousError;

    _outputPWM = _kp * error + _ki * _integral + _kd * derivative;
    _outputPWM = constrain(abs(_outputPWM), 0, 255);

    // 모터 방향 설정
    bool direction;
    if (_isReversed) {
        direction = (error < 0); // 반전된 경우
    } else {
        direction = (error >= 0); // 정방향
    }

    // 방향 및 PWM 출력
    if (direction) {
        digitalWrite(_dirPin, HIGH); // FORWARD
    } else {
        digitalWrite(_dirPin, LOW);  // REVERSE
    }
    analogWrite(_pwmPin, _outputPWM);

    // 이전 오차 저장
    _previousError = error;
}

void MotorControl::stop() {
    _targetSpeed = 0;
    _integral = 0;
    _previousError = 0;
    analogWrite(_pwmPin, 0);
}
