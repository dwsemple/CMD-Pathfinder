#pragma once
#include <iostream>
#include <cstdlib>
#include <climits>
#include <sstream>
#include <string>
#include <vector>
#include <conio.h>
#include <time.h>
#include <fstream>

struct Level {
	std::vector<std::string> level;
	int start_x;
	int start_y;
	int player_x;
	int player_y;
	int end_x;
	int end_y;
};

class CmdPathfinder {

private:
	bool running;
	int menu;
	Level level;
	int number_of_levels;
	std::vector<std::string> level_completion;
	int current_level;
	Level custom_level;
	bool custom_level_has_generated;
	int generate_level_x;
	int generate_level_y;

public:
	CmdPathfinder();

	bool getRunning();
	int getMenu();
	void clearLevel(Level& clear_level);

	void setRunning(bool will_run);

	void displayMainMenu();

	void runGame();

	std::string displayLevel(std::string error);
	std::string displayLevel(std::string error, int menu);

	std::string processMovement(std::string move);

	std::string checkPosition(int x, int y);

	void displayInstructions();

	bool generateLevelLine(std::string line);

	bool generateLevel();

	std::vector<std::string> generateCustomLevel(int maximum_x, int maximum_y);	

	void generationOptionsScreen();

	void completionScreen();

	void levelFailed(std::string error);

	void selectLevel();

	int randomNumber(int maxValue);

	void writeToFile(std::string filename, std::string write_line);

	std::string castCharToStr(char to_string);
	std::string castCharToStr(int to_string);
	std::string castIntToStr(int to_string);
	int castStrToInt(std::string to_int);
};