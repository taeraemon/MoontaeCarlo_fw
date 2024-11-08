#include <Arduino.h>
#include "config.h"

// PID 제어 변수
struct MotorControl {
    float targetSpeed;
    float currentSpeed;
    int outputPWM;
    float kp, ki, kd;
    float previousError, integral;
};

int throttle = 0, yaw = 0, roll = 0, pitch = 0;  // 제어 입력 변수

MotorControl motor1 = {0, 0, 0, 25, 0.0, 2};
MotorControl motor2 = {0, 0, 0, 25, 0.0, 2};
MotorControl motor3 = {0, 0, 0, 25, 0.0, 2};
MotorControl motor4 = {0, 0, 0, 25, 0.0, 2};

void updateControl();
void stopMotors();
void controlMecanumWheels(int throttle, int rudder, int elevator, int aileron);
void updateMotor();

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

    motor1.targetSpeed = 0;
    motor2.targetSpeed = 0;
    motor3.targetSpeed = 0;
    motor4.targetSpeed = 0;
}

/**
 * controlMecanumWheels
 * 
 * 이 함수는 메카넘 휠 로봇의 4개의 모터를 rudder, elevator, aileron
 * 입력 값에 따라 제어합니다. 각 입력 값은 -127에서 127 사이의 정수로
 * 가정하며, PWM 범위인 0~255로 매핑하여 모터에 전달합니다.
 *
 * @param throttle 사용하지 않음
 * @param rudder 제자리 회전을 제어하는 값 (-127 ~ 127, 시계/반시계 방향)
 * @param elevator 전후 방향 평행 이동을 위한 속도 제어 값 (-127 ~ 127)
 * @param aileron 좌우 방향 평행 이동을 위한 속도 제어 값 (-127 ~ 127)
 */
void controlMecanumWheels(int throttle, int rudder, int elevator, int aileron) {
    // 각 모터 속도 계산
    // V1: Bottom-Right, V2: Bottom-Left, V3: Top-Left, V4: Top-Right
    int V1 = elevator + aileron - rudder; // Bottom-Right
    int V2 = elevator - aileron + rudder; // Bottom-Left
    int V3 = elevator + aileron + rudder; // Top-Left
    int V4 = elevator - aileron - rudder; // Top-Right

    // 속도 정규화 과정 (PWM의 0~255 범위에 맞춰 조정)
    // 각 모터 속도는 절대값이 127을 넘지 않도록 조절되었으므로 추가적인 정규화가 불필요.
    // analogWrite는 0~255의 값을 사용하므로, 여기서는 abs()로 절대값을 취한 후 2배 확장.
    // (-127 ~ 127의 값을 0~255로 맞추기 위해 abs(Vx) * 2 사용)
    
    // // 각 모터에 속도 전달 (PWM 0~255 범위에 매핑)
    // analogWrite(M1_PWM, abs(V1) * 2);          // V1 모터 속도 전달
    // digitalWrite(M1_DIR, V1 >= 0 ? LOW : HIGH); // 방향 설정 (V1 >= 0일 때 정방향)

    // analogWrite(M2_PWM, abs(V2) * 2);          // V2 모터 속도 전달
    // digitalWrite(M2_DIR, V2 >= 0 ? LOW : HIGH); // 방향 설정 (V2 >= 0일 때 정방향)

    // analogWrite(M3_PWM, abs(V3) * 2);          // V3 모터 속도 전달
    // digitalWrite(M3_DIR, V3 >= 0 ? HIGH : LOW); // 방향 설정 (V3 >= 0일 때 정방향)

    // analogWrite(M4_PWM, abs(V4) * 2);          // V4 모터 속도 전달
    // digitalWrite(M4_DIR, V4 >= 0 ? HIGH : LOW); // 방향 설정 (V4 >= 0일 때 정방향)

    motor1.targetSpeed = V1;
    motor2.targetSpeed = V2;
    motor3.targetSpeed = V3;
    motor4.targetSpeed = V4;
}

void updateMotor() {
    float error;
    float derivative;
    
    motor1.currentSpeed = encoder1.getSpeed();
    error = motor1.targetSpeed - motor1.currentSpeed;
    motor1.integral += error;
    derivative = error - motor1.previousError;
    motor1.outputPWM = motor1.kp * error + motor1.ki * motor1.integral + motor1.kd * derivative;
    motor1.outputPWM = constrain(abs(motor1.outputPWM), 0, 255);
    digitalWrite(M1_DIR, error >= 0 ? LOW : HIGH);
    analogWrite(M1_PWM, motor1.outputPWM);
    motor1.previousError = error;

    motor2.currentSpeed = encoder2.getSpeed();
    error = motor2.targetSpeed - motor2.currentSpeed;
    motor2.integral += error;
    derivative = error - motor2.previousError;
    motor2.outputPWM = motor2.kp * error + motor2.ki * motor2.integral + motor2.kd * derivative;
    motor2.outputPWM = constrain(abs(motor2.outputPWM), 0, 255);
    digitalWrite(M2_DIR, error >= 0 ? LOW : HIGH);
    analogWrite(M2_PWM, motor2.outputPWM);
    motor2.previousError = error;

    motor3.currentSpeed = encoder3.getSpeed();
    error = motor3.targetSpeed - motor3.currentSpeed;
    motor3.integral += error;
    derivative = error - motor3.previousError;
    motor3.outputPWM = motor3.kp * error + motor3.ki * motor3.integral + motor3.kd * derivative;
    motor3.outputPWM = constrain(abs(motor3.outputPWM), 0, 255);
    digitalWrite(M3_DIR, error >= 0 ? HIGH : LOW);
    analogWrite(M3_PWM, motor3.outputPWM);
    motor3.previousError = error;

    motor4.currentSpeed = encoder4.getSpeed();
    error = motor4.targetSpeed - motor4.currentSpeed;
    motor4.integral += error;
    derivative = error - motor4.previousError;
    motor4.outputPWM = motor4.kp * error + motor4.ki * motor4.integral + motor4.kd * derivative;
    motor4.outputPWM = constrain(abs(motor4.outputPWM), 0, 255);
    digitalWrite(M4_DIR, error >= 0 ? HIGH : LOW);
    analogWrite(M4_PWM, motor4.outputPWM);
    motor4.previousError = error;
}
