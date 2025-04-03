#include "GameController.h"

// Simply instantiates the game
GameController::GameController(
        const string &space_grid_file_name,
        const string &celestial_objects_file_name,
        const string &leaderboard_file_name,
        const string &player_file_name,
        const string &player_name

) {
    game = new AsteroidDash(space_grid_file_name, celestial_objects_file_name, leaderboard_file_name, player_file_name,
                            player_name);
    // TODO: Your code here, if you want to perform extra initializations
}

// Reads commands from the given input file, executes each command in a game tick
void GameController::play(const string &commands_file) {
    // TODO: Your code here
    ifstream file(commands_file);
    if(!file.is_open()) {
        cerr << "couldnt open command file: " << commands_file << endl;
        return;
    }

    string command;
    while(file>>command)   {
        game->game_time++;
        game->update_space_grid();
        game->print_space_grid();

        if(game->game_over)  {
            cout <<  "Game over!" << endl;
            game->print_space_grid();
            game->leaderboard.print_leaderboard();
            break;
        }

        if(command == "PRINT_GRID")   {
            game->print_space_grid();
        } else if (command == "MOVE_UP")   {
            game->player->move_up();
        } else if(command=="MOVE_DOWN")   {
            game->player->move_down(21);
        } else if (command == "MOVE_RIGHT")  {
            game->player->move_right(30);
        } else if(command=="MOVE_LEFT")   {
            game->player->move_left();
        } else if (command == "SHOOT")  {
            game->shoot();
        } else if(command== "NOP") {

        } else  {
            cerr << "Unkown command: " << command << endl;
        }
    }
    
    if (!game->game_over)  {
        cout <<  "GAME FINISHED! No more commands!" << endl;
        game->print_space_grid();
        game->leaderboard.print_leaderboard();
    }

}

// Destructor to delete dynamically allocated member variables here
GameController::~GameController() {
    // TODO: Your code here
    if (game!=nullptr)   {
        delete game;
        game = nullptr;
    }
}
