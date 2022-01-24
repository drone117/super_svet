/*
* SVET_TEST.cpp
*
* Created: 1/13/2022 12:53:21 PM
* Author : Pumpkin
*/

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/core/core.h"
//#include "lib/core/stdafx.h"

#define DIN pinB2
#define IMP pinA7
#define BTN pinA3


//static bool b_BTN = true;

void set_H_and_delay()	{
	PORTB = 0b00000100;
	_delay_us(1);
	PORTB = 0b00000000;
}

void set_L_and_delay()	{
	PORTB = 0b00000100;
	PORTB = 0b00000000;
	_delay_us(1);
}

void PWM_generation() {
	TCCR0A = 0b00100011; //COM0B1 = 0, COM0B0 = 1; WGM01 = 0, WGM00 = 1; (phase-corrent PWM on A7)
	TCCR0B = 0b00000001; // CLK (need more info on frequency)
	TCNT0=0; //counter starting value
	OCR0B=127; //PWM duty
}

void ADC_init(){ //ADC on A2
	ADMUX = 0b00000010; //check the reference voltage or the VCC ???????????????????????????
	ADCSRA = 0b10000111; //CLK/128 (~60kHz)
	ADCSRB = 0b00010000; // left-shift ADCH for 8-bit value
	//while (ADCSRA & (1 << ADSC) );
}

void EEPROM_write(uint8_t data, uint8_t address/*, something else? */) {
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	
	/* Set Programming mode */
	EECR = (0<<EEPM1)|(0<<EEPM0);
	
	/* Set up address and data registers */
	EEAR = address;
	EEDR = data;
	
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}

uint8_t EEPROM_read(uint8_t address)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	
	/* Set up address register */
	EEAR = address;
	
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	
	/* Return data from data register */
	return EEDR;
}

void send_byte(unsigned char b) {
	if (b & 0b10000000) {
		set_H_and_delay();
	}
	else {
		set_L_and_delay();
	}
	if (b & 0b01000000) {
		set_H_and_delay();
	}
	else {
		set_L_and_delay();
	}
	if (b & 0b00100000) {
		set_H_and_delay();
	}
	else {
		set_L_and_delay();
	}
	if (b & 0b00010000) {
		set_H_and_delay();
	}
	else {
		set_L_and_delay();
	}
	if (b & 0b00001000) {
		set_H_and_delay();
	}
	else {
		set_L_and_delay();
	}
	if (b & 0b00000100) {
		set_H_and_delay();
	}
	else {
		set_L_and_delay();
	}
	if (b & 0b00000010) {
		set_H_and_delay();
	}
	else {
		set_L_and_delay();
	}
	if (b & 0b00000001) {
		set_H_and_delay();
	}
	else {
		set_L_and_delay();
	}
}

void send_GRB(uint8_t g, uint8_t r, uint8_t b){
	for (uint8_t i = 0; i < 10; i++){
		send_byte (g);
		send_byte (r);
		send_byte (b);
	}
}

bool get_button_status(){
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC) );
	if(ADCH <= 150){
		return true;
	}
	else{
		return false;
	}
}

