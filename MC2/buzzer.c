/*
 * buzzer.c
 *
 *  Created on: Oct 30, 2021
 *      Author: Salma
 */
#include "buzzer.h"
#include "gpio.h"

#define BUZZER_PORT_ID   PORTC_ID
#define BUZZER_PIN_ID    PIN3_ID

void buzzer_Init(){
	GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PIN_ID, PIN_OUTPUT);
	buzzer_OFF();
}

void buzzer_ON(){
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_HIGH);
}

void buzzer_OFF(){
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}
