#define F_CPU 16000000

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include <stdbool.h>
#include "uart.h"
#include "lcd.h"
#include "touchDetect.h"

#define FREQ 16000000
#define BAUD 9600
#define HIGH 1
#define LOW 0
#define BUFFER 1024
#define BLACK 0x000001

char displayChar = 0;
char * a;

uint8_t		play1_score = 1;
uint8_t		play2_score = 0;

struct balls {
	uint8_t x;
	uint8_t y;
	uint8_t dx;
	uint8_t dy;
	uint8_t r;
};

struct balls ball;

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

}

void choose_mood(void){
	drawstring(buff, 5, 0, "Select Mode");
	
	drawrect(buff, 20, 12, 97, 16, BLACK);
	drawstring(buff, 35, 2, "Touch");
	
	drawrect(buff, 20, 28, 97, 16, BLACK);
	drawstring(buff, 35, 4, "Accelerometer");
	
	drawrect(buff, 20, 44, 97, 16, BLACK);
	drawstring(buff, 35, 6, "2 Players");
	
}

void score(void){
	char player1_score_display[] = "Score:";
	char player2_score_display[] = "Score:";
	drawstring(buff, 15, 0, player1_score_display);
	drawstring(buff, 76, 0, player2_score_display);
	drawchar(buff, 50, 0, play1_score + 48);
	drawchar(buff, 111, 0, play2_score + 48);
}

void move_ball(int c_ai, int c_pl){
	//update ball position
	ball.x += ball.dx;
	ball.y += ball.dy;
	
	//ball boundary
	uint8_t	ball_left = ball.y - ball.r;
	uint8_t	ball_right = ball.y + ball.r;
	uint8_t	ball_top = ball.x + ball.r;
	uint8_t	ball_bottom = ball.x - ball.r;
	
	uint8_t	ai_paddle_top = 122 - 1;
	uint8_t	ai_paddle_bottom = 122 + 1;
	uint8_t	ai_paddle_left = c_ai + 5;
	uint8_t	ai_paddle_right = c_ai - 5;
	
	uint8_t	player_paddle_top = 2 + 3;
	uint8_t	player_paddle_bottom = 2 - 1;
	uint8_t	player_paddle_left = c_pl - 5;
	uint8_t	player_paddle_right = c_pl + 5;
	
	//hit judge flag
	_Bool	hit_ai_paddle;
	_Bool	hit_player_paddle;
	
	//game finish flag
	_Bool	game_finish;
	
	/*rebound judge*/
	//ball hit boundary
	if((ball_left <= 0) || (ball_right >= 63)){
		ball.dy = -ball.dy;
		PORTB &= ~(1 << PORTB0);
		PORTB &= ~(1 << PORTB2);
		PORTD |= (1 << PORTD7);
	}
	//ball hit paddle
	hit_ai_paddle = (ball_top > ai_paddle_top)
	&& (ball.y >= ai_paddle_right) && (ball.y <= ai_paddle_left);
	
	hit_player_paddle = (ball_bottom < player_paddle_top)
	&& (ball.y >= player_paddle_left) && (ball.y <= player_paddle_right);
	if(hit_ai_paddle || hit_player_paddle){
		ball.dx = -ball.dx;
		PORTB &= ~(1 << PORTB0);
		PORTB |= (1 << PORTB2);
		PORTD &= ~(1 << PORTD7);
	}
	
	//return game_finish;
}

int move_paddle_ai(int c_ai, int ball_y){
	if(c_ai <= 0){
		c_ai++;
	}
	else if(c_ai + 10 >= 63){
		c_ai--;
	}
	else{
		if(ball_y > c_ai + 5){
			c_ai++;
		}
		else if(ball_y < c_ai + 5){
			c_ai--;
		}
	}
	return c_ai;
}

int move_paddle_pl(int b_c, int p_c){
	return p_c;
}

void draw_now(int c_ai, int c_pl, int ball_x, int ball_y){
	
	drawcircle(buff, ball_x, ball_y, 3, BLACK);
	
	fillrect(buff, 2, c_ai, 3, 11, BLACK);
	fillrect(buff, 122, c_pl, 3, 11, BLACK);
	
	write_buffer(buff);
	return 0;
}


int main(void)
{
	uart_init();

	init();
	clear_buffer(buff);
	choose_mood();
	write_buffer(buff);
	
	ball.x	= 63;
	ball.y	= 13;
	ball.dx = ((rand() % 2) & 1) ? 1 : -1;
	ball.dy = ((rand() % 2) & 1) ? 1 : -1;
	ball.r	= 3;
	
	int		paddle_ai	= 27;
	int		paddle_pl	= 27;

	int		touch_x		= 0;
	int		touch_y		= 0;
	_Bool	if_touched	= false;
	_Bool	if_dead		= false;

	
	while (1)
	{
		//while(if_dead || if_touched){
		//draw_background();
		//}
		score();
		draw_background();
		if (touch_detect()==1){
			
			printf("Touched");
		}

		//ball.x--;
		//ball.y--;
		//paddle_pl--;
		//paddle_ai++;
		move_ball(paddle_ai, paddle_pl);
		//move_paddle_pl();
		paddle_ai = move_paddle_ai(paddle_ai,ball.y);
		paddle_pl = move_paddle_ai(paddle_pl,ball.y);
		draw_now(paddle_ai,paddle_pl,ball.x,ball.y);
		_delay_ms(40);
		clear_buffer(buff);
	}
}