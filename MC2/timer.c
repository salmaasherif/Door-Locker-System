/*
 *	File name: timer.c
 *  Created on: Oct 30, 2021
 *  Author: Salma
 */

#include "timer.h"
#include "std_types.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>


static void (*g_Timer0CallBackPtr)(void) = NULL_PTR;
static void (*g_Timer1CallBackPtr)(void) = NULL_PTR;
static void (*g_Timer2CallBackPtr)(void) = NULL_PTR;

void Timer_init(const Timer_configuration *timer_config){
	if (timer_config->timer_ID == Timer0){

		SET_BIT(TCCR0,FOC0); /* for non-PWM */
		TCNT0 = timer_config->initialValue;
		TCCR0 = 0;
		TCCR0 |= (timer_config->prescaler);

		if (timer_config->mode == overflow){
			SET_BIT(TIMSK,TOIE0);
			SET_BIT(TIFR,TOV0);

		}else if (timer_config->mode == compare){
			SET_BIT(TCCR0,WGM01);
			OCR0 = timer_config->compareValue;
			SET_BIT(TIMSK,OCIE0);
			SET_BIT(TIFR,OCF0);
		}

		g_Timer0CallBackPtr = timer_config->callBackPtr;


	}else if (timer_config->timer_ID == Timer1){
		TCCR1A =0;
		SET_BIT(TCCR1A,FOC1A);
		SET_BIT(TCCR1A,FOC1B);
		TCCR1B = timer_config->prescaler;
		TCNT1 = timer_config->initialValue;

		if (timer_config->mode == overflow) {
			SET_BIT(TIMSK, TOIE1);
			SET_BIT(TIFR,TOV1);

		} else if (timer_config->mode == compare) {
			SET_BIT(TCCR1B, WGM12);
			OCR1A = timer_config->compareValue;
			SET_BIT(TIMSK, OCIE1A);
			SET_BIT(TIFR,OCF1A);
		}
		g_Timer1CallBackPtr = timer_config->callBackPtr;
	}
	else if (timer_config->timer_ID == Timer2){
		SET_BIT(TCCR2, FOC2); /* for non-PWM */
		TCNT2 = timer_config->initialValue;
		TCCR2 = 0;
		TCCR2 |= (timer_config->prescaler);

		if (timer_config->mode == overflow) {
			SET_BIT(TIMSK, TOIE2);
			SET_BIT(TIFR,TOV2);
		}
		else if (timer_config->mode == compare) {
			SET_BIT(TCCR0, WGM21);
			OCR2 = timer_config->compareValue;
			SET_BIT(TIMSK, OCIE2);
			SET_BIT(TIFR,OCF2);
		}
		g_Timer2CallBackPtr = timer_config->callBackPtr;
	}
}


void Timer_deinit(Timer_ID timer_ID){
	if (timer_ID == Timer0){
		TCCR0 = 0;
		TCNT0= 0;
		OCR0 = 0;
		CLEAR_BIT(TIMSK,OCIE0);
		CLEAR_BIT(TIMSK,TOIE0);
		g_Timer0CallBackPtr = NULL_PTR;

	}else if (timer_ID == Timer1){
		TCCR1A =0;
		TCCR1B = 0;
		TCNT1 = 0;
		CLEAR_BIT(TIMSK, TOIE1);
		OCR1A = 0;
		CLEAR_BIT(TIMSK, OCIE1A);
		g_Timer1CallBackPtr = NULL_PTR;

	}else if (timer_ID == Timer2){
		TCCR2 = 0;
		TCNT2 = 0;
		OCR2 = 0;
		CLEAR_BIT(TIMSK, OCIE2);
		CLEAR_BIT(TIMSK, TOIE2);
		g_Timer2CallBackPtr = NULL_PTR;
	}
}
ISR(TIMER0_OVF_vect) {
	if (g_Timer0CallBackPtr != NULL_PTR) {
		(*g_Timer0CallBackPtr)();
	}
}

ISR(TIMER0_COMP_vect) {
	if (g_Timer0CallBackPtr != NULL_PTR) {
		(*g_Timer0CallBackPtr)();
	}
}

ISR(TIMER1_OVF_vect) {
	if (g_Timer1CallBackPtr != NULL_PTR) {
		(*g_Timer1CallBackPtr)();
	}
}

ISR(TIMER1_COMPA_vect) {
	if (g_Timer1CallBackPtr != NULL_PTR) {
		(*g_Timer1CallBackPtr)();
	}
}

ISR(TIMER2_OVF_vect) {
	if (g_Timer2CallBackPtr != NULL_PTR) {
		(*g_Timer2CallBackPtr)();
	}
}

ISR(TIMER2_COMP_vect) {
	if (g_Timer2CallBackPtr != NULL_PTR) {
		(*g_Timer2CallBackPtr)();
	}
}
