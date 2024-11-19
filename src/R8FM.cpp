#include "R8FM.h"

R8FM::R8FM(uint8_t ppmPin, uint8_t channels)
    : _ppmPin(ppmPin), _channels(channels), _ppm(ppmPin, channels), 
      _raw_A(0), _raw_E(0), _raw_T(0), _raw_R(0),
      _map_A(0), _map_E(0), _map_T(0), _map_R(0),
      _connected(false) {}

void R8FM::init() {
    // PPMReader 초기화는 생성자에서 이미 수행됨
    pinMode(_ppmPin, INPUT);
}

void R8FM::update() {
    // PPM 값 읽기
    _raw_A = _ppm.latestValidChannelValue(1, 0);
    _raw_E = _ppm.latestValidChannelValue(2, 0);
    _raw_T = _ppm.latestValidChannelValue(3, 0);
    _raw_R = _ppm.latestValidChannelValue(4, 0);

    // 연결 상태 확인
    if (_raw_T == 0) {
        _connected = false;
    }
    else {
        _connected = true;
    }

    // 값 매핑
    _map_A = map(_raw_A, 1000, 2000, -127, 127);
    _map_E = map(_raw_E, 1000, 2000, -127, 127);
    _map_T = map(_raw_R, 1000, 2000, -127, 127);
    _map_R = map(_raw_R, 1000, 2000, -127, 127);
}

bool R8FM::isConnected() {
    return _connected;
}

int8_t R8FM::getA() {
    return _map_A;
}

int8_t R8FM::getE() {
    return _map_E;
}

int8_t R8FM::getT() {
    return _map_T;
}

int8_t R8FM::getR() {
    return _map_R;
}
