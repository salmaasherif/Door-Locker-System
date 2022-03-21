/*
 * main1.c
 *
 *  Created on: Oct 30, 2021
 *      Author: Salma
 */

#include "keypad.h"
#include "gpio.h"
#include "lcd.h"
#include "uart.h"
#include <util/delay.h>
#include "password.h"
#include "std_types.h"
#include <avr/io.h>
#include "timer.h"

/*MCU States and General Definitions*/
#define F_CPU 8000000
#define MC2_READY 0x10
#define MC1_READY 0x01
#define PASS_MATCH 1
#define PASS_MISMATCH 0
#define PASS_CHANGED 6
#define OPEN_DOOR 3
#define CHANGE_PASS 4
#define ROTATING_PERIOD 5
#define STOPPING_PERIOD 3

/*Global Variables*/
uint8 password[5] ; uint8 confirm_pass[5];
uint8 state; uint16 g_seconds;

/*Function Prototypes*/
void main_options();
void uart_sendPassword(uint8  array[]);
void getpass(uint8 * arrayName);
void password_init();
void open_door();
void handshake();

/*Function Definitions*/
void handshake(){
	UART_sendByte(MC1_READY);
	while (UART_recieveByte() != MC2_READY);
}

void timer_counter(){
	g_seconds++;
}

void uart_sendPassword(uint8  array[]){
	uint8 i;
	for (i=0;i<5;i++){
		UART_sendByte(array[i]);
	}
}
void getpass(uint8 * arrayName){
	LCD_moveCursor(1, 0);
	uint8 i=0;
	uint8 key;

	while(i!=5){
		key = KEYPAD_getPressedKey();
		if (key >= 0 && key <= 9) {
			LCD_displayCharacter('*');
			*(arrayName + i) = key;
			i++;
		}
		_delay_ms(3000);
	}
	key=0;
}

void main_options(void){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "+: open door");
	LCD_displayStringRowColumn(1, 0, "-: change pass");
}

void password_init(){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter password");
	LCD_moveCursor(1,0);
	getpass(password);

	handshake();
	uart_sendPassword(password);
	LCD_clearScreen();

	LCD_displayString("Re-enter pass");
	LCD_moveCursor(1,0);
	getpass(password);

	handshake();
	uart_sendPassword(password);

	state = UART_recieveByte();
	if(state == PASS_MISMATCH){
		LCD_clearScreen();
		LCD_displayString("Wrong password");
		_delay_ms(5000);
		password_init();
	}
	else{
		main_options();
	}
}

void open_door(){
	handshake();
	UART_sendByte(OPEN_DOOR);

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Confirm pass");
	LCD_moveCursor(1,0);
	getpass(confirm_pass);

	handshake();
	uart_sendPassword(confirm_pass);
	state = UART_recieveByte();
	if(state == PASS_MATCH){
		LCD_clearScreen();
		LCD_displayString("DOOR OPENING");
		g_seconds=0;
		while(g_seconds < ROTATING_PERIOD);
		LCD_clearScreen();
		LCD_displayString("STOPPING");
		g_seconds=0;
		while(g_seconds < STOPPING_PERIOD);
		LCD_clearScreen();
		LCD_displayString("DOOR CLOSING");
		g_seconds=0;
		while(g_seconds < ROTATING_PERIOD);
		LCD_clearScreen();

	}
	else if(state == PASS_MISMATCH){
		LCD_clearScreen();
		LCD_displayString("Pass Mismatch");
		LCD_displayStringRowColumn(1,0,"Try Again");
		_delay_ms(5000);
		main_options();	}
}


int main(){
	SREG = 1<<7;
	LCD_init();
	uart_config uart_ptr = {char_size_8, disabled, 1, 9600 };
	UART_init(&uart_ptr);
	password_init();
	/* Timer freq = 8MHz/1024, one clock-cycle time = 128 uSecond
	 interrupt every 1 second ,, compare value 1/(128us) = 7813 */
	Timer_configuration timer_config_ptr = { Timer1, compare, 0, 7813, pre1024, timer_counter };
	Timer_init(&timer_config_ptr);
	while(1){
		if(KEYPAD_getPressedKey() == '+'){
			UART_sendByte(OPEN_DOOR);
			open_door();
			main_options();
		}
		else if(KEYPAD_getPressedKey() == '-'){
			UART_sendByte(CHANGE_PASS);
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Enter old pass");
			LCD_moveCursor(1,0);
			getpass(password);
			handshake();
			uart_sendPassword(password);
			state = UART_recieveByte();
			if(state == PASS_MATCH){
				LCD_clearScreen();
				LCD_displayString("Enter new pass");
				LCD_moveCursor(1,0);
				getpass(password);
				handshake();
				uart_sendPassword(password);

				if(UART_recieveByte()==PASS_CHANGED){
					LCD_clearScreen();
					LCD_displayString("Pass Changed.");
					_delay_ms(10000);
					main_options();
				}
			}
			else if(state == PASS_MISMATCH){
				LCD_clearScreen();
				LCD_displayString("Pass Mismatch");
				LCD_displayStringRowColumn(1,0,"Try Again");
				_delay_ms(5000);
				main_options();
			}
		}
	}
}
