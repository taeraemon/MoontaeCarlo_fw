#define M1_DIR PC7
#define M1_PWM PC6
#define M2_DIR PC9
#define M2_PWM PC8
#define M3_DIR PA8
#define M3_PWM PA11
#define M4_DIR PB1
#define M4_PWM PB0

#define M1_HLA PA15
#define M1_HLB PB3
#define M2_HLA PB7
#define M2_HLB PB6
#define M3_HLA PA0
#define M3_HLB PA1
#define M4_HLA PA7
#define M4_HLB PA6

#define PPM_INT PB5
#define PPM_CH 8

#include <Arduino.h>
#include "PPMReader.h"

bool decodePacket(String packet);
void updateControl();
void stopMotors();
void controlMecanumWheels(int throttle, int yaw, int roll, int pitch);
void update_ppm();



int throttle = 0, yaw = 0, roll = 0, pitch = 0;  // 제어 입력 변수
String buffer = "";  // 시리얼로 들어오는 패킷을 버퍼에 저장
unsigned long lastConnTime = 0;  // 마지막 패킷 수신 시간을 기록

PPMReader ppm(PPM_INT, PPM_CH);
unsigned A=0, E=0, T=0, R=0, U1=0, U2=0, U3=0, U4=0, conn=0, armed=0, mode=0;

unsigned long timer_10Hz = 0;
unsigned long timer_100Hz = 0;




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
}

void loop()
{
    // 1. 시리얼 명령 수신 시나리오
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

    // 1.2 조종기 연결 시나리오
    if (millis() - timer_100Hz >= 10) {
        timer_10Hz = millis();
        lastConnTime = millis();
        
        update_ppm();
        updateControl();
    }

    // 2. 패킷이 수신된 지 1초가 넘었으면 모터 정지
    if (millis() - lastConnTime > 1000) {
        stopMotors();
    }
}





void update_ppm() {
    A = ppm.latestValidChannelValue(1, 0);
    E = ppm.latestValidChannelValue(2, 0);
    T = ppm.latestValidChannelValue(3, 0);
    R = ppm.latestValidChannelValue(4, 0);
    U1 = ppm.latestValidChannelValue(5, 0);
    U2 = ppm.latestValidChannelValue(6, 0);
    U3 = ppm.latestValidChannelValue(7, 0);
    U4 = ppm.latestValidChannelValue(8, 0);
    if (T == 0) {
        conn = 0;
    } else {
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

    // if (conn == 1) {
    //     r_tar = map(A, 1000, 2000, -20, 20);
    //     p_tar = map(E, 1000, 2000, -20, 20);
    //     y_tar = map(R, 1000, 2000, -20, 20);
    //     mode = map(U1, 1000, 2000, 3, 1);
    //     if (U3 < 1500) {
    //         armed = 1;
    //     } else {
    //         armed = 0;
    //     }
    // } else {
    //     r_tar = 0;
    //     p_tar = 0;
    //     y_tar = y_cur;
    //     mode = 0;
    //     armed = 0;
    // }
    
    // 파싱한 값 할당
    throttle = map(T, 1000, 2000, -127, 127);
    yaw      = map(R, 1000, 2000, -127, 127);
    roll     = map(E, 1000, 2000, -127, 127);
    pitch    = map(A, 1000, 2000, -127, 127);
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
    yaw = values[1];
    roll = values[2];
    pitch = values[3];

    return true;  // 성공적으로 파싱되었으면 true 반환
}

/**
 * 제어 입력에 따라 모터 속도와 방향을 업데이트합니다.
 */
void updateControl() {
    controlMecanumWheels(throttle, yaw, roll, pitch);
}

/**
 * 1초 이상 패킷이 없을 시 호출하여 모터를 정지합니다.
 */
void stopMotors() {
    analogWrite(M1_PWM, 0); analogWrite(M2_PWM, 0);
    analogWrite(M3_PWM, 0); analogWrite(M4_PWM, 0);
}

/**
 * controlMecanumWheels
 * 
 * 이 함수는 메카넘 휠 로봇의 4개의 모터를 throttle, yaw, roll, pitch 
 * 입력 값에 따라 제어합니다. 각 입력 값은 -127에서 127 사이의 정수로
 * 가정하며, PWM 범위인 0~255로 매핑하여 모터에 전달합니다.
 *
 * @param throttle 전후 방향 이동을 위한 속도 제어 값 (-127 ~ 127)
 * @param yaw 로봇의 회전을 제어하는 값 (-127 ~ 127, 시계/반시계방향)
 * @param roll 좌우 방향 이동을 위한 속도 제어 값 (-127 ~ 127)
 * @param pitch 상하 방향 이동을 위한 속도 제어 값 (-127 ~ 127)
 */
void controlMecanumWheels(int throttle, int yaw, int roll, int pitch) {
    // 각 모터 속도 계산
    // V1: Front-Left, V2: Front-Right, V3: Back-Left, V4: Back-Right
    // 입력 값의 조합을 통해 각 모터의 최종 속도를 결정
    int V1 = throttle + yaw + roll + pitch;
    int V2 = throttle - yaw - roll + pitch;
    int V3 = throttle + yaw - roll - pitch;
    int V4 = throttle - yaw + roll - pitch;

    // 속도 정규화 과정 (PWM의 0~255 범위에 맞춰 조정)
    // 각 모터 속도는 절대값이 127을 넘지 않도록 조절되었으므로 추가적인 정규화가 불필요.
    // analogWrite는 0~255의 값을 사용하므로, 여기서는 abs()로 절대값을 취한 후 2배 확장.
    // (-127 ~ 127의 값을 0~255로 맞추기 위해 abs(Vx) * 2 사용)
    
    // 각 모터에 속도 전달 (PWM 0~255 범위에 매핑)
    analogWrite(M1_PWM, abs(V1) * 2);          // V1 모터 속도 전달
    digitalWrite(M1_DIR, V1 >= 0 ? LOW : HIGH); // 방향 설정 (V1 >= 0일 때 정방향)

    analogWrite(M2_PWM, abs(V2) * 2);          // V2 모터 속도 전달
    digitalWrite(M2_DIR, V2 >= 0 ? LOW : HIGH); // 방향 설정 (V2 >= 0일 때 정방향)

    analogWrite(M3_PWM, abs(V3) * 2);          // V3 모터 속도 전달
    digitalWrite(M3_DIR, V3 >= 0 ? HIGH : LOW); // 방향 설정 (V3 >= 0일 때 정방향)

    analogWrite(M4_PWM, abs(V4) * 2);          // V4 모터 속도 전달
    digitalWrite(M4_DIR, V4 >= 0 ? HIGH : LOW); // 방향 설정 (V4 >= 0일 때 정방향)
}
