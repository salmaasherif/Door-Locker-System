#include "dcMotor.h"
#include "gpio.h"
#include "common_macros.h"
#include <avr/io.h>

void DcMotor_Init(void){
	GPIO_setupPinDirection(PORTB_ID, 0 , PIN_OUTPUT);
	GPIO_setupPinDirection(PORTB_ID, 1 , PIN_OUTPUT);
	//initially, motor is off
	CLEAR_BIT(PORTB,0);
	CLEAR_BIT(PORTB,1);
}

void DcMotor_Rotate(DcMotor_State state){
	if(state==cw){    /* for cw rotation */
		SET_BIT(PORTB,0);
	}
	else if(state==acw){    /* for acw rotation */
		SET_BIT(PORTB,1);
	}
	else {  /*stop*/
		CLEAR_BIT(PORTB,0);
		CLEAR_BIT(PORTB,1);
	}
}


