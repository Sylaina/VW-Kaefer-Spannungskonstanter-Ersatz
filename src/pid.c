#include "pid.h"

// limits of return value of pid_compute()
// type can be defined by application, 
// default are uint8_t for using as 8-bit PWM registers

#define RETURN_VALUE_MIN	0
#define RETURN_VALUE_MAX	0xff

void pid_init(float kp, 
			  float ki, 
			  float kd, 
			  pid_controller_t* controller){
    controller->kpFactor = kp;
    controller->kiFactor = ki;
    controller->kdFactor = kd;
    controller->lastValue = 0;
    controller->error = 0;
    controller->lastError = 0;
    controller->preLastError = 0;
}

// remember chance type-cast of return if you change type of pid_compute()
float pid_compute(float sollWert, 
				  float istWert, 
				  pid_controller_t* controller){
    controller->error = sollWert - istWert;
    
    float y = controller->lastValue
    + (controller->kpFactor+controller->kiFactor+controller->kdFactor) * controller->error
    + (-controller->kpFactor-2*controller->kdFactor) * controller->lastError
    + controller->kdFactor * controller->preLastError;
    
    controller->preLastError = controller->lastError;
    controller->lastError = controller->error;
    
    // limit y at type of pid_compute()
    if(y < RETURN_VALUE_MIN){
    	y = RETURN_VALUE_MIN;
    }else if(y > RETURN_VALUE_MAX){
    	y = RETURN_VALUE_MAX;
    }
    
    controller->lastValue = y;
    
    // cast y to type of pid_compute()
    return y;
}
