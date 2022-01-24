#define LOW		 0x00
#define HIGH	 0x01

#define INPUT	 0x00
#define OUTPUT	 0x01

#define ADC0     0x90
#define ADC1     0x91
#define ADC2     0x92
#define ADC3     0x93
#define ADC4     0x94
#define ADC5     0x95
#define ADC6     0x96
#define ADC7     0x97

#define pinA0    0xA0
#define pinA1    0xA1
#define pinA2    0xA2
#define pinA3    0xA3
#define pinA4    0xA4
#define pinA5    0xA5
#define pinA6    0xA6
#define pinA7    0xA7

#define pinB0    0xB0
#define pinB1    0xB1
#define pinB2    0xB2
#define pinB3    0xB3
#define pinB4    0xB4
#define pinB5    0xB5
#define pinB6    0xB6
#define pinB7    0xB7

#define pinC0    0xC0
#define pinC1    0xC1
#define pinC2    0xC2
#define pinC3    0xC3
#define pinC4    0xC4
#define pinC5    0xC5
#define pinC6    0xC6

#define pinD0    0xD0
#define pinD1    0xD1
#define pinD2    0xD2
#define pinD3    0xD3
#define pinD4    0xD4
#define pinD5    0xD5
#define pinD6    0xD6
#define pinD7    0xD7

void pinMode(uint8_t byte_set_pin, uint8_t byte_mode)  {
	uint8_t byte_port = byte_set_pin & 0xf0;
	uint8_t byte_pin = byte_set_pin & 0x0f;
	uint8_t byte_DDRx;
	volatile uint8_t *DDRx;
	
	//
	switch (byte_port) {
		case 0xA0:
		DDRx = &DDRA;
		break;
		case 0xb0:
		DDRx = &DDRB;
		break;
		/*
		case 0xc0:
		DDRx = &DDRC;
		break;
		case 0xd0:
		DDRx = &DDRD;
		break;
		*/
	}
	
	byte_DDRx = *DDRx;
	byte_DDRx &= ~(1 << byte_pin);
	if ( (byte_mode == INPUT) || (byte_mode == OUTPUT) )  {
		byte_DDRx |= (byte_mode << byte_pin);
		*DDRx = byte_DDRx;
	}
	//
}

void digitalWrite(uint8_t byte_set_pin, uint8_t byte_mode)  {
	uint8_t byte_port = byte_set_pin & 0xf0;
	uint8_t byte_pin = byte_set_pin & 0x0f;
	uint8_t byte_PORTx = 0;
	volatile uint8_t *PORTx;
	//
	switch (byte_port) {
		case 0xA0:
		PORTx = &PORTA;
		break;
		case 0xB0:
		PORTx = &PORTB;
		break;
		/*
		case 0xc0:
		PORTx = &PORTC;
		break;
		case 0xd0:
		PORTx = &PORTD;
		break;
		*/
	}

	byte_PORTx = *PORTx;
	byte_PORTx &= ~(1 << byte_pin);
	if ((byte_mode == HIGH) || (byte_mode == LOW))  {
		byte_PORTx |= (byte_mode << byte_pin);
		*PORTx = byte_PORTx;
	}
	//
}

uint8_t digitalRead(uint8_t byte_set_pin)  {
	uint8_t byte_port = byte_set_pin & 0xf0;
	uint8_t byte_pin = byte_set_pin & 0x0f;
	uint8_t byte_PINx = 0;
	volatile uint8_t *PINx;
	//
	switch (byte_port) {
		case 0xA0:
		PINx = &PINA;
		break;
		case 0xB0:
		PINx = &PINB;
		break;
		/*
		case 0xc0:
		PORTx = &PORTC;
		break;
		case 0xd0:
		PORTx = &PORTD;
		break;
		*/
	}

	byte_PINx = *PINx;

	if (( byte_PINx & (1<<byte_pin) ) == 0)  {
		return LOW;
	} else {
		return HIGH;
	}
	//
}