/*
* SVET_TEST.cpp
*
* Created: 1/13/2022 12:53:21 PM
* Author : Pumpkin
*/

#define F_CPU 20000000UL //20MHz

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/core/core.h"


#define DIN pinB2
#define IMP pinA7
#define BTN pinA3

uint8_t base_ADC_level = 0;


void set_H_and_delay()	{ //send H bit
	PORTB |= 0b00000100;
	_delay_us(1);
	PORTB &= 0b11111011;
}

void set_L_and_delay()	{ //send L bit
	PORTB |= 0b00000100;
	PORTB &= 0b11111011;
	_delay_us(1);
}

void PWM_generation() {	   //8-bit PWM for reading the button status
	TCCR0A = 0b00100011; //COM0B1 = 0, COM0B0 = 1; WGM01 = 0, WGM00 = 1; (phase-corrent PWM on A7)
	TCCR0B = 0b00000001; // CLK
	TCNT0=0; //counter starting value
	OCR0B=127; //PWM duty
}




void EEPROM_write(uint8_t data, uint8_t address) {
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

void send_byte(unsigned char b) { // compare the HEX value of color to send the corresponding bits
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

void send_GRB(uint8_t g, uint8_t r, uint8_t b){ //send the current colors in the following order - green, red, blue (the value is HEX)
	for (uint8_t i = 0; i < 144; i++){
		send_byte (g);
		send_byte (r);
		send_byte (b);
	}
}



void ADC_init(){ //ADC on A2
	ADMUX = 0b00000010; //check the reference voltage or the VCC
	ADCSRA = 0b10000111; //CLK/128
	ADCSRB = 0b00010000; // left-shift ADCH for 8-bit value
}

void base_ADC_measure(){ //set the average adc level for fair measurement
	
	uint16_t current_ADC_level = 0;
	uint8_t adc_lvl = 0;
	for (uint8_t i = 0; i < 100; i++) //measure current ADC level 100 times
	{
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC) );
		adc_lvl = ADCH;
		current_ADC_level += (uint16_t)(adc_lvl);
	}
	current_ADC_level = current_ADC_level / 100u; //find the average value of base
	current_ADC_level = current_ADC_level * 95u; //set the base to 95% of the average
	current_ADC_level = current_ADC_level / 100u;
	::base_ADC_level = (uint8_t)(current_ADC_level);

}

void PWM_generation_16() {	   //16-bit PWM for testing purposes
	TCCR1A = 0b00100001; //COM0B1 = 0, COM0B0 = 1; WGM01 = 0, WGM00 = 1; (phase-corrent PWM on A7)
	TCCR1B = 0b00000001; // CLK (need more info on frequency)
	//TCCR1C = 0b01000000; // CLK (need more info on frequency)
	TCNT1H=0; //counter starting value
	TCNT1L=0; //counter starting value
	OCR1BH=ADCH; //PWM duty
	OCR1BL=ADCH; //PWM duty
}

bool get_button_status(){ //read the current button status
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC) );
	if(ADCH <= ::base_ADC_level){
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
	base_ADC_measure();


	int8_t dir = 1;
	bool btn_status = false, btn_press = false, turn_off = false;

	uint8_t cur_g = EEPROM_read(0);
	uint8_t cur_r = EEPROM_read(1);
	uint8_t cur_b = EEPROM_read(2);
	uint8_t stage = EEPROM_read(3);
	uint8_t brt = EEPROM_read(4);
	uint16_t last_stage_time = EEPROM_read(5);
	uint16_t cur_stage_time = EEPROM_read(6);
	uint16_t timebase = 0;
	while (1)
	{

		
		if (get_button_status())	{
			
			timebase++;

			if (timebase >= 50 && timebase < 1000 && btn_press == false){ //turning on or off the lights
				
				if (btn_status == false){//turning on the lights
					
					send_GRB(cur_g, cur_r, cur_b);
					btn_status = !btn_status;
					btn_press = true;
					turn_off = false;
					_delay_ms(100);
				}
				else { //turning off the lights
					send_GRB(0,0,0);
					EEPROM_write(cur_g, 0);
					EEPROM_write(cur_r, 1);
					EEPROM_write(cur_b, 2);
					EEPROM_write(stage, 3);
					EEPROM_write(brt, 4);
					EEPROM_write(last_stage_time, 5);
					EEPROM_write(cur_stage_time, 6);
					btn_status = !btn_status;
					btn_press = true;
					turn_off = true;
					_delay_ms(100);
				}
			}
			else if (timebase >= 1000 && turn_off == false) //start the color changing procedure
			{
				if (last_stage_time != 0){
					timebase += last_stage_time;
					last_stage_time = 0;
				}
				
				brt += dir;
				if (brt >= 255) {
					
					dir = -1;
				}
				else if (brt == 0) {
					
					dir = 1;
				}
				cur_stage_time++;
				// time the animation
				if (timebase > 1509) {
					cur_stage_time=0;
					timebase = 1000;

					stage += 1;
					brt = 0;
					dir = 1;
					if (stage >= 8) {
						stage = 0;
					}
				}


				if (stage == 0) { //red
					send_GRB(0,brt,0);
					cur_g = 0;
					cur_r = brt;
					cur_b = 0;

					} else if (stage == 1) { //orange
					send_GRB(brt / 2,brt,0);
					cur_g = brt / 2;
					cur_r = brt;
					cur_b = 0;

					} else if (stage == 2) {//yellow
					send_GRB(brt,brt,0);
					cur_g = brt;
					cur_r = brt;
					cur_b = 0;

					} else if (stage == 3) {//green
					send_GRB(brt,0,0);
					cur_g = brt;
					cur_r = 0;
					cur_b = 0;

					} else if (stage == 4) {//light blue
					send_GRB(brt,0, brt);
					cur_g = brt;
					cur_r = 0;
					cur_b = brt;
					} else if (stage == 5) {// blue
					send_GRB(0, 0, brt);
					cur_g = 0;
					cur_r = 0;
					cur_b = brt;

					} else if (stage == 6) {//purple
					send_GRB(0, brt / 2, brt);
					cur_g = 0;
					cur_r = brt / 2;
					cur_b = brt;

					} else if (stage == 7) {//white
					send_GRB(brt, brt, brt);
					cur_g = brt;
					cur_r = brt;
					cur_b = brt;

				}
			}
		}
		else {

			btn_press = false;
			timebase = 0;
			last_stage_time = cur_stage_time;
		}
		_delay_ms(1);
	}
}
