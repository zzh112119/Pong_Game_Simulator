#define F_CPU 16000000

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include <stdbool.h>
#include "lcd.h"


#define FREQ 16000000
#define BAUD 9600
#define HIGH 1
#define LOW 0
#define BUFFER 1024
#define BLACK 0x000001

char displayChar = 0;
char * a;

uint8_t	direct;

void init(void){
	//setting up the GPIO for back light
	DDRD |= 0x80;
	PORTD &= ~0x80;
	PORTD |= 0x00;
	
	DDRB |= 0x05;
	PORTB &= ~0x05;
	PORTB |= 0x00;
	
	//LCD initialization
	lcd_init();
	lcd_command(CMD_DISPLAY_ON);
	lcd_set_brightness(0x20);
	//range: x:0-127,y:0-7
	write_buffer(buff);
	_delay_ms(10000);
	clear_buffer(buff);
	_delay_ms(5000);
	displayChar++;
}

void draw_background(void){
	/*
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
*/
	
	drawcircle(buff, 63, 31, 3, BLACK);
		
	fillrect(buff, 2, 27, 3, 11, BLACK);
	fillrect(buff, 122, 27, 3, 11, BLACK);
		
	write_buffer(buff);
	//
}

void choose_mood(void){}

int move_ball(int c,int x, int y){
	
	return c, x, y;
}

int move_paddle_ai(int c_ai, int c_ball[2]){
	
	if(c_ball[1] > c_ai){
		c_ai++;
	}
	else if(c_ball[1] < c_ai){
		c_ai--;
	}
	
	return c_ai;
}

int move_paddle_pl(int b_c, int p_c){
	
	return p_c;
}

void draw_now(int c_ai, int c_pl, int c_ball[2]){
	
	drawcircle(buff, c_ball[0], c_ball[1], 3, BLACK);
		
	fillrect(buff, 2, c_ai, 3, 11, BLACK);
	fillrect(buff, 122, c_pl, 3, 11, BLACK);
		
	write_buffer(buff);
	return 0;
}

int main(void)
{
	init();
	choose_mood();
	draw_background();
	int		ball_c[2]	= {63, 31};
	int		paddle_ai	= 27;
	int		paddle_pl	= 27;
	int		ball_dx		= rand() %10 ;
	int		ball_dy		= rand() %10 ;
	_Bool	if_touched	= false; 
	_Bool	if_dead		= false;

		
	while (1)
	{
		while(if_dead || if_touched){
			draw_background();
		}
		ball_c[0]--;
		ball_c[1]--;
		//paddle_pl--;
		//paddle_ai++;
		//ball_c, ball_dx, ball_dy = move_ball();
		//move_paddle_pl();
		paddle_ai = move_paddle_ai(paddle_ai,ball_c);
		paddle_pl = move_paddle_ai(paddle_pl,ball_c);
		draw_now(paddle_ai,paddle_pl,ball_c);
		_delay_ms(100);
		clear_buffer(buff);
	}
}

