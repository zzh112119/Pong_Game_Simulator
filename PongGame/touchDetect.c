#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


int touch_xvalue(void){
	
	int temp_x;
	ADMUX &= ~0x0F; //reset ADC input port
	ADMUX |= 0x03;
	DDRC = 0x00;	// init PORTC to be all inputs
	DDRC |= (1 << PORTC0) | (1 << PORTC1); //Port C2/C3 outputs
	
	PORTC |= (1 << PORTC1) ;
	PORTC &= ~((1 << PORTC0) | (1<< PORTC2)|(1<<PORTC3)); // set PC0 active high
	_delay_ms(10);

	//printf("X: %d\n", ADC );
	temp_x = ADC;
	return temp_x;
}


int touch_yvalue(void){
	int temp_y;
	ADMUX &= ~0x0F;
	DDRC = 0x00;	// init PORTC to be all inputs
	DDRC |= (1 << PORTC2) | (1 << PORTC3); //Port C2/C3 outputs
		
	PORTC |= (1 << PORTC3);
	PORTC &= ~((1 << PORTC0) | (1<< PORTC2) |(1 << PORTC1)); // set PC0 active high
	_delay_ms(10);

	//printf("Y: %d\n", ADC );
	temp_y = ADC;	
	return temp_y;
}



int touch_detect(void){
	DDRC = 0x00;	// init PORTC to be all inputs
	DDRC |= (1 << PORTC2) | (1 << PORTC3); //Port C2/C3 outputs
	
	PORTC |= (1 << PORTC1);
	PORTC &= ~((1 << PORTC0) | (1<< PORTC2)|(1<<PORTC3) ); // set PC0 active high
	
	ADMUX |= (1 << REFS0); //AVcc refer
	ADMUX |= 0x01;
	ADCSRA |= (1 << ADEN); //turn on ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //division factor = 128, ADC works in
	ADCSRA |= (1 << ADATE); //Signal source, in this case is the free-running
	ADCSRA |= (1 << ADSC); //Start conversation
	ADCSRB &= ~((1 << ADTS2)|(1 << ADTS1)|(1 << ADTS0));//ADC in free-running mode
	_delay_ms(10);
	if (ADC > 500){
		return 0; // not touched yet
	}
	else {
		return 1; // touched
	}
}
