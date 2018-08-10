#include "cmdpathfinder.h"

int main() {
	srand(time(NULL));
	CmdPathfinder* game = new CmdPathfinder();
	while(game->getRunning()) {
		switch(game->getMenu()) {
			case 0:
				game->displayMainMenu();
				break;
			case 1:
				game->runGame();
				break;
			case 2:
				game->completionScreen();
				break;
			case 3:
				game->levelFailed("Level failed. You have not landed on all X spaces before the E. Enter r to retry or press e to return to main menu");
				break;
			case 4:
				game->selectLevel();
				break;
			case 5:
				game->levelFailed("Level failed. No more valid moves. Enter r to retry or press e to return to main menu");
				break;
			case 6:
				game->generationOptionsScreen();
				break;
			default:
				game->setRunning(false);
		}
	}
	delete game;
	return 0;
}

CmdPathfinder::CmdPathfinder() {
	running = true;
	menu = 0;
	number_of_levels = 9;
	current_level = -1;
	for(int i=0;i < number_of_levels;i++) {
		level_completion.push_back("");
	}
	custom_level_has_generated = false;
	generate_level_x = 5;
	generate_level_y = 5;
}

bool CmdPathfinder::getRunning() {
	return running;
}

int CmdPathfinder::getMenu() {
	return menu;
}

void CmdPathfinder::clearLevel(Level& clear_level) {
	clear_level.level.clear();
	clear_level.start_x = 0;
	clear_level.start_y = 0;
	clear_level.player_x = 0;
	clear_level.player_y = 0;
	clear_level.end_x = 0;
	clear_level.end_y = 0;
}

void CmdPathfinder::setRunning(bool will_run) {
	running = will_run;
}

void CmdPathfinder::displayMainMenu() {
	std::string input;
	std::string error;
	bool valid_option = false;
	bool exit_flag = false;
	while(!valid_option) {	
		system("CLS");
		std::cout << "*********************************\n";
		std::cout << "*                               *\n";
		std::cout << "*   Welcome to CMD_Pathfinder   *\n";
		std::cout << "*                               *\n";
		std::cout << "*********************************\n";
		std::cout << "\n";
		std::cout << error+"\n";
		std::cout << "\n";
		std::cout << "1. Start Game\n";
		std::cout << "2. Instructions\n";
		std::cout << "3. Exit\n";
		if(!exit_flag) {
			std::cout << "\n";
			std::cout << "Please enter menu selection: ";
		} else {
			std::cout << "\n";
			std::cout << "Are you sure you want to exit the game (y/n)?: ";
		}

		char key = _getch();
		std::string option = castCharToStr(key);
		if(!exit_flag) {
			if(option == "1") {
				menu = 4;
				valid_option = true;
			} else if (option == "2") {
				displayInstructions();
			} else if (option == "3") {
				exit_flag = true;
			} else {
				error = "Invalid option";
			}
		} else {
			if(option == "y") {
				menu = -1;
				valid_option = true;
			} else if(option == "n") {
				exit_flag = false;
				error = "";
			} else {
				error = "Invalid command";
			}
		}	
	}
}

std::string CmdPathfinder::displayLevel(std::string error_message) {
	std::string input = displayLevel(error_message, 1);
	return input;
}

std::string CmdPathfinder::displayLevel(std::string error_message, int menu) {
	std::string input;
	std::string error;
	bool valid_option = false;
	while(!valid_option) {	
		error = error_message;
		system("CLS");
		for(int i=0;i < level.level.size();i++) {
			std::cout << level.level[i] + "\n";
		}
		std::cout << "\n";
		std::cout << error+"\n";
		if(menu == 1) {
			std::cout << "\n";
			std::cout << "Enter command (? for help): \n";
		} else if(menu == 2) {
			std::cout << "\n";
			std::cout << "Are you sure you want to exit the level (y/n)?: \n";
		}
		char key = _getch();
		error = castCharToStr(key);
		return error;
	}
}

