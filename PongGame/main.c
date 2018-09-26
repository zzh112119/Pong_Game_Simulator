#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include "lcd.h"


#define FREQ 16000000
#define BAUD 9600
#define HIGH 1
#define LOW 0
#define BUFFER 1024
#define BLACK 0x000001

char displayChar = 0;
char * a;

uint8_t	move_x = 2;
uint8_t move_y = 2;
uint8_t	direct;

int main(void)
{
	//setting up the gpio for backlight
	DDRD |= 0x80;
	PORTD &= ~0x80;
	PORTD |= 0x00;
	
	DDRB |= 0x05;
	PORTB &= ~0x05;
	PORTB |= 0x00;
	
	//lcd initialisation
	lcd_init();
	lcd_command(CMD_DISPLAY_ON);
	lcd_set_brightness(0x20);
	//range: x:0-127,y:0-7
	write_buffer(buff);
	_delay_ms(10000);
	clear_buffer(buff);
	_delay_ms(5000);
	displayChar++;
	
	while (1)
	{
		//
		drawline(buff, 0 , 0, 127, 0, BLACK);
		drawline(buff, 0 , 0, 0, 63, BLACK);
		drawline(buff, 0 , 63, 127, 63, BLACK);
		drawline(buff, 127 , 0, 127, 63, BLACK);
		
		drawline(buff, 63 , 0, 63, 4, BLACK);
		drawline(buff, 63 , 8, 63, 12, BLACK);
		drawline(buff, 63 , 16, 63, 20, BLACK);
		drawline(buff, 63 , 24, 63, 28, BLACK);
		
		drawline(buff, 63 , 59, 63, 63, BLACK);
		drawline(buff, 63 , 51, 63, 55, BLACK);
		drawline(buff, 63 , 43, 63, 47, BLACK);
		drawline(buff, 63 , 35, 63, 39, BLACK);
		
		drawcircle(buff, 63, 31, 3, BLACK);
		
		fillrect(buff, 2, 27, 3, 11, BLACK);
		fillrect(buff, 122, 27, 3, 11, BLACK);
		
		write_buffer(buff);
		//
	}
}

