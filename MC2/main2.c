/*
 * main1.c
 *
 *  Created on: Oct 30, 2021
 *      Author: Salma
 */

#include "uart.h"
#include <util/delay.h>
#include "external_eeprom.h"
#include "gpio.h"
#include "buzzer.h"
#include "dcMotor.h"
#include "twi.h"
#include <avr/io.h>
#include "timer.h"


/*MCU States and General Definitions*/
#define F_CPU 8000000
#define MC2_READY 0x10
#define MC1_READY 0x01
#define EEPROM_START 0x00
#define PASS_MATCH 1
#define PASS_MISMATCH 0
#define PASS_CHANGED 6
#define OPEN_DOOR 3
#define CHANGE_PASS 4
#define ROTATING_PERIOD 5
#define STOPPING_PERIOD 3
#define BUZZER_ON_PERIOD 6
#define NUMBER_OF_WRONG_ATTEMPTS 3

/*Global Variables*/
uint8 recieved_pass1[5], recieved_pass2[5];
uint8 saved_pass[5] ; uint8 confirmation_pass[5];
uint16 g_seconds = 0;
uint8 state; uint8 num_of_wrong_attempts;

/*Function Prototypes*/
uint8 compare_password(uint8 a1[5],uint8 a2[5]);
void open_door();
void read_saved_password();
void uart_receivePassword(uint8 array[5]);
void save_password();
void password_init();
void handshake();

/*Function Definitions*/
void handshake(){
	while(UART_recieveByte() != MC1_READY);
	UART_sendByte(MC2_READY);
}

void open_door(){
	handshake();
	if(UART_recieveByte() == OPEN_DOOR ){
		handshake();
		uart_receivePassword(confirmation_pass);
		read_saved_password();

		if(compare_password(saved_pass,confirmation_pass) == 1){
			UART_sendByte(PASS_MATCH);
			g_seconds = 0;
			DcMotor_Rotate(cw);
			while (g_seconds < ROTATING_PERIOD);
			g_seconds = 0;
			DcMotor_Rotate(stop);
			while (g_seconds < STOPPING_PERIOD);
			g_seconds = 0;
			DcMotor_Rotate(acw);
			while (g_seconds < ROTATING_PERIOD);
			DcMotor_Rotate(stop);
		}
		else{
			num_of_wrong_attempts++;
			UART_sendByte(PASS_MISMATCH);
			if(num_of_wrong_attempts == NUMBER_OF_WRONG_ATTEMPTS){
				buzzer_ON();
				g_seconds=0;
				while(g_seconds < BUZZER_ON_PERIOD);
				buzzer_OFF();
				num_of_wrong_attempts=0;
			}
		}
	}
}

void read_saved_password(){
	int i;
	for (i = 0; i < 5; ++i) {
		EEPROM_readByte(EEPROM_START + i, saved_pass+i);
		_delay_ms(100);
	}
}

void uart_receivePassword(uint8 array[5]){
	uint8 i;
	for (i=0 ; i<5 ; i++){
		*(array + i) = UART_recieveByte();
	}
}
uint8 compare_password(uint8 a1[5],uint8 a2[5]){
	uint8 x;
	for(int i = 0 ; i < 5 ; i++){
		if(a1[i] == a2[i]){x=1;}
		else{x=0;}
	}
	return x;
}
void save_password(){
	for (int i = 0 ; i < 5 ; i++){
		EEPROM_writeByte(EEPROM_START + i,recieved_pass1[i]);
		_delay_ms(100);
	}
}
void password_init(){

	handshake();
	uart_receivePassword(recieved_pass1);

	handshake();
	uart_receivePassword(recieved_pass2);

	UART_sendByte(compare_password(recieved_pass1,recieved_pass2));
	if(compare_password(recieved_pass1,recieved_pass2)==1){
		save_password();
	}
	else{
		UART_sendByte(PASS_MISMATCH);
	}
}
void timer_counter(){
	g_seconds++;
}

int main(){
	SREG |=(1<<7);
	uart_config uart_ptr = {char_size_8, disabled, 1, 9600};
	UART_init(&uart_ptr);
	/* Timer freq = 8MHz/1024, one clock-cycle time = 128 uSecond
	 interrupt every 1 second ,, compare value 1/(128us) = 7813 */
	Timer_configuration timer_config_ptr = { Timer1, compare, 0, 7813, pre1024, timer_counter };
	Timer_init(&timer_config_ptr);
	TWI_Config twi_ptr = {fast , pre_1};
	TWI_init(&twi_ptr);
	DcMotor_Init();
	buzzer_Init();
	password_init();
	while(1){
		state=UART_recieveByte();
		if(state == OPEN_DOOR){
			open_door();
		}
		else if(state == CHANGE_PASS)
		{
			handshake();
			uart_receivePassword(confirmation_pass);
			read_saved_password();

			if(compare_password(confirmation_pass,saved_pass)==1){
				UART_sendByte(PASS_MATCH);
				handshake();
				uart_receivePassword(recieved_pass1);
				save_password();
				UART_sendByte(PASS_CHANGED);
			}
			else{
				UART_sendByte(PASS_MISMATCH);
				num_of_wrong_attempts++;
				if(num_of_wrong_attempts == NUMBER_OF_WRONG_ATTEMPTS){
					buzzer_ON();
					g_seconds=0;
					while(g_seconds < BUZZER_ON_PERIOD);
					buzzer_OFF();
					num_of_wrong_attempts=0;
				}
			}
		}
	}
}

