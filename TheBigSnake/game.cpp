#define is_down(b) input->buttons[b].is_down
#define pressed(b) input->buttons[b].is_down && input->buttons[b].is_changed
#define released(b) !input->buttons[b].is_down && input->buttons[b].is_changed


enum Gamemode {

	GM_MAINMENU,
	GM_GAMEPLAY,
	GM_LEVEL,
	GM_HIGHSCORES,
	GM_GAMEOVER
};
Gamemode current_gamemode;

enum SnakeDirection {
	STOP,
	RIGHT,
	LEFT,
	UP,
	DOWN
};
SnakeDirection snake_direction;

class GameData {
private: 
	int high_scores[5];
	int level;
public:
	GameData() {
		for (int i = 0; i < 5; i++)
		{
			high_scores[i] = 0;
		}
		level = 1;
	}

	int get_level() {
		return level;
	}

	void assign_level(int a) {
		level = a;
	}

	int get_high_score(int index) {
		return high_scores[index];
	}

	void update_highscore(int score) {
		if (high_scores[4] < score) {
			high_scores[4] = score;
			for (int i = 0; i < 5; i++) {
				for (int j = i; j < 5; j++) {
					if (high_scores[i] < high_scores[j]) {
						int temp = high_scores[i];
						high_scores[i] = high_scores[j];
						high_scores[j] = temp;
					}
				}
			}
		}
	}
};
GameData gamedata;

u32 arena_border_color = 0x1b09b8, arena_color = 0x6758ed;
u32 title_color = 1, options_color= 0xff0000,selected_option_color=0xffcccc,high_score_color=0xb2e320;
u32 head_color=0xff0000, tail_color=0xff5500, food_color=0x00ff00, score_color=0x00fffb;
u32 gameover_rect_color = 0x00fffb,gameover_text_color=0x09054a,back_to_mainmenu_color=0x4141cc;

int main_menu_selection, level_selection = gamedata.get_level() - 1;;
float arena_half_size_x=85, arena_half_size_y=45;
float snake_half_size_x = 1.f, snake_half_size_y = 1.f;
float food_half_size_x = 0.5f, food_half_size_y = 0.5f;
float snake_speed = 2.f;
int score;

#define MAX  120

float snake_pos_x[MAX], snake_pos_y[MAX];
float food_pos_x, food_pos_y;
int length_of_snake;
bool gameOver = false;
bool foodPosAllocated = false;

bool isFirstLoop = true;


internal void set_default_game_values() {
	score=0;
	length_of_snake=5;
	for (int i = 0; i < length_of_snake; i++)
	{
		snake_pos_x[i] = 0 - i * 2 * snake_half_size_x;
		snake_pos_y[i] = 0;

	}
	gameOver = false;
	foodPosAllocated = false;
}

internal void draw_arena() {
	draw_arena_borders(arena_half_size_x, arena_half_size_y, arena_border_color);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, arena_color);;
}

internal void read_game_data() {
	std::ifstream ifile("gamedata.bin", std::ios::in | std::ios::binary);
	if (ifile.is_open()) {
		ifile.read(reinterpret_cast<char*>(&gamedata), sizeof(gamedata));
		ifile.close();
	}
}
internal void update_game_data() {
	std::ofstream ofile("gamedata.bin", std::ios::out | std::ios::binary);
	if (ofile.is_open()) {
		ofile.write(reinterpret_cast<char*>(&gamedata), sizeof(gamedata));
		ofile.close();
	}
}



