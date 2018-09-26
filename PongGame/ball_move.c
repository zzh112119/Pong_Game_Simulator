bool    ball_move(ball_t ball, paddle_t paddle[2]){

	//ball boundary
	uint8_t    ball_left = ball.x - ball.r;
	uint8_t    ball_right = ball.x + ball.r;
	uint8_t    ball_top = ball.y + ball.r;
	uint8_t    ball_bottom = ball.y - ball.r;
	
	uint8_t    ai_paddle_top = paddle[0].x;
	uint8_t    ai_paddle_bottom = paddle[0].x + paddle[0].h;
	uint8_t    ai_paddle_left = paddle[0].y;
	uint8_t    ai_paddle_right = paddle[0].y + paddle[0].w;	
	
	uint8_t    player_paddle_top = paddle[1].x +  paddle[1].h;
	uint8_t    player_paddle_bottom = paddle[1].x;
	uint8_t    player_paddle_left = paddle[1].y + paddle[1].w;
	uint8_t    player_paddle_right = paddle[1].y;
	
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