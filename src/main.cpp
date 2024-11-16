#include <Arduino.h>
#include "config.h"
#include "PPMReader.h"
#include "QEncoder.h"
#include "MotorControl.h"

bool decodePacket(String packet);

String buffer = "";  // 시리얼로 들어오는 패킷을 버퍼에 저장
unsigned long lastConnTime = 0;  // 마지막 패킷 수신 시간을 기록

unsigned long timer_10Hz = 0;
unsigned long timer_10Hz_log = 0;
unsigned long timer_100Hz = 0;
unsigned long timer_100Hz_encoder = 0;



void setup()
{
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(M1_DIR, OUTPUT);    pinMode(M1_PWM, OUTPUT);
    pinMode(M2_DIR, OUTPUT);    pinMode(M2_PWM, OUTPUT);
    pinMode(M3_DIR, OUTPUT);    pinMode(M3_PWM, OUTPUT);
    pinMode(M4_DIR, OUTPUT);    pinMode(M4_PWM, OUTPUT);

    digitalWrite(M1_DIR, HIGH); analogWrite(M1_PWM, 0);
    digitalWrite(M2_DIR, HIGH); analogWrite(M2_PWM, 0);
    digitalWrite(M3_DIR, HIGH); analogWrite(M3_PWM, 0);
    digitalWrite(M4_DIR, HIGH); analogWrite(M4_PWM, 0);

    pinMode(M1_HLA, INPUT_PULLUP); pinMode(M1_HLB, INPUT_PULLUP);
    pinMode(M2_HLA, INPUT_PULLUP); pinMode(M2_HLB, INPUT_PULLUP);
    pinMode(M3_HLA, INPUT_PULLUP); pinMode(M3_HLB, INPUT_PULLUP);
    pinMode(M4_HLA, INPUT_PULLUP); pinMode(M4_HLB, INPUT_PULLUP);

    encoder1.init(); encoder1.setCount(0);
    encoder2.init(); encoder2.setCount(0);
    encoder3.init(); encoder3.setCount(0);
    encoder4.init(); encoder4.setCount(0);
}

void loop()
{
    // 1.1 시리얼 명령 수신 시나리오
    while (Serial.available()) {
        char receivedChar = Serial.read();  // 한 글자씩 읽기
        if (receivedChar == '#') {  // 패킷 종료 문자 확인
            if (decodePacket(buffer)) {   // 패킷 디코딩 성공 시 true 반환
                lastConnTime = millis();  // 마지막 패킷 수신 시간 갱신
                updateControl();    // 제어 입력 업데이트
            }
            buffer = "";  // 패킷 초기화
        }
        else {
            buffer += receivedChar;  // #이 아닌 경우 버퍼에 문자 추가
        }
    }

    // // 1.2 패킷이 수신된 지 1초가 넘었으면 모터 정지
    // if (millis() - lastConnTime > 1000) {
    //     stopMotors();
    // }


    // // 2.1 조종기 연결 시나리오
    // if (millis() - timer_100Hz >= 10) {
    //     timer_10Hz = millis();
    //     lastConnTime = millis();
        
    //     update_ppm();
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

        updateMotor();

        // 속도 출력
        // Serial.print("Speed 1: "); Serial.print(encoder1.getSpeed()); Serial.print(" count/ms\t");
        // Serial.print("Speed 2: "); Serial.print(encoder2.getSpeed()); Serial.print(" count/ms\t");
        // Serial.print("Speed 3: "); Serial.print(encoder3.getSpeed()); Serial.print(" count/ms\t");
        // Serial.print("Speed 4: "); Serial.print(encoder4.getSpeed()); Serial.print(" count/ms\t");
        // Serial.println();
    }

    if (millis() - timer_10Hz_log >= 100) {
        timer_10Hz_log = millis();


        Serial.print(encoder1.getSpeed());
        Serial.print("\t");
        Serial.print(motor1.targetSpeed);
        Serial.print("\t");
        Serial.print(motor1.currentSpeed);
        Serial.print("\t");
        Serial.print(motor1.outputPWM);
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
