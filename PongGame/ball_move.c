bool    ball_move(uint8_t ball, uint8_t paddle){
	
	//update ball position
	ball.x += ball.dx;
	ball.y += ball.dy;
	
	//ball boundary
	uint8_t    ball_left = ball.x - ball.r;
	uint8_t    ball_right = ball.x + ball.r;
	uint8_t    ball_top = ball.y + ball.r;
	uint8_t    ball_bottom = ball.y - ball.r;
	
	uint8_t    ai_paddle_top = paddle.x;
	uint8_t    ai_paddle_bottom = paddle.x + paddle.h;
	uint8_t    ai_paddle_left = paddle.y;
	uint8_t    ai_paddle_right = paddle.y + paddle.w;	
	
	uint8_t    player_paddle_top = paddle.x +  paddle.h;
	uint8_t    player_paddle_bottom = paddle.x;
	uint8_t    player_paddle_left = paddle.y + paddle.w;
	uint8_t    player_paddle_right = paddle.y;
	
	//hit judge flag
	bool	   hit_ai_paddle;
	bool       hit_player_paddle;
	
	//game finish flag
	bool game_finish;
	
	/*rebound judge*/
	//ball hit boundary
	if((ball_left < 0) || (ball_right > 63)){
		ball.dy = -ball.dy;
	}
	//ball hit paddle
	hit_ai_paddle = (ball_top > ai_paddle_top) 
		&& (ball.x <= ai_paddle_right) && (ball.x >= ai_paddle_left);
		
	hit_player_paddle = (ball_bottom < player_paddle_top) 
		&& (ball.x <= player_paddle_left) && (ball.x >= player_paddle_right);
	if(hit_ai_paddle || hit_player_paddle){
		ball.dx = -ball.dx;
	}
	
	return game_finish;
}