internal void simulate_game(Input* input, float dt)
{
	if (isFirstLoop) {
		set_default_game_values();

		read_game_data();
		level_selection = gamedata.get_level() - 1;

		isFirstLoop = false;
	}
	//simulate arena
	if (current_gamemode!=GM_GAMEOVER) draw_arena();
	
	if (current_gamemode==GM_GAMEPLAY){

		//make tail follow the snake head
		if (snake_direction != STOP) {
			for (int i = length_of_snake - 1; i > 0; i--) {
				snake_pos_x[i] = snake_pos_x[i - 1];
				snake_pos_y[i] = snake_pos_y[i - 1];
			}
			
		}
		//movement of head
		if (snake_direction == UP) snake_pos_y[0] += snake_speed * gamedata.get_level() * dt;
		if (snake_direction == DOWN) snake_pos_y[0] -= snake_speed * gamedata.get_level() * dt;
		if (snake_direction == RIGHT) snake_pos_x[0] += snake_speed * gamedata.get_level() * dt;
		if (snake_direction == LEFT) snake_pos_x[0] -= snake_speed * gamedata.get_level() * dt;
		

		//input
		if (!gameOver) {
			if ((pressed(BUTTON_UP) || pressed(BUTTON_W)) && snake_direction != DOWN) snake_direction = UP;
			if ((pressed(BUTTON_DOWN) || pressed(BUTTON_S)) && snake_direction != UP) snake_direction = DOWN;
			if ((pressed(BUTTON_RIGHT) || pressed(BUTTON_D)) && snake_direction != LEFT) snake_direction = RIGHT;
			if ((pressed(BUTTON_LEFT) || pressed(BUTTON_A)) && snake_direction != RIGHT) snake_direction = LEFT;
		}

		//draw snake 
		for (int i = 0; i < length_of_snake; i++) {
			if (i == 0) draw_rect(snake_pos_x[0], snake_pos_y[0], snake_half_size_x, snake_half_size_y, head_color);
			else draw_rect(snake_pos_x[i], snake_pos_y[i], snake_half_size_x, snake_half_size_y, tail_color);
		}

		//draw food
		if (!foodPosAllocated) {
			srand(rand());
			food_pos_x = rand() % static_cast<int>(arena_half_size_x - 2 * snake_half_size_x);
			food_pos_y = rand() % static_cast<int>(arena_half_size_y - 2 * snake_half_size_x);
			foodPosAllocated = true;
		}
		draw_rect(food_pos_x, food_pos_y, food_half_size_x, food_half_size_y, food_color);

		//draw_score
		draw_number(score, 0, 40, 1.f, score_color);

		//Collision detection
		{
			//collision with food 
			if (food_pos_x - food_half_size_x < snake_pos_x[0] + snake_half_size_x &&
				food_pos_x + food_half_size_x > snake_pos_x[0] - snake_half_size_x &&
				food_pos_y - food_half_size_y < snake_pos_y[0] + snake_half_size_y &&
				food_pos_y + food_half_size_y > snake_pos_y[0] - snake_half_size_y) {

				foodPosAllocated = false;
				score += gamedata.get_level();
				if (length_of_snake < MAX)length_of_snake++;
			}

			//collision with walls
			if (arena_half_size_x - snake_half_size_x <= snake_pos_x[0] ||
				arena_half_size_y - snake_half_size_y <= snake_pos_y[0] ||
				arena_half_size_x - snake_half_size_x <= -snake_pos_x[0] ||
				arena_half_size_y - snake_half_size_y <= -snake_pos_y[0]) {

				gameOver = true;
				snake_direction = STOP;
				current_gamemode = GM_GAMEOVER;
			}

		}


	}
	else if (current_gamemode == GM_GAMEOVER){
		draw_rect(0, 0, 50, 20, gameover_rect_color);
		draw_text("YOUR FINAL SCORE IS", -40,10, .5f, gameover_text_color);
		draw_number(score, 40, 8, 2.f, gameover_text_color);
		draw_text("BACK TO MAINMENU", -25, -10, .5f, back_to_mainmenu_color);
		
		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_MAINMENU;
			gamedata.update_highscore(score);
			update_game_data();
			set_default_game_values();
		}
	}
	else if (current_gamemode==GM_MAINMENU){

		draw_text("THE BIG SNAKE", -35, 40, 1.f, title_color);

		if (main_menu_selection == 0) {
			draw_text("PLAY", -10, 20, 1.f, selected_option_color);
			draw_text("LEVEL", -12, 5, 1.f, options_color);
			draw_text("HIGHSCORES", -25, -10, 1.f, options_color);
			draw_text("EXIT", -10, -25, 1.f, options_color);
		}
		else if(main_menu_selection==1){
			draw_text("PLAY", -10, 20, 1.f, options_color);
			draw_text("LEVEL", -12, 5, 1.f, selected_option_color);
			draw_text("HIGHSCORES", -25, -10, 1.f, options_color);
			draw_text("EXIT", -10, -25, 1.f, options_color);
		}
		else if (main_menu_selection == 2) {
			draw_text("PLAY", -10, 20, 1.f, options_color);
			draw_text("LEVEL", -12, 5, 1.f, options_color);
			draw_text("HIGHSCORES", -25, -10, 1.f, selected_option_color);
			draw_text("EXIT", -10, -25, 1.f, options_color);
		}
		else {
			draw_text("PLAY", -10, 20, 1.f, options_color);
			draw_text("LEVEL", -12, 5, 1.f, options_color);
			draw_text("HIGHSCORES", -25, -10, 1.f, options_color);
			draw_text("EXIT", -10, -25, 1.f, selected_option_color);
		}

		if (pressed(BUTTON_UP) || pressed(BUTTON_W))main_menu_selection--;
		if (pressed(BUTTON_DOWN) || pressed(BUTTON_S))main_menu_selection++;
		if (main_menu_selection > 3) main_menu_selection = 0;
		else if (main_menu_selection < 0) main_menu_selection = 3;

		if (pressed(BUTTON_ENTER)) {
			switch (main_menu_selection) {
			case 0:
				current_gamemode = GM_GAMEPLAY;
				break;
			case 1:
				current_gamemode = GM_LEVEL;
				break;
			case 2:
				current_gamemode = GM_HIGHSCORES;
				break;
			case 3:
				running = false;
				break;
			}
		}
	}
	else if (current_gamemode == GM_LEVEL) {
		draw_text("LEVEL", -12, 40, 1.f, title_color);

		if (level_selection == 0) {
			draw_text("EASY", -10, 20, 1.f, selected_option_color);
			draw_text("MEDIUM", -15, 5, 1.f, options_color);
			draw_text("HARD", -10, -10, 1.f, options_color);
			draw_text("BACK TO MAIN MENU", -45, -25, 1.f, options_color);
		}else if(level_selection ==1){
			draw_text("EASY", -10, 20, 1.f, options_color);
			draw_text("MEDIUM", -15, 5, 1.f, selected_option_color);
			draw_text("HARD", -10, -10, 1.f, options_color);
			draw_text("BACK TO MAIN MENU", -45, -25, 1.f, options_color);
		}
		else if(level_selection == 2){
			draw_text("EASY", -10, 20, 1.f, options_color);
			draw_text("MEDIUM", -15, 5, 1.f, options_color);
			draw_text("HARD", -10, -10, 1.f, selected_option_color);
			draw_text("BACK TO MAIN MENU", -45, -25, 1.f, options_color);
		}
		else {
			draw_text("EASY", -10, 20, 1.f, options_color);
			draw_text("MEDIUM", -15, 5, 1.f, options_color);
			draw_text("HARD", -10, -10, 1.f, options_color);
			draw_text("BACK TO MAIN MENU", -45, -25, 1.f, selected_option_color);
		}

		if (pressed(BUTTON_UP) || pressed(BUTTON_W))level_selection--;
		if (pressed(BUTTON_DOWN) || pressed(BUTTON_S))level_selection++;
		if (level_selection > 3) level_selection = 0;
		else if (level_selection < 0) level_selection = 3;

		if (pressed(BUTTON_ENTER)) {
			switch (level_selection) {
			case 0:
				gamedata.assign_level(1);
				update_game_data();
				current_gamemode = GM_MAINMENU;
				break;
			case 1:
				gamedata.assign_level(2);
				update_game_data();
				current_gamemode = GM_MAINMENU;
				break;
			case 2:
				gamedata.assign_level(3);
				update_game_data();
				current_gamemode = GM_MAINMENU;
				break;
			case 3:
				current_gamemode = GM_MAINMENU;
				break;
			}
		}
	}
	else if (current_gamemode == GM_HIGHSCORES){
		draw_text("HIGHSCORES", -30, 40, 1.f, title_color);

		for (int i = 0; i < 5; i++)
		{
			draw_number(i+1, -8, 18-i*10, .5f, options_color);
			draw_text(".", -6, 24-i*10, 1.f, options_color);
			if(i==0)draw_number(gamedata.get_high_score(i), 6, 20, 1.5f, high_score_color);
			else draw_number(gamedata.get_high_score(i), 2, 18 - i * 10, 1.f, score_color);
		}	

		draw_text("BACK TO MAINMENU", -25, -30, .5f, selected_option_color);

		if (pressed(BUTTON_ENTER)) current_gamemode = GM_MAINMENU;

	}



}








/*




//collision
		


		*/