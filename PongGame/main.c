#define F_CPU 16000000
#define FREQ 16000000
#define BAUD 9600
#define HIGH 1
#define LOW 0
#define BUFFER 1024
#define BLACK 0x000001

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



char displayChar = 0;
char * a;

uint8_t		play1_score = 0;
uint8_t		play2_score = 0;

struct Balls {
	uint8_t x;
	uint8_t y;
	uint8_t dx;
	uint8_t dy;
	uint8_t r;
};

enum Moods{
	MOOD_TOUCH,
	MOOD_ACCEL,
	MOOD_TWOPL
};


struct Balls ball;

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

void ball_init(void){
	ball.x	= 63;
	ball.y	= 13;
	ball.dx = ((rand() % 2) & 1) ? 1 : -1;
	ball.dy = ((rand() % 2) & 1) ? 1 : -1;
	ball.r	= 3;
}

void draw_background(void)
{

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
void draw_mood(void){
	clear_buffer(buff);
	drawstring(buff, 5, 0, "Please Select Mode");
	drawrect(buff, 20, 12, 97, 16, BLACK);
	drawstring(buff, 35, 2, "Touch");
	drawrect(buff, 20, 28, 97, 16, BLACK);
	drawstring(buff, 35, 4, "Accelerometer");
	drawrect(buff, 20, 44, 97, 16, BLACK);
	drawstring(buff, 35, 6, "Two Players");
	write_buffer(buff);
}
int choose_mood(void){
	int temp_counter = 0;
	int temp_x_array[30] = {0};
	int temp_y_array[30] = {0};	
	int temp_sum_x = 0;
	int temp_sum_y = 0;
	int avg_x = 0;
	int avg_y = 0;

	while(temp_counter<30){
		if (touch_detect()==1){
			temp_x_array[temp_counter-1] = touch_xvalue();
			temp_y_array[temp_counter-1] = touch_yvalue();
			temp_counter++;
			_delay_ms(2);
		}
		else{
			temp_counter=0;
		}
	}

	for (int i=0; i<30; i++){
		temp_sum_x += temp_x_array[i];
		temp_sum_y += temp_y_array[i];	
	}

	avg_x = temp_sum_x/30;
	avg_y = temp_sum_y/30;

	if (360>=avg_y && avg_y>200){
		clear_buffer(buff);
		drawrect(buff, 20, 12, 97, 16, BLACK);
		drawstring(buff, 35, 2, "Touch");
		write_buffer(buff);
		_delay_ms(1300);
		return MOOD_TOUCH;
	}
	else if (480>=avg_y && avg_y>360){
		clear_buffer(buff);
		drawrect(buff, 20, 12, 97, 16, BLACK);
		drawstring(buff, 35, 2, "Accelerometer");
		write_buffer(buff);
		_delay_ms(1300);
		return MOOD_ACCEL;
	}
	else if (650>=avg_y && avg_y>480){
		clear_buffer(buff);
		drawrect(buff, 20, 12, 97, 16, BLACK);
		drawstring(buff, 35, 2, "Two Players");
		write_buffer(buff);
		_delay_ms(1300);
		return MOOD_TWOPL;
	}
}

void score(void){
	char player1_score_display[] = "Score:";
	char player2_score_display[] = "Score:";
	drawstring(buff, 15, 0, player1_score_display);
	drawstring(buff, 76, 0, player2_score_display);
	drawchar(buff, 50, 0, play1_score + 48);
	drawchar(buff, 111, 0, play2_score + 48);
}

int move_ball(int c_ai, int c_pl){
	
	int win = 0;
	
	//update ball position
	ball.x += ball.dx;
	ball.y += ball.dy;
	
	//ball boundary
	uint8_t	ball_left = ball.y - ball.r;
	uint8_t	ball_right = ball.y + ball.r;
	uint8_t	ball_top = ball.x + ball.r;
	uint8_t	ball_bottom = ball.x - ball.r;

	//hit judge flag
	_Bool	hit_ai_paddle;
	_Bool	hit_player_paddle;
	
	//game finish flag
	_Bool	game_finish;
	
	/*rebound judge*/
	//ball hit boundary
	if((ball_left <= 0) || (ball_right >= 63)){
		ball.dy = -ball.dy;
		beep_tone();
		PORTB &= ~(1 << PORTB0);
		PORTB &= ~(1 << PORTB2);
		PORTD |= (1 << PORTD7);
	}

	hit_ai_paddle = (ball.x >= (122 - ball.r))
	&& (ball.y >= c_ai - 1) && (ball.y <= c_ai + 11);
	
	hit_player_paddle = (ball.x <= (5 + ball.r))
	&& (ball.y >= c_pl - 1) && (ball.y <= c_pl + 11);

	if (hit_ai_paddle){
		ball.dx = -ball.dx;
		ball.x  = ball.x - 3;
		beep_tone();
		PORTB &= ~(1 << PORTB0);
		PORTB |= (1 << PORTB2);
		PORTD &= ~(1 << PORTD7);
	}
	else if (hit_player_paddle){
		ball.dx = -ball.dx;
		ball.x  = ball.x + 3;
		beep_tone();
		PORTB &= ~(1 << PORTB0);
		PORTB |= (1 << PORTB2);
		PORTD &= ~(1 << PORTD7);
	}
	
	
	if(ball_top >= 126){
		play1_score ++;
		win = 1;
	}
	if(ball_top <= 2){
		play2_score ++;
		win = 1;
	}
	return win;
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

int move_paddle_pl1(int x_value, int y_value, int p_c){
	_Bool y_range = (y_value > 120) && (y_value < 630);
	_Bool x_range = (x_value >580);
	
	if(touch_detect() == 1){
		if(y_range && x_range){
			if(y_value < ((p_c * 8) + 120)){
				p_c--;
			}
			else if(y_value > (((p_c + 10) * 8) + 120)){
				p_c++;
			}
		}
	}

	return p_c;
}

int move_paddle_pl2(int x_value, int y_value, int p_c){
	_Bool y_range = (y_value > 120) && (y_value < 630);
	_Bool x_range = (x_value < 380);
	if(touch_detect() == 1){
		if(y_range && x_range){
			if(y_value < ((p_c * 8) + 120)){
				p_c--;
			}
			else if(y_value > (((p_c + 10) * 8) + 120)){
				p_c++;
			}
		}
	}

	return p_c;
}

int move_paddle_pl1_acc(int ADC_value, int p_c){
	
	if(ADC_value > 360){
		p_c++;
	}
	else if(ADC_value < 300){
		p_c--;
	}
	if(p_c < 2){
		p_c++;
	}
	if(p_c > 50){
		p_c--;
	}
	return p_c;
}

void draw_now(int c_ai, int c_pl, int ball_x, int ball_y)
{
	clear_buffer(buff);
	draw_background();
	score();
	drawcircle(buff, ball_x, ball_y, 3, BLACK);
	fillrect(buff, 2, c_ai, 3, 11, BLACK);
	fillrect(buff, 122, c_pl, 3, 11, BLACK);
	write_buffer(buff);
	return 0;
}

//buzzer
void beep_tone(){
	DDRB |= (1 << PORTB1);   // set Port B pin 1 to output
	TCCR1A |= (1 << COM1A0);
	TCCR1B |= (1 << CS10) | (1 << WGM12);
	OCR1A =  1000; 
	_delay_ms(50);
	TCCR1A &= ~(1 << COM1A0);
}


/*** accelerator ***/
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

int filtered_ADC(void){
	
	DDRC   |= 0x0;	// init PORTC to be all inputs
	PORTC  &= ~(1 << PORTC4); // set PC0 active high
	
	ADMUX  |= (1 << REFS0); //AVcc refer
	ADMUX  |= 0x04;
	ADCSRA |= (1 << ADEN); //turn on ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //division factor = 128, ADC works in
	ADCSRA |= (1 << ADATE); //Signal source, in this case is the free-running
	ADCSRA |= (1 << ADSC); //Start conversation
	ADCSRB &= ~((1 << ADTS2)|(1 << ADTS1)|(1 << ADTS0));//ADC in free-running mode
		
	int count = 0;
	int adc_value = 0;
	static int adc_array[10] = { 0,0,0,0,0,0,0,0,0,0 };
	while (count<10){
		adc_array[count] = ADC;

		count++;
	}
	adc_value = repeated_filter(adc_array);
}

void game_over(void){
	clear_buffer(buff);
	drawrect(buff, 20, 12, 97, 16, BLACK);
	drawstring(buff, 35, 2, "Game Over");
	drawrect(buff, 20, 44, 97, 16, BLACK);
	drawstring(buff, 35, 6, "Restart");
	write_buffer(buff);
}



int main(void)
{
	uart_init();
	init();	
	ball_init();
	
	int		paddle_ai	= 27;
	int		paddle_pl	= 27;
	int		paddle_pl2	= 27;

	int		touch_x		= 0;
	int		touch_y		= 0;
	_Bool	if_touched	= false;
	_Bool	if_dead		= false;

	int		ADC_value	= 0;
	int		next_round	= 0;
	
	draw_mood();
	_delay_ms(1000);
	int game_mood = choose_mood();
	
	sei();


	while (1)
	{
		if((play1_score < 3) && (play2_score < 3)){
			if(next_round == 0){
				switch (game_mood){
					case MOOD_TOUCH:
						//while(if_dead || if_touched){
						//draw_background();
						//}
						if (touch_detect()==1){
							touch_x = touch_xvalue();
							touch_y = touch_yvalue();
						}


						paddle_pl = move_paddle_pl1(touch_x,touch_y,paddle_pl);
						paddle_ai = move_paddle_ai(paddle_ai,ball.y);
						next_round = move_ball(paddle_ai, paddle_pl);
						draw_now(paddle_pl,paddle_ai,ball.x,ball.y);
						break;

					case MOOD_ACCEL:
						ADC_value = filtered_ADC();
						paddle_pl = move_paddle_pl1_acc(ADC_value, paddle_pl);
						paddle_ai = move_paddle_ai(paddle_ai,ball.y);
						next_round = move_ball(paddle_ai, paddle_pl);
						draw_now(paddle_pl,paddle_ai,ball.x,ball.y);
						break;

					case MOOD_TWOPL:
						//while(if_dead || if_touched){
						//draw_background();
						//}
						if (touch_detect()==1){
							touch_x = touch_xvalue();
							touch_y = touch_yvalue();
						}


						paddle_pl = move_paddle_pl1(touch_x,touch_y,paddle_pl);
						paddle_pl2 = move_paddle_pl2(touch_x,touch_y,paddle_pl2);
						next_round = move_ball(paddle_pl2, paddle_pl);
						draw_now(paddle_pl,paddle_pl2,ball.x,ball.y);		
						break;
				}
				_delay_ms(10);
			}
			else{
				ball_init();
				next_round = 0;
				_delay_ms(1000);
			}
		}
		else{
			game_over();
			_delay_ms(500);
			touch_y = touch_yvalue();			
			 if (650>=touch_y && touch_y>480){
				 clear_buffer(buff);
				drawrect(buff, 20, 12, 97, 16, BLACK);
				drawstring(buff, 35, 2, "Restart");
				write_buffer(buff);
				 _delay_ms(1300);
				draw_mood();
				_delay_ms(1000);
				game_mood = choose_mood();
				play1_score = 0;
				play2_score = 0;
				paddle_ai	= 27;
				paddle_pl	= 27;
				paddle_pl2	= 27;
				printf("game_mood: %d",game_mood);
			 }
		}
	}
}