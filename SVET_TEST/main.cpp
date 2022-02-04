/*
* SVET_TEST.cpp
*
* Created: 1/13/2022 12:53:21 PM
* Author : Pumpkin
*/

#define F_CPU 20000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/core/core.h"
//#include "lib/core/stdafx.h"

#define DIN pinB2
#define IMP pinA7
#define BTN pinA3

uint8_t base_ADC_level = 0;
//static bool b_BTN = true;

void set_H_and_delay()	{
	PORTB |= 0b00000100;
	_delay_us(1);
	PORTB &= 0b11111011;
}

void set_L_and_delay()	{
	PORTB |= 0b00000100;
	PORTB &= 0b11111011;
	_delay_us(1);
}

void PWM_generation() {
	TCCR0A = 0b00100011; //COM0B1 = 0, COM0B0 = 1; WGM01 = 0, WGM00 = 1; (phase-corrent PWM on A7)
	TCCR0B = 0b00000001; // CLK (need more info on frequency)
	TCNT0=0; //counter starting value
	OCR0B=127; //PWM duty
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
	for (uint8_t i = 0; i < 144; i++){
		send_byte (g);
		send_byte (r);
		send_byte (b);
	}
}



void ADC_init(){ //ADC on A2
	uint16_t current_ADC_level = 0;

	uint8_t adc_lvl = 0;
	ADMUX = 0b00000010; //check the reference voltage or the VCC ???????????????????????????
	ADCSRA = 0b10000111; //CLK/128 (~?kHz)
	ADCSRB = 0b00010000; // left-shift ADCH for 8-bit value
	ADCSRA |= (1 << ADSC);

	for (uint8_t i = 0; i < 100; i++)
	{
		while (ADCSRA & (1 << ADSC) );
		adc_lvl = ADCH;
		current_ADC_level += adc_lvl;
	}
	
	current_ADC_level = current_ADC_level / 100u;
	current_ADC_level = current_ADC_level * 8u;
	current_ADC_level = current_ADC_level / 10u;
	::base_ADC_level = (uint8_t)(current_ADC_level);

}

bool get_button_status(){
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC) );
	if(ADCH <= base_ADC_level){
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
	ADC_init();
	PWM_generation();
	uint8_t brt = 0,stage = 0, cur_g = 0, cur_r = 0, cur_b = 0;
	int8_t dir = 1;
	bool btn_status = false, btn_press = false;

	//uint8_t cur_g = EEPROM_read(0);
	//uint8_t cur_r = EEPROM_read(1);
	//uint8_t cur_b = EEPROM_read(2);
	//uint8_t stage = EEPROM_read(3);
	//uint8_t brt = EEPROM_read(4);

	int timebase = 0;
	_delay_ms(10);
	send_GRB(255,255,255);
	while (1)
	{
		
		if (get_button_status())	{
			
			timebase++;
			//digitalWrite(IMP, HIGH);
			if (timebase >= 50 && timebase < 1000 && btn_press == false){
				
				if (btn_status == false){
					
					send_GRB(cur_g, cur_r, cur_b);
					btn_status = !btn_status;
					btn_press = true;
					_delay_ms(100);
				}
				else {
					send_GRB(0,0,0);
					EEPROM_write(cur_g, 0);
					EEPROM_write(cur_r, 1);
					EEPROM_write(cur_b, 2);
					EEPROM_write(stage, 3);
					EEPROM_write(brt, 4);
					btn_status = !btn_status;
					btn_press = true;
					_delay_ms(100);
				}
			}
			else if (timebase >= 1000)
			{
				
				
				brt += dir;
				if (brt >= 255) {
					
					dir = -1;
				}
				else if (brt == 0) {
					
					dir = 1;
				}

				// time the animation
				if (timebase > 1509) {
					
					timebase = 1000;
					stage += 1;
					brt = 0;
					dir = 1;
					if (stage >= 8) {
						stage = 0;
					}
				}
				//send_GRB(0,0,255-ADCH);

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
		}
		else {
			//btn_status = false;
			btn_press = false;
			timebase = 0;
		}
		_delay_ms(2);
	}
}
