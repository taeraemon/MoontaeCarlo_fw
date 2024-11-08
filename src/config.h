#include <Arduino.h>

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
#define M2_HLA PB4  // OLD : PB7 // Cause EXTi duplicate issue
#define M2_HLB PA10 // OLD : PB6 // Cause EXTi duplicate issue
#define M3_HLA PA1  // OLD : PA0 // Cause Direction issue
#define M3_HLB PA0  // OLD : PA1 // Cause Direction issue
#define M4_HLA PA7
#define M4_HLB PA6

#define PPM_INT PB5
#define PPM_CH 8



//    ↑A-----B↑
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑

//    ↓A-----B↓
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓

//    =A-----B↑
//     |   ↖ |
//     | ↖   |
//    ↑C-----D=

//    ↓A-----B↑
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓

//    ↓A-----B=
//     | ↙   |
//     |   ↙ |
//    =C-----D↓

//    ↑A-----B=
//     | ↗   |
//     |   ↗ |
//    =C-----D↑

//    ↑A-----B↓
//     |  →  |
//     |  →  |
//    ↓C-----D↑

//    =A-----B↓
//     |   ↘ |
//     | ↘   |
//    ↓C-----D=

//    ↑A-----B↓
//     | ↗ ↘ |
//     | ↖ ↙ |
//    ↑C-----D↓

//    ↓A-----B↑
//     | ↙ ↖ |
//     | ↘ ↗ |
//    ↓C-----D↑

//    =A-----B=
//     |  =  |
//     |  =  |
//    =C-----D=