void CmdPathfinder::runGame() {
	bool game_active = true;
	bool exit_flag = false;
	std::string error = "";
	game_active = generateLevel();
	if(!game_active) {
		system("CLS");
		menu = 0;
		std::cout << "Error loading level\n";
		std::cout << "\n";
		std::cout << "\n";
		std::cout << "Press enter to return to main menu ";
		std::string input = "";
		getline(std::cin, input);
		std::stringstream input_stream(input);

		menu = 0;
	} else {
		std::string player_char = "";
		player_char = castCharToStr(254);
		level.level[level.start_y].replace(level.start_x, 1, player_char);
		while(game_active) {
			if(!exit_flag) {	
				if(error == "finish") {
					bool complete = true;
					for(int i = 1;i < level.level.size() - 1;i++) {
						for(int j = 1; j < level.level[i].size() - 1;j++){
							if(level.level[i][j] == 'X') {
								complete = false;
							}
						}
					}
					if(complete) {
						menu = 2;
					} else {
						menu = 3;
					}
					game_active = false;
				} else if(error == "no_moves") {
					menu = 5;
					game_active = false;
				} else {
					std::string command = displayLevel(error);
					if(command == "w" || command == "a" || command == "s" || command == "d") {
						error = processMovement(command);
					} else if(command == "?") {
						displayInstructions();
					} else if(command == "e") {
						exit_flag = true;
					} else {
						error = "Invalid command";
					}
				}
			} else {
				std::string command = displayLevel(error, 2);
				if(command == "y") {
					menu = 0;
					game_active = false;
				} else if(command == "n") {
					exit_flag = false;
					error = "";
				} else {
					error = "Invalid command";
				}
			}
		}
	}
}

std::string CmdPathfinder::processMovement(std::string move) {
	std::string error = "";
	bool check_no_moves = false;
	int x = 0;
	int y = 0;
	if(move == "w") {
		y = level.player_y - 1;
		x = level.player_x;
	} else if(move == "a") {
		y = level.player_y;
		x = level.player_x - 1;
	} else if(move == "s") {
		y = level.player_y + 1;
		x = level.player_x;
	} else if(move == "d") {
		y = level.player_y;
		x = level.player_x + 1;
	} else {
		return "Invalid command";
	}
	
	std::string position = checkPosition(x,y);
	
	if(position == "S" || position == "O" || position == "*" || position == castCharToStr(176)) {
		error = "Invalid movement";
		check_no_moves = true;
	}
	else if(position == "X" || position == "E") {
		std::string player = castCharToStr(254);
		level.level[y].replace(x, 1, player);
		if(level.player_x == level.start_x && level.player_y == level.start_y) {
			level.level[level.player_y].replace(level.player_x, 1, "S");
		} else {
			level.level[level.player_y].replace(level.player_x, 1, "O");
		}		
		level.player_x = x;
		level.player_y = y;
		if(position == "E") {
			error = "finish";
		} else {
			check_no_moves = true;
		}
	}
	
	if(check_no_moves) {
		std::string up = checkPosition(level.player_x,level.player_y-1);
		std::string left = checkPosition(level.player_x-1,level.player_y);
		std::string down = checkPosition(level.player_x,level.player_y+1);
		std::string right = checkPosition(level.player_x+1,level.player_y);
		if(up == "*" || up == "S" || up == "O" || up == castCharToStr(176)) {
			if(left == "*" || left == "S" || left == "O" || left == castCharToStr(176)) {
				if(down == "*" || down == "S" || down == "O" || down == castCharToStr(176)) {
					if(right == "*" || right == "S" || right == "O" || right == castCharToStr(176)) {
						error = "no_moves";
					}
				}	
			}
		}
	}

	return error;
}

std::string CmdPathfinder::checkPosition(int x, int y) {
	std::string error = "";
	if((y >= 0 && y <= level.level.size()-1) && (x >= 0 && x <= level.level[y].size()-1)) {
		char position = level.level[y][x];
		error = castCharToStr(position);
	} else {
		error = "*";
	}
	return error;
}

bool CmdPathfinder::generateLevelLine(std::string line) {
	for(int i=0;i < line.size();i++)
	{
		if(line[i] != '*' && line[i] != 'S' && line[i] != 'E' && line[i] != 'X' && line[i] != 'O' && line[i] != (char)176 && line[i] != (char)254) {
			return false;
		}
	}
	level.level.push_back(line);
	return true;
}

