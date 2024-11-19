#include <Arduino.h>
#include "config.h"
#include "QEncoder.h"
#include "MotorControl.h"
#include "R8FM.h"

// R8FM 객체 생성 (PPM 핀, 채널 수)
R8FM receiver(PPM_INT, PPM_CH);

// QEncoder 객체 생성
QEncoder encoder1(M1_HLA, M1_HLB);
QEncoder encoder2(M2_HLA, M2_HLB);
QEncoder encoder3(M3_HLA, M3_HLB);
QEncoder encoder4(M4_HLA, M4_HLB);

// #define Kp 15
// #define Ki 3
// #define Kd 75

#define Kp 15
#define Ki 0
#define Kd 75
// MotorControl 객체 생성
MotorControl motor1(M1_PWM, M1_DIR, &encoder1, Kp, Ki, Kd, true);
MotorControl motor2(M2_PWM, M2_DIR, &encoder2, Kp, Ki, Kd, true);
MotorControl motor3(M3_PWM, M3_DIR, &encoder3, Kp, Ki, Kd, false);
MotorControl motor4(M4_PWM, M4_DIR, &encoder4, Kp, Ki, Kd, false);

bool decodePacket(String packet);

String buffer = "";  // 시리얼로 들어오는 패킷을 버퍼에 저장
unsigned long lastConnTime = 0;  // 마지막 패킷 수신 시간을 기록

unsigned long timer_10Hz = 0;
unsigned long timer_10Hz_log = 0;
unsigned long timer_100Hz = 0;
unsigned long timer_100Hz_encoder = 0;

// 입력 값
int throttle = 0, yaw = 0, roll = 0, pitch = 0;



void setup()
{
    Serial.begin(115200);

    encoder1.init();
    encoder2.init();
    encoder3.init();
    encoder4.init();

    motor1.init();
    motor2.init();
    motor3.init();
    motor4.init();

    receiver.init();
}

void loop()
{
    // // 1.1 시리얼 명령 수신 시나리오
    // while (Serial.available()) {
    //     char receivedChar = Serial.read();  // 한 글자씩 읽기
    //     if (receivedChar == '#') {  // 패킷 종료 문자 확인
    //         if (decodePacket(buffer)) {   // 패킷 디코딩 성공 시 true 반환
    //             lastConnTime = millis();  // 마지막 패킷 수신 시간 갱신
    //             updateControl();    // 제어 입력 업데이트
    //         }
    //         buffer = "";  // 패킷 초기화
    //     }
    //     else {
    //         buffer += receivedChar;  // #이 아닌 경우 버퍼에 문자 추가
    //     }
    // }

    // // 1.2 패킷이 수신된 지 1초가 넘었으면 모터 정지
    // if (millis() - lastConnTime > 1000) {
    //     stopMotors();
    // }


    // // 2.1 조종기 연결 시나리오
    // if (millis() - timer_100Hz >= 10) {
    //     timer_10Hz = millis();
    //     lastConnTime = millis();
        
    //     update_ppm(&throttle, &yaw, &roll, &pitch);
    //     updateControl();
    // }

    // // 2.2 조종기 연결이 끊기면 모터 정지
    // if (conn = 0) {
    //     stopMotors();
    // }


    // 100Hz 주기로 각 엔코더의 속도 업데이트
    if (millis() - timer_100Hz_encoder >= 10) {
        timer_100Hz_encoder = millis();

        encoder1.updateSpeed();
        encoder2.updateSpeed();
        encoder3.updateSpeed();
        encoder4.updateSpeed();

        receiver.update();

        if (abs(receiver.getE()) < 5 && abs(receiver.getA()) < 5 && abs(receiver.getR()) < 5 || !receiver.isConnected()) {
            motor1.stop();
            motor2.stop();
            motor3.stop();
            motor4.stop();
        }
        else {
            int V1 = receiver.getE() + receiver.getA() - receiver.getR(); // Bottom-Right
            int V2 = receiver.getE() - receiver.getA() + receiver.getR(); // Bottom-Left
            int V3 = receiver.getE() + receiver.getA() + receiver.getR(); // Top-Left
            int V4 = receiver.getE() - receiver.getA() - receiver.getR(); // Top-Right

            // 목표 속도 설정
            motor1.setTargetSpeed(V1 / 5.0);
            motor2.setTargetSpeed(V2 / 5.0);
            motor3.setTargetSpeed(V3 / 5.0);
            motor4.setTargetSpeed(V4 / 5.0);

            motor1.update();
            motor2.update();
            motor3.update();
            motor4.update();
        }
    }

    if (millis() - timer_10Hz_log >= 10) {
        timer_10Hz_log = millis();

        Serial.print(motor1._targetSpeed);
        Serial.print("\t");
        Serial.print(motor1._currentSpeed);
        Serial.print("\t");
        Serial.print(motor1._outputPWM);
        Serial.print("\t");
        
        Serial.println();
    }
}



/**
 * 패킷을 디코딩하여 throttle, yaw, roll, pitch 값으로 변환합니다.
 * 데이터가 올바른 형식이면 true를 반환하고, 그렇지 않으면 false를 반환합니다.
 */
bool decodePacket(String packet) {
    int values[4];  // throttle, yaw, roll, pitch를 저장할 배열
    int index = 0;
    int lastCommaIndex = 0;

    // 각 값을 ','로 구분하여 파싱
    for (int i = 0; i < packet.length(); i++) {
        if (packet[i] == ',') {
            if (index < 4) {
                values[index++] = packet.substring(lastCommaIndex, i).toInt();
                lastCommaIndex = i + 1;
            }
            else {
                return false;  // 파싱 실패 시 false 반환
            }
        }
    }

    // 마지막 값 파싱
    if (index == 3) {
        values[index] = packet.substring(lastCommaIndex).toInt();
    }
    else {
        return false;  // 4개의 값이 아니라면 오류로 간주
    }

    // 파싱한 값 할당
    throttle = values[0];
    yaw      = values[1];
    roll     = values[2];
    pitch    = values[3];

    return true;  // 성공적으로 파싱되었으면 true 반환
}