main()
{
	pinMode(DIN, OUTPUT);
	pinMode(IMP, OUTPUT);
	pinMode(BTN, INPUT);
	digitalWrite(BTN, HIGH);
	PWM_generation();
	ADC_init();
	uint8_t brt = 0,stage = 0;
	int8_t dir = 1;
	bool btn_status = false, btn_press = false;
	
	//uint8_t n_of_diodes = 10;
	
	//uint8_t cur_g = EEPROM_read(0);
	//uint8_t cur_r = EEPROM_read(1);
	//uint8_t cur_b = EEPROM_read(2);
	
	uint8_t cur_g = 0, cur_r = 0, cur_b = 0;
	//uint8_t prev_g = 0, prev_r = 0, prev_b = 0, min_brt = 0;
	int timebase = 0;
	_delay_ms(10);
	while (1)
	{
		//if (get_button_status() && timebase == 0){
		//if (!get_button_status()){
		//if(btn_status == 0){
		//send_GRB(cur_g,cur_r,cur_b);
		//}
		//else {
		//send_GRB(0,0,0);
		//}
		//btn_status = !btn_status;
		//_delay_ms(10);
		//}
		//}
		
		
		if (get_button_status() && btn_press == false)	{
			_delay_ms(500);
			if (!get_button_status()){
				if(btn_status == false){
					send_GRB(cur_g,cur_r,cur_b);
					_delay_ms(100);
					btn_press = true;
					} else {
					send_GRB(0,0,0);
					//EEPROM_write(cur_g, 0);
					//EEPROM_write(cur_r, 1);
					//EEPROM_write(cur_b, 2);
					_delay_ms(100);
				}
				btn_status = !btn_status;
			}
		}
		if (get_button_status())	{
			timebase++;
			//digitalWrite(IMP, HIGH);

			brt += dir;
			if (brt >= 255) {
				dir = -1;
			}
			else if (brt == 0) {
				dir = 1;
			}

			// time the animation
			if (timebase > 200) {
				timebase = 0;
				stage += 1;
				if (stage >= 8) {
					stage = 0;
				}
			}
			if (stage == 0) {
				send_GRB(0,brt,0);
				cur_g = 0;
				cur_r = brt;
				cur_b = 0;
				} else if (stage == 1) {
				send_GRB(brt / 2,brt,0);
				cur_g = brt / 2;
				cur_r = brt;
				cur_b = 0;
				} else if (stage == 2) {
				send_GRB(brt,brt,0);
				cur_g = brt;
				cur_r = brt;
				cur_b = 0;
				} else if (stage == 3) {
				send_GRB(brt,0,0);
				cur_g = brt;
				cur_r = 0;
				cur_b = 0;
				} else if (stage == 4) {
				send_GRB(brt,0, brt);
				cur_g = brt;
				cur_r = 0;
				cur_b = brt;
				} else if (stage == 5) {
				send_GRB(0, 0, brt);
				cur_g = 0;
				cur_r = 0;
				cur_b = brt;
				} else if (stage == 6) {
				send_GRB(0, brt / 2, brt);
				cur_g = 0;
				cur_r = brt / 2;
				cur_b = brt;
				} else if (stage == 7) {
				send_GRB(brt, brt, brt);
				cur_g = brt;
				cur_r = brt;
				cur_b = brt;
			}
		}
		else {
			btn_press = false;
		}
		
		//if (timebase >= 100 && timebase < 116) {
		//
		//if (!get_button_status()){
		//timebase = 0;
		//break;
		//}
		//brt += dir;
		//if (brt >= 7){
		//dir = -1;
		//}
		//else if (brt == 0){
		//dir = 1;
		//}
		//if (dir == -1){
		//cur_g = cur_g / 2;
		//cur_r = cur_r / 2;
		//cur_b = cur_b / 2;
		//}
		//else {
		//cur_g = cur_g * 2;
		//cur_r = cur_r * 2;
		//cur_b = cur_b * 2;
		//}
		//send_GRB(cur_g, cur_r, cur_b);
		//_delay_ms(100);
		//}
		//else if (timebase >= 116 && timebase < 117)
		//{
		//brt = 6;
		//min_brt = 0;
		//for (int j = 0; j < 1536; j++){
		//if (!get_button_status()){
		//timebase = 0;
		//break;
		//}
		//
		//if(j < 256){
		//cur_g = 255;
		//cur_r = j;
		//cur_b = 0;
		//send_GRB(cur_g, cur_r, cur_b);
		//_delay_ms(1);
		//continue;
		//}
		//
		//if(j < 512){
		//cur_g = 512-j;
		//cur_r = 255;
		//cur_b = 0;
		//send_GRB(cur_g, cur_r, cur_b);
		//_delay_ms(1);
		//continue;
		//}
		//if(j < 768){
		//cur_g = 0;
		//cur_r = 255;
		//cur_b = j-512;
		//send_GRB(cur_g, cur_r, cur_b);
		//_delay_ms(1);
		//continue;
		//}
		//if(j < 1024){
		//cur_g = 0;
		//cur_r = 1024-j;
		//cur_b = 255;
		//send_GRB(cur_g, cur_r, cur_b);
		//_delay_ms(1);
		//continue;
		//
		//}
		//if(j < 1280){
		//cur_g = j-1024;
		//cur_r = 0;
		//cur_b = 255;
		//send_GRB(cur_g, cur_r, cur_b);
		//_delay_ms(1);
		//continue;
		//}
		//if (j < 1536){
		//cur_g = 255;
		//cur_r = 0;
		//cur_b = 1536-j;
		//send_GRB(cur_g, cur_r, cur_b);
		//_delay_ms(1);
		//continue;
		//}
		//break;
		//}
		//}
		//else if (timebase >= 117){
		//timebase = 100;
		//min_brt = 0;
		//brt = 6;
		//cur_g = 255;
		//cur_r = 255;
		//cur_b = 255;
		//}
		//}
		//else {
		//
		////digitalWrite(IMP, LOW);
		//timebase = 0;
		//}


		//digitalWrite(IMP, HIGH);

		//digitalWrite(IMP, LOW);
		//_delay_ms(100);
		

		//digitalWrite(IMP, HIGH);
		//_delay_us(1);
		//digitalWrite(IMP, LOW);
		//_delay_us(1);
		
		_delay_ms(3);
	}
}