bool CmdPathfinder::generateLevel() {
	clearLevel(level);
	bool valid = true;
	std::string new_line = "";
	std::vector<std::string> level_lines;
	switch(current_level) {
		case 0:
			level_lines.push_back("*******");
			level_lines.push_back("*XXXXX*");
			level_lines.push_back("*XXXXX*");
			level_lines.push_back("*XXXXX*");
			new_line = "*XXXX";
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			level_lines.push_back(new_line);
			level_lines.push_back("*XXXXX*");
			level_lines.push_back("*XXXXX*");
			level_lines.push_back("*******");
			level.start_x = 1;
			level.start_y = 7;
			level.player_x = 1;
			level.player_y = 7;
			level.end_x = 5;
			level.end_y = 1;
			break;
		case 1:
			level_lines.push_back("*******");
			level_lines.push_back("*XXXXE*");
			level_lines.push_back("*XXXXX*");
			level_lines.push_back("*XXXXX*");
			new_line = "*";
			new_line += (char)176;
			new_line += "XXXX*";
			level_lines.push_back(new_line);
			level_lines.push_back(new_line);
			level_lines.push_back("*XXXXX*");
			level_lines.push_back("*XXXXS*");
			level_lines.push_back("*******");
			level.start_x = 5;
			level.start_y = 7;
			level.player_x = 5;
			level.player_y = 7;
			level.end_x = 5;
			level.end_y = 1;
			break;
		case 2:
			level_lines.push_back("*******");
			level_lines.push_back("*XXXXE*");
			level_lines.push_back("*XXXXX*");
			level_lines.push_back("*XSXXX*");
			new_line = "*X";
			new_line += (char)176;
			new_line += "XXX*";
			level_lines.push_back(new_line);
			level_lines.push_back(new_line);
			new_line = "*XXX";
			new_line += (char)176;
			new_line += "X*";
			level_lines.push_back(new_line);
			level_lines.push_back("*XXXXX*");
			level_lines.push_back("*******");
			level.start_x = 2;
			level.start_y = 3;
			level.player_x = 2;
			level.player_y = 3;
			level.end_x = 5;
			level.end_y = 1;
			break;
		case 3:
			level_lines.push_back("*******");
			level_lines.push_back("*EXXXX*");
			level_lines.push_back("*XXXXX*");
			new_line = "*X";
			new_line += (char)176;
			new_line += "XXX*";
			level_lines.push_back(new_line);
			level_lines.push_back(new_line);
			level_lines.push_back(new_line);
			new_line = "*XXX";
			new_line += (char)176;
			new_line += "X*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += "SXXX*";
			level_lines.push_back(new_line);
			level_lines.push_back("*******");
			level.start_x = 2;
			level.start_y = 7;
			level.player_x = 2;
			level.player_y = 7;
			level.end_x = 1;
			level.end_y = 1;
			break;
		case 4:
			level_lines.push_back("*******");
			new_line = "*XXXX";
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			level_lines.push_back("*XXXXX*");
			new_line = "*X";
			new_line += (char)176;
			new_line += "XXX*";
			level_lines.push_back(new_line);
			level_lines.push_back(new_line);
			level_lines.push_back(new_line);
			new_line = "*XXE";
			new_line += (char)176;
			new_line += "X*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += "SXXX*";
			level_lines.push_back(new_line);
			level_lines.push_back("*******");
			level.start_x = 2;
			level.start_y = 7;
			level.player_x = 2;
			level.player_y = 7;
			level.end_x = 3;
			level.end_y = 6;
			break;
		case 5:
			level_lines.push_back("***********");
			new_line = "*";
			new_line += (char)176;
			new_line += "XXX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "XXX*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += "XXXXXXXX*";
			level_lines.push_back(new_line);
			new_line = "*XXXXX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "X";
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			new_line = "*XXXX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "XXX*";
			level_lines.push_back(new_line);
			level_lines.push_back("*XXXXXXXXX*");
			new_line = "*XXXX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "XX";
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += "XXXXXX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += "XXXX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += "XXXX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			level_lines.push_back("***********");
			
			level.start_x = 7;
			level.start_y = 4;
			level.player_x = 7;
			level.player_y = 4;
			level.end_x = 3;
			level.end_y = 5;
			break;
		case 6:
			level_lines.push_back("***********");
			new_line = "*XXX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "XXXX*";
			level_lines.push_back(new_line);
			new_line = "*X";
			new_line += (char)176;
			new_line += "XXXXXXX*";
			level_lines.push_back(new_line);
			level_lines.push_back("*XXXXXXXXX*");
			new_line = "*";
			new_line += (char)176;
			new_line += "XXXX";
			new_line += (char)176;
			new_line += "XX";
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += "XX";
			new_line += (char)176;
			new_line += "XXXXX*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += "XX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "XXXX*";
			level_lines.push_back(new_line);
			new_line = "*XX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += "X";
			new_line += (char)176;
			new_line += "XX*";
			level_lines.push_back(new_line);
			level_lines.push_back("*XXXXXXXXX*");
			new_line = "*";
			new_line += (char)176;
			new_line += "XX";
			new_line += (char)176;
			new_line += "XXXXX*";
			level_lines.push_back(new_line);
			level_lines.push_back("***********");

			level.start_x = 5;
			level.start_y = 3;
			level.player_x = 5;
			level.player_y = 3;
			level.end_x = 3;
			level.end_y = 6;
			break;
		case 7:
			level_lines.push_back("***********");
			new_line = "*";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "XX";
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			level_lines.push_back("*XXXXXXXXX*");
			level_lines.push_back("*XXXXXXXXX*");
			new_line = "*XXXXX";
			new_line += (char)176;
			new_line += "XXX*";
			level_lines.push_back(new_line);
			new_line = "*XXXXXXX";
			new_line += (char)176;
			new_line += "X*";
			level_lines.push_back(new_line);
			new_line = "*X";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "XXXXXX*";
			level_lines.push_back(new_line);
			new_line = "*XXXXX";
			new_line += (char)176;
			new_line += "XX";
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += (char)176;
			new_line += "XXXXXX";
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			new_line = "*";
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += (char)176;
			new_line += "*";
			level_lines.push_back(new_line);
			level_lines.push_back("***********");

			level.start_x = 3;
			level.start_y = 4;
			level.player_x = 3;
			level.player_y = 4;
			level.end_x = 6;
			level.end_y = 6;
			break;
		case 8:
			if(!custom_level_has_generated) {
				level_lines = generateCustomLevel(generate_level_x, generate_level_y);
				custom_level.level = level_lines;
				custom_level.start_x = level.start_x;
				custom_level.start_y = level.start_y;
				custom_level.player_x = level.player_x;
				custom_level.player_y = level.player_y;
				custom_level.end_x = level.end_x;
				custom_level.end_y = level.end_y;
				custom_level_has_generated = true;
			} else {
				level_lines = custom_level.level;
				level.start_x = custom_level.start_x;
				level.start_y = custom_level.start_y;
				level.player_x = custom_level.player_x;
				level.player_y = custom_level.player_y;
				level.end_x = custom_level.end_x;
				level.end_y = custom_level.end_y;
			}
			break;
	}
	for(int i=0;i < level_lines.size();i++) {
		valid = generateLevelLine(level_lines[i]);
		if(!valid) {
			if(custom_level_has_generated) {
				clearLevel(custom_level);
				custom_level_has_generated = false;
			}
			return false;
		}
	}
	level.level[level.start_y].replace(level.start_x, 1, "S");
	level.level[level.end_y].replace(level.end_x, 1, "E");

	return true;
}

