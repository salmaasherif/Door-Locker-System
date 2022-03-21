/*
 *	File name: timer.h
 *  Created on: Oct 30, 2021
 *  Author: Salma
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

typedef enum{
	Timer0, Timer1, Timer2
}Timer_ID;

typedef enum{
	overflow,
	compare
}Timer_mode;

typedef enum{
	no_clk, pre1, pre8, pre64, pre256, pre1024
}Timer_Prescaler;

typedef struct{
	Timer_ID timer_ID;
	Timer_mode mode;
	uint16 initialValue;
	uint16 compareValue;
	Timer_Prescaler prescaler;
	void (*callBackPtr)();
}Timer_configuration;


void Timer_init(const Timer_configuration* timer_config);
void Timer_deinit(Timer_ID timer_ID);

#endif /* TIMER_H_ */
