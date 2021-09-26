#ifdef __cplusplus
extern "C" {
#endif

#ifndef _pid_h_
#define _pid_h_
#include <avr/io.h>
#include <limits.h>

typedef struct PID_CONTROLLER{
    float kpFactor;
    float kiFactor;
    float kdFactor;
    float lastValue;
    float error;
    float lastError;
    float preLastError;
} pid_controller_t;

void pid_init(float kp, float ki, float kd, pid_controller_t* controller);
float pid_compute(float sollWert, float istWert, pid_controller_t* controller);

#endif

#ifdef __cplusplus
}
#endif