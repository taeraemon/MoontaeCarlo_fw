#pragma once

#include <Arduino.h>
#include "PPMReader.h"

class R8FM {
public:
    R8FM(uint8_t ppmPin, uint8_t channels);

    void init();
    void update();

    // Getter 메서드
    bool isConnected();
    int8_t getA();
    int8_t getE();
    int8_t getT();
    int8_t getR();

private:
    PPMReader _ppm;
    uint8_t _ppmPin;
    uint8_t _channels;

    // PPM 데이터
    unsigned int _raw_A;
    unsigned int _raw_E;
    unsigned int _raw_T;
    unsigned int _raw_R;
    unsigned int _raw_U1;
    unsigned int _raw_U2;
    unsigned int _raw_U3;
    unsigned int _raw_U4;

    // 파싱된 값
    int8_t _map_A;
    int8_t _map_E;
    int8_t _map_T;
    int8_t _map_R;

    bool _connected;
};