std::vector<std::string> CmdPathfinder::generateCustomLevel(int maximum_x, int maximum_y) {
	bool level_complete = false;
	while(!level_complete) {
		std::string new_line = "";
		std::vector<std::string> level_lines;
		int max_x = maximum_x;
		int max_y = maximum_y;	
		int x_side_weight = 0;
		int y_side_weight = 0;

		level.start_x = 0;
		level.start_y = 0;
		level.player_x = 0;
		level.player_y = 0;
		level.end_x = 0;
		level.end_y = 0;

		for(int i=0;i<max_y+2;i++) {
			std::string new_line = "";
			for(int j=0;j<max_x+2;j++) {
				if(j == 0 || j == max_x+1) {
					new_line += "*";
				} else {
					if(i == 0 || i == max_y+1) {
						new_line += "*";
					} else {
						new_line += "X";
					}
				}
			}
			level_lines.push_back(new_line);
		}

		std::vector<int> random_rolls;
		int side_weight = randomNumber(2);
		x_side_weight = side_weight;

		for(int i=0;i<5;i++) {
			int roll = randomNumber(max_x);
			if(side_weight == 0 && roll > 0) {
				roll -= 1;
			} else if(side_weight == 1 && roll < max_x - 1) {
				roll += 1;
			}
			random_rolls.push_back(roll);
		}
		for(int i=0;i<random_rolls.size();i++) {
			level.start_x += random_rolls[i];
		}
		level.start_x = (level.start_x / random_rolls.size()) + 1;
		if(side_weight == 0 && level.start_x > 1) {
			level.start_x -= 1;
		} else if(side_weight == 1 && level.start_x < max_x) {
			level.start_x += 1;
		}

		random_rolls.clear();


		side_weight = randomNumber(2);
		y_side_weight = side_weight;
		for(int i=0;i<5;i++) {
			int roll = randomNumber(max_y);
			if(side_weight == 0 && roll > 0) {
				roll -= 1;
			} else if(side_weight == 1 && roll < max_y - 1) {
				roll += 1;
			}
			random_rolls.push_back(roll);
		}
		for(int i=0;i<random_rolls.size();i++) {
			level.start_y += random_rolls[i];
		}
		level.start_y = (level.start_y / random_rolls.size()) + 1;
		if(side_weight == 0 && level.start_y > 1) {
			level.start_y -= 1;
		} else if(side_weight == 1 && level.start_y < max_y) {
			level.start_y += 1;
		}
		random_rolls.clear();

		for(int i=0;i<5;i++) {
			int roll = randomNumber(max_x);
			if(x_side_weight == 1 && roll > 0) {
				roll -= 1;
			} else if(x_side_weight == 0 && roll < max_x - 1) {
				roll += 1;
			}
			random_rolls.push_back(roll);
		}
		for(int i=0;i<random_rolls.size();i++) {
			level.end_x += random_rolls[i];
		}
		level.end_x = (level.end_x / random_rolls.size()) + 1;
		if(x_side_weight == 1 && level.end_x > 1) {
			level.end_x -= 1;
		} else if(x_side_weight == 0 && level.end_x < max_x) {
			level.end_x += 1;
		}

		random_rolls.clear();
		for(int i=0;i<5;i++) {
			int roll = randomNumber(max_y);
			if(y_side_weight == 1 && roll > 0) {
				roll -= 1;
			} else if(y_side_weight == 0 && roll < max_y - 1) {
				roll += 1;
			}
			random_rolls.push_back(roll);
		}
		for(int i=0;i<random_rolls.size();i++) {
			level.end_y += random_rolls[i];
		}
		level.end_y = (level.end_y / random_rolls.size()) + 1;
		if(y_side_weight == 1 && level.end_y > 1) {
			level.end_y -= 1;
		} else if(y_side_weight == 0 && level.end_y < max_y) {
			level.end_y += 1;
		}


		if(level.end_x == level.start_x) {
			if(x_side_weight == 0) {
				if(level.end_x < max_x) {
					level.end_x += 1;
				} else {
					level.end_x -= 1;
				}
			} else {
				if(level.end_x > 1) {
					level.end_x -= 1;
				} else {
				level.end_x += 1;
				}
			}
		}
		if(level.end_y == level.start_y) {
			if(y_side_weight == 0) {
				if(level.end_y < max_y) {
					level.end_y += 1;
				} else {
					level.end_y -= 1;
				}
			} else {
				if(level.end_y > 1) {
					level.end_y -= 1;
				} else {
					level.end_y += 1;
				}
			}
		}

		level.player_x = level.start_x;
		level.player_y = level.start_y;

		for(int i=0;i<max_y;i++) {
			for(int j=0;j<max_x;j++) {
				int place_wall = randomNumber(15);
				if(place_wall == 14) {
					if((i+1 != level.start_y && j+1 != level.start_x) && (i+1 != level.end_y && j+1 != level.end_x)) {
						level_lines[i+1][j+1] = (char)176;
					}
				}
			}
		}

		int start = 0;
		int end = -5;
		int barrier = -4;
		int wall = -3;
		int empty = -2;
		int error = -1;
		int generated_level_lines[max_y+2][max_x+2];
		for(int i=0;i<max_y+2;i++) {
			for(int j=0;j<max_x+2;j++) {
				if(level_lines[i][j] == '*') {
					generated_level_lines[i][j] = barrier;
				} else if(level_lines[i][j] == (char)176) {
					generated_level_lines[i][j] = wall;
				} else if(i == level.start_y && j == level.start_x) {
					generated_level_lines[i][j] = start;
				} else if(i == level.end_y && j == level.end_x) {
					generated_level_lines[i][j] = end;
				} else if(level_lines[i][j] == 'X') {
					generated_level_lines[i][j] = empty;
				} else {
					generated_level_lines[i][j] = empty;
				}
			}
		}

		int current_x = level.start_x;
		int current_y = level.start_y;
		int last_x = level.start_x;
		int last_y = level.start_y;
		bool next_up = true;
		bool next_down = true;
		bool next_left = true;
		bool next_right = true;
		int min_moves = (max_x * max_y) * 0.6;
		int current_move = 0;
		std::vector<int> errors_x;
		std::vector<int> errors_y;
		std::vector<int> errors_move;
		std::vector<int> errors_iteration_count;
		int error_iteration_offest = max_x * max_y;

		bool pathfinding = true;

		//0 = left
		//1 = right
		//2 = up
		//3 = down

		std::string test_line = "";
		std::vector<std::string> test_map;
		std::vector<int> possible_directions;
		possible_directions.push_back(0);
		possible_directions.push_back(1);
		possible_directions.push_back(2);
		possible_directions.push_back(3);
		int directions_left = 4;
		int count = 1;

		while(pathfinding) {
			int direction = randomNumber(directions_left);
			//writeToFile("debug1.txt", "Direction: " + castIntToStr(direction));
			int new_x = 0;
			int new_y = 0;
			bool bad_direction = false;
			bool bad_move = false;
			bool good_move = false;
			if(possible_directions[direction] == 0) {
				new_x = current_x - 1;
				new_y = current_y;
			} else if(possible_directions[direction] == 1) {
				new_x = current_x + 1;
				new_y = current_y;
			} else if(possible_directions[direction] == 2) {
				new_x = current_x;
				new_y = current_y - 1;
			} else if(possible_directions[direction] == 3) {
				new_x = current_x;
				new_y = current_y + 1;
			}

			if(generated_level_lines[new_y][new_x] == end) {
				if(current_move+1 >= min_moves) {
					pathfinding = false;
					level_complete = true;
				} else {
					bad_direction = true;
				}
			} else if(generated_level_lines[new_y][new_x] == -1) {
				for(int i=0;i<errors_x.size();i++) {
					if(errors_x[i] == new_x && errors_y[i] == new_y) {
						if(errors_move[i] > current_move || errors_iteration_count[i] < count-error_iteration_offest) {
							errors_x.erase(errors_x.begin()+i);
							errors_y.erase(errors_y.begin()+i);
							errors_move.erase(errors_move.begin()+i);
							errors_iteration_count.erase(errors_iteration_count.begin()+i);
							good_move = true;
							break;
						} else {
							bad_direction = true;
						}
					}
				}
			} else if(generated_level_lines[new_y][new_x] == start || generated_level_lines[new_y][new_x] == wall || generated_level_lines[new_y][new_x] == barrier || generated_level_lines[new_y][new_x] > 0) {
				bad_direction = true;
			} else if(generated_level_lines[new_y][new_x] == empty) {
				good_move = true;
			}
	
			if(bad_direction) {
				if(possible_directions[direction] == 0) {
					next_left = false;
				} else if(possible_directions[direction] == 1) {
					next_right = false;
				} else if(possible_directions[direction] == 2) {
					next_up = false;
				} else if(possible_directions[direction] == 3) {
					next_down = false;
				}
				possible_directions.erase(possible_directions.begin()+direction);
				directions_left--;
			} else {
				good_move;
			}
	
			if(!next_up && !next_down && !next_left && !next_right) {
				bad_move = true;
			}
		
			if(bad_move) {
				if(current_move > 0) {
					generated_level_lines[current_y][current_x] = -1;
					current_move--;
					errors_x.push_back(current_x);
					errors_y.push_back(current_y);
					errors_move.push_back(current_move);
					errors_iteration_count.push_back(count);
					current_x = last_x;
					current_y = last_y;
					
					if(current_move > 0) {
						if(generated_level_lines[current_y+1][current_x] == current_move-1) {
							last_x = current_x;
							last_y = current_y+1;
						} else if(generated_level_lines[current_y-1][current_x] == current_move-1) {
							last_x = current_x;
							last_y = current_y-1;
						} else if(generated_level_lines[current_y][current_x+1] == current_move-1) {
							last_x = current_x+1;
							last_y = current_y;
						} else if(generated_level_lines[current_y][current_x-1] == current_move-1) {
							last_x = current_x-1;
							last_y = current_y;
						}
					} else {
						last_x = current_x;
						last_y = current_y;
					}
				}
				next_up = true;
				next_down = true;
				next_left = true;
				next_right = true;
				possible_directions.clear();
				possible_directions.push_back(0);
				possible_directions.push_back(1);
				possible_directions.push_back(2);
				possible_directions.push_back(3);
				directions_left = 4;
				
			} else if(good_move) {
				current_move++;
				last_x = current_x;
				last_y = current_y;
				current_x = new_x;
				current_y = new_y;
				generated_level_lines[current_y][current_x] = current_move;
				next_up = true;
				next_down = true;
				next_left = true;
				next_right = true;
				possible_directions.clear();
				possible_directions.push_back(0);
				possible_directions.push_back(1);
				possible_directions.push_back(2);
				possible_directions.push_back(3);
				directions_left = 4;
			}
	
			//writeToFile("debug1.txt", castIntToStr(count));
			for(int i=0;i<max_y+2;i++) {
				for(int j=0;j<max_x+2;j++) {
					if(generated_level_lines[i][j] >= 0 && generated_level_lines[i][j] <= 9) {
						test_line += " " + castIntToStr(generated_level_lines[i][j]);
					} else {
						test_line += castIntToStr(generated_level_lines[i][j]);
					}
				}
				test_map.push_back(test_line);
				test_line = "";
			}
			
			/*
			for(int i=0;i<test_map.size();i++) {
				writeToFile("debug1.txt", test_map[i]);
			}
			writeToFile("debug1.txt", "");
			*/

			test_map.clear();
	
			count++;
			if(count > (max_x * max_y) * 5) {
				pathfinding = false;
			}
		}	
	
		if(level_complete) {
			for(int i=1;i<max_y+1;i++) {
				for(int j=1;j<max_x+1;j++) {
					if(generated_level_lines[i][j] == empty || generated_level_lines[i][j] == error) {
						level_lines[i].replace(j, 1, castCharToStr((char)176));
					}
				}
			}
			return level_lines;
		} else {
			//writeToFile("debug1.txt", "\nFailed to generate a level before timeout\n");
		}	
	}
}

