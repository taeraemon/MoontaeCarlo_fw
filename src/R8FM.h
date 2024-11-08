#include <Arduino.h>
#include "PPMReader.h"
#include "config.h"
#include "MotorControl.h"

PPMReader ppm(PPM_INT, PPM_CH);
unsigned A = 0, E = 0, T = 0, R = 0, U1 = 0, U2 = 0, U3 = 0, U4 = 0;
unsigned conn=0;

void update_ppm() {
    A  = ppm.latestValidChannelValue(1, 0);
    E  = ppm.latestValidChannelValue(2, 0);
    T  = ppm.latestValidChannelValue(3, 0);
    R  = ppm.latestValidChannelValue(4, 0);
    U1 = ppm.latestValidChannelValue(5, 0);
    U2 = ppm.latestValidChannelValue(6, 0);
    U3 = ppm.latestValidChannelValue(7, 0);
    U4 = ppm.latestValidChannelValue(8, 0);

    if (T == 0) {
        conn = 0;
    }
    else {
        conn = 1;
    }

    Serial.print(T);
    Serial.print("\t");
    Serial.print(R);
    Serial.print("\t");
    Serial.print(E);
    Serial.print("\t");
    Serial.print(A);
    Serial.print("\t");
    Serial.println();

    // 파싱한 값 할당
    throttle = map(T, 1000, 2000, -127, 127);
    yaw      = map(R, 1000, 2000, -127, 127);
    roll     = map(E, 1000, 2000, -127, 127);
    pitch    = map(A, 1000, 2000, -127, 127);
}
