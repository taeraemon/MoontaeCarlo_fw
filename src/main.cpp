#define M1_DIR PC7
#define M1_PWM PC6
#define M2_DIR PC9
#define M2_PWM PC8
#define M3_DIR PA8
#define M3_PWM PA11
#define M4_DIR PB1
#define M4_PWM PB0

#include <Arduino.h>

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(M1_DIR, OUTPUT);
    pinMode(M1_PWM, OUTPUT);
    pinMode(M2_DIR, OUTPUT);
    pinMode(M2_PWM, OUTPUT);
    pinMode(M3_DIR, OUTPUT);
    pinMode(M3_PWM, OUTPUT);
    pinMode(M4_DIR, OUTPUT);
    pinMode(M4_PWM, OUTPUT);

    digitalWrite(M1_DIR, HIGH);
    analogWrite(M1_PWM, 0);
    digitalWrite(M2_DIR, HIGH);
    analogWrite(M2_PWM, 0);
    digitalWrite(M3_DIR, HIGH);
    analogWrite(M3_PWM, 0);
    digitalWrite(M4_DIR, HIGH);
    analogWrite(M4_PWM, 0);
}

void loop()
{
    digitalWrite(M1_DIR, HIGH);
    analogWrite(M1_PWM, 50);
    digitalWrite(M2_DIR, HIGH);
    analogWrite(M2_PWM, 50);
    digitalWrite(M3_DIR, HIGH);
    analogWrite(M3_PWM, 50);
    digitalWrite(M4_DIR, HIGH);
    analogWrite(M4_PWM, 50);
    delay(1000);

    digitalWrite(M1_DIR, HIGH);
    analogWrite(M1_PWM, 100);
    digitalWrite(M2_DIR, HIGH);
    analogWrite(M2_PWM, 100);
    digitalWrite(M3_DIR, HIGH);
    analogWrite(M3_PWM, 100);
    digitalWrite(M4_DIR, HIGH);
    analogWrite(M4_PWM, 100);
    delay(1000);

    digitalWrite(M1_DIR, LOW);
    analogWrite(M1_PWM, 50);
    digitalWrite(M2_DIR, LOW);
    analogWrite(M2_PWM, 50);
    digitalWrite(M3_DIR, LOW);
    analogWrite(M3_PWM, 50);
    digitalWrite(M4_DIR, LOW);
    analogWrite(M4_PWM, 50);
    delay(1000);


    
    digitalWrite(M1_DIR, HIGH);
    analogWrite(M1_PWM, 0);
    digitalWrite(M2_DIR, HIGH);
    analogWrite(M2_PWM, 0);
    digitalWrite(M3_DIR, HIGH);
    analogWrite(M3_PWM, 0);
    digitalWrite(M4_DIR, HIGH);
    analogWrite(M4_PWM, 0);
    delay(3000);
}