void CmdPathfinder::generationOptionsScreen() {
	bool ready_to_generate = false;
	int menu_screen = 0;
	std::string error = "";

	while(!ready_to_generate) {
		system("CLS");
		if(menu_screen == 2) {
			std::cout << "Please wait while level is generated...";
			ready_to_generate = true;
		} else {
			std::cout << "Level Generation Options";
			std::cout << "\n";
			std::cout << error+"\n";
			std::cout << "\n";
			std::cout << "\n";
			if(menu_screen == 0) {
				std::cout << "Please enter level width (min 5, max 9): ";
			} else if(menu_screen == 1) {
				std::cout << "Please enter level height (min 5, max 9): ";
			}

			error = "";
			char key = _getch();
			std::string input_string = castCharToStr(key);
			int option = castStrToInt(input_string);

			if(option >= 5 && option <= 9) {
				if(menu_screen == 0) {
					generate_level_x = option;
					menu_screen++;
				} else if(menu_screen == 1) {
					generate_level_y = option;
					menu_screen++;
				}
			} else {
				error = "Input must be a number between 5 and 9";
			}
		}
	}
	menu = 1;
}

void CmdPathfinder::completionScreen() {
	bool valid_option = false;
	
	while(!valid_option) {
		system("CLS");
		std::cout << "Congratulations you have completed the level!!\n";
		std::cout << "\n";
		std::cout << "\n";
		std::cout << "Press e to return to main menu ";

		char key = _getch();
		std::string input_string = castCharToStr(key);

		if(input_string == "e") {
			menu = 0;
			if(current_level != number_of_levels) {
				level_completion[current_level] = "Completed";
			}
			current_level = -1;
			if(custom_level_has_generated) {
				clearLevel(custom_level);
				custom_level_has_generated = false;
			}
			valid_option = true;
		}
	}
}

