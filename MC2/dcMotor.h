#ifndef DCMOTOR_H_
#define DCMOTOR_H_
#include "std_types.h"

typedef enum{
	stop,
	cw,
	acw
}DcMotor_State;


void DcMotor_Init(void);
/*
• Description
➢ The Function responsible for setup the direction for the two
  motor pins through the GPIO driver.
➢ Stop at the DC-Motor at the beginning through the GPIO driver.
 */

void DcMotor_Rotate(DcMotor_State state);
/*
 * • Description:
➢ The function is responsible for rotating the DC Motor CW/
or A-CW or stop the motor based on the state input state value.
➢ Send the required duty cycle to the PWM driver
based on the required speed value.
• Inputs:
➢ state: The required DC Motor state, it should be CW or
 A-CW or stop.
➢ speed: decimal value for the required motor speed,
it should be from 0 → 100. For example, if the input is 50,
The motor should rotate with 50% of its maximum speed.
 */

#endif /* DCMOTOR_H_ */
