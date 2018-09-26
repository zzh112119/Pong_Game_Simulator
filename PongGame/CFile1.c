/*
 * Lab2 3.1.c
 *
 * Created: 9/17/2018 3:18:00 PM
 * Author : zzh11
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "uart.h"
#include <stdlib.h>
#include <avr/interrupt.h>
float voltage = 0.0f;
#include <util/delay.h>

int compare_function(const void *a, const void *b)
{
	int *x = (int*)a;
	int *y = (int*)b;
	return *x-*y;	
}

int repeated_filter(int t_array[])
{
	int n = sizeof(t_array)/sizeof(*t_array);
	qsort(t_array, n, sizeof(*t_array), compare_function);
	int max_count = 1, res = t_array[0], curr_count = 1;
	for (int i=1; i<n; i++)
	{	if (t_array[i]==t_array[i-1])
		{curr_count++;}
		else{	
			if(curr_count>max_count){
				max_count = curr_count;
				res = t_array[i-1];
			}
			curr_count = 1;
		}
	}
	
	if(curr_count>max_count){
		max_count = curr_count;
		res = t_array[n-1];
	}
	return res;
}
int main(void)
{
	uart_init();
	
	DDRC = 0x0;	// init PORTC to be all inputs
	PORTC &= ~(1 << PORTC4); // set PC4	 active high
	
	ADMUX |= (1 << REFS0) | (1 << REFS1); //AVcc refer
	ADCSRA |= (1 << ADEN); //turn on ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //division factor = 128, ADC works in
	ADCSRA |= (1 << ADATE); //Signal source, in this case is the free-running
	ADCSRA |= (1 << ADSC); //Start conversation
	ADCSRB &= ~((1 << ADTS2)|(1 << ADTS1)|(1 << ADTS0));//ADC in free-running mode
	
	int count = 0;
	int adc_value = 0;

	static int adc_array[10] = { 0,0,0,0,0,0,0,0,0,0 };

	while (1)
	{
		//float voltage = (float)ADC / 1024.0f * 5.0f;//convert ADC value to voltage value
		//printf("%.2f V \n",voltage);
		while (count<10){ 
			adc_array[count] = ADC;
			_delay_ms(10);
			count++;
		}
		for(int i=0; i<10; i++){
			printf("%d,",adc_array[i]);
		}		
		adc_value = repeated_filter(adc_array);
		printf("ADC: %d\n", adc_value);
		count = 0;
	}
}