void CmdPathfinder::levelFailed(std::string error) {
	bool valid_option = false;
	
	while(!valid_option) {
		system("CLS");
		std::string option = displayLevel(error, 0);
		if(option == "r") {
			menu = 1;
			valid_option = true;
		} else if(option == "e") {
			if(current_level != number_of_levels && level_completion[current_level] != "Completed") {
				level_completion[current_level] = "Failed";
			}
			current_level = -1;
			menu = 0;
			if(custom_level_has_generated) {
				clearLevel(custom_level);
				custom_level_has_generated = false;
			}
			valid_option = true;
		}
	}
}

void CmdPathfinder::selectLevel() {
	bool valid_option = false;
	std::string error = "";
	std::string input_string = "";
	while(!valid_option) {
		system("CLS");
		for(int i=0;i < number_of_levels-1;i++) {
			std::cout << castIntToStr(i+1) + ". Level "+castIntToStr(i+1)+"\t"+level_completion[i]+"\n";  
		}
		std::cout << castIntToStr(number_of_levels) + ". Randomly Generate Level\n";
		std::cout << "\n";
		std::cout << error+"\n";
		std::cout << "\n";
		std::cout << "\n";
		std::cout << "Please select a level. Press e to return to main menu: ";
		
		char key = _getch();
		input_string = castCharToStr(key);
		int option = castStrToInt(input_string);
		
		for(int j=0;j < number_of_levels;j++) {
			if(option == j+1) {
				valid_option = true;
			}
		}

		if(valid_option) {
			if(option == number_of_levels) {
				menu = 6;
			} else {
				menu = 1;
			}
			current_level = option-1;
		} else if(input_string == "e"){
			menu = 0;
			valid_option = true;
		} else {
			error = "Invalid option";
		}
	}
}

void CmdPathfinder::displayInstructions() {
	std::string input;
	system("CLS");
	std::cout << "GAME INSTRUCTIONS\n";
	std::cout << "Get from the Start 'S' square to the finish 'E' square.\n";
	std::cout << "You must land on each X square once before you reach the finish.\n";
	std::cout << "You cannot land on an X square again once you have already landed on it.\n";
	std::cout << "Every O square denotes an X square you have already landed on.\n";
	std::cout << "You cannot land on the S square again after you have moved off it.\n";
	std::cout << (char) 176 << " squares denote squares that are impassable, you cannot land on these squares and must move around them.\n";
	std::cout << "* squares are the level barrier. You cannot move onto these squares.\n";
	std::cout << (char) 254 << " is your character. This shows your current position.\n";
	std::cout << "\n";
	std::cout << "GAME CONTROLS\n";
	std::cout << "Movement\n";
	std::cout << "Up: w\n";
	std::cout << "Down: s\n";
	std::cout << "Left: a\n";
	std::cout << "Right: d\n";
	std::cout << "\n";
	std::cout << "Miscellaneous \n";
	std::cout << "Help: ?\n";
	std::cout << "Exit to main menu: e\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "Press any key to exit help";

	char key = _getch();
	/*
	getline(std::cin, input);
	std::stringstream input_stream(input);*/
}

void CmdPathfinder::writeToFile(std::string filename, std::string write_line) {
	std::ofstream file_to_write;
  	file_to_write.open(filename.c_str(), std::ios::app);
  	file_to_write << write_line + "\n";
  	file_to_write.close();
}

int CmdPathfinder::randomNumber(int maxValue) {
	int random_number = 0;
	random_number = rand() % maxValue;
	return random_number;
}

std::string CmdPathfinder::castCharToStr(char to_string) {
	std::stringstream str_stream;
	std::string str = "";
	str_stream << to_string;
	str_stream >> str;
	return str;
}

std::string CmdPathfinder::castCharToStr(int to_string) {
	std::stringstream str_stream;
	std::string str = "";
	char ch = to_string;
	str_stream << ch;
	str_stream >> str;
	return str;
}

std::string CmdPathfinder::castIntToStr(int to_string) {
	std::stringstream str_stream;
	std::string str = "";
	str_stream << to_string;
	str_stream >> str;
	return str;
}

int CmdPathfinder::castStrToInt(std::string to_int) {
	std::stringstream str_stream;
	int as_int;
	str_stream << to_int;
	str_stream >> as_int;
	return as_int;
};