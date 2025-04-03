#include "AsteroidDash.h"



// Constructor to initialize AsteroidDash with the given parameters
AsteroidDash::AsteroidDash(const string &space_grid_file_name,
                           const string &celestial_objects_file_name,
                           const string &leaderboard_file_name,
                           const string &player_file_name,
                           const string &player_name)

        : leaderboard_file_name(leaderboard_file_name), leaderboard(Leaderboard()) {

    read_player(player_file_name, player_name);  // Initialize player using the player.dat file
    read_space_grid(space_grid_file_name);  // Initialize the grid after the player is loaded
    read_celestial_objects(celestial_objects_file_name);  // Load celestial objects
    leaderboard.read_from_file(leaderboard_file_name);

    current_score = 0;
    game_time = 0;
    game_over = false;
}

// Function to read the space grid from a file
void AsteroidDash::read_space_grid(const string &input_file) {
    // TODO: Your code here
    ifstream file(input_file); 
    if (!file.is_open()) { 
        cerr << "Error opening file: " << input_file << endl;
        return;
    }

    int value;
    vector<int> row;  

    while (file >> value) {
        row.push_back(value); 

        if (row.size() == 24) {
            space_grid.push_back(row);  
            row.clear();  
        }
    }

    file.close();

}

// Function to read the player from a file
void AsteroidDash::read_player(const string &player_file_name, const string &player_name) {
    // TODO: Your code here
        ifstream file(player_file_name);
    if(!file.is_open())  {
        cerr << "error couldnt open player file: " << player_file_name << endl;
    }

    int start_row, start_col;
    file >> start_row >> start_col;
    file.ignore();

    vector<vector<bool>> spacecraft_shape;
    string line;

    while(getline(file, line))  {
        vector<bool> row;

        for(char c: line)    {
            if (c=='1')  {
                row.push_back(true);
            } else if (c=='0')   {
                row.push_back(false);
            }   
        }
        if (!row.empty())   {
            spacecraft_shape.push_back(row);
        } 
    }

    file.close();

    if (player != nullptr)   {
        delete player;
    }

    int max_ammo = 10;
    int lives = 3;
    player = new Player(spacecraft_shape, start_row, start_col, player_name, max_ammo, lives);

}


vector<vector<bool>> AsteroidDash::rotate_shape(const vector<vector<bool>> &shape)   {
    int row = shape.size();
    int col = shape[0].size();
    vector<vector<bool>> rotated(col, vector<bool>(row));

    for (int i = 0; i<row; ++i)     {
        for (int j = 0; j<col; ++j)    {
            rotated[j][row-1-i] = shape [i][j];
        }
    }

    return rotated;
}


// Function to read celestial objects from a file
void AsteroidDash::read_celestial_objects(const string &input_file) {
    // TODO: Your code here
std::ifstream file(input_file);
    if (!file.is_open())   {
        std::cerr << "error file cannot open " << input_file << std::endl;
        return;
    }

    CelestialObject *last_object = nullptr;
    std::string line;

    while (std::getline(file, line))    {
        if (line.empty()) continue;

        char bracket_type = line[0];
        bool is_power_up =  (bracket_type == '{');
        std::vector<std::vector<bool>> shape;

        do {
            std::vector<bool> shape_row;
            for (char c: line)    {
                if (c == '1')    {
                    shape_row.push_back(true);
                }   else if (c=='0')    { 
                    shape_row.push_back(false);    
                }
            }
            if (!shape_row.empty())   {
                shape.push_back(shape_row);
            } 
        }  while (std::getline(file, line) && (line[0]=='1' || line[0] == '0'));
        
        int start_row = -1, time_of_appearance = -1;
        std::string effect = "";

        if (line.substr(0, 2) == "s:") {
            start_row = std::stoi(line.substr(2));
        }

        std::getline(file,line);

        if (line.substr(0,2) == "t:")   {
            time_of_appearance = std::stoi(line.substr(2));
        } 

        if (is_power_up && std::getline(file,line) && line.substr(0,2) == "e:")   {
            effect =line.substr(2);
        }

        ObjectType type;
        if (is_power_up)    {
            type = (effect == "life") ? ObjectType::LIFE_UP : ObjectType::AMMO;
        }  else   {
            type = ObjectType::ASTEROID;
        }

        CelestialObject *new_object = new CelestialObject(shape, type, start_row, time_of_appearance);

        CelestialObject *current_rotation = new_object;

        while(true)  {
            vector<vector<bool>> rotated = rotate_shape(current_rotation -> shape);

            if (rotated == new_object->shape)  {
                break;
            }

            CelestialObject *rotation = new CelestialObject(rotated, type, start_row, time_of_appearance);

            current_rotation->right_rotation = rotation;
            rotation->left_rotation = current_rotation;

            current_rotation = rotation;
        }  

        current_rotation->right_rotation = new_object;
        new_object->left_rotation = current_rotation;

        if (celestial_objects_list_head == nullptr)    {
            celestial_objects_list_head = new_object;
        }  else  {
            last_object->next_celestial_object = new_object;
        }
        last_object = new_object;
    }
    file.close();
}


// Print the entire space grid
void AsteroidDash::print_space_grid() const {
    // TODO: Your code here
    cout << "Tick: " << game_time << endl;
    cout << "Lives: " << player->lives << endl;
    cout << "Ammo: " << player->current_ammo << endl;
    cout << "Score: " << current_score << endl;
    if (leaderboard.head_leaderboard_entry!=nullptr) {
        cout << "High Score: " << leaderboard.head_leaderboard_entry->score << endl;
    } else {
        cout << "High Score: " << 0 << endl;
    }

    for (const auto& row : space_grid) { 
        for (const auto& cell : row) { 
           
            if (cell == 0) {
                cout << unoccupiedCellChar; 
            } else {
                cout << occupiedCellChar;
            }
        }
        cout << endl; 
    }
}

bool AsteroidDash::check_collision_with_celestial(Projectile *projectile, CelestialObject *celestial)   {
    int proj_row = projectile->proj_row;
    int proj_col = projectile->proj_col;

    int celestial_row = celestial->starting_row;
    int celestial_col = 30-(game_time - celestial->time_of_appearance);

    for (int i = 0;i < celestial->shape.size(); i++)    {
        for (int j = 0; j<celestial->shape[0].size(); j++)   {

            if (celestial->shape[i][j])   {
                int grid_row = celestial_row + i; 
                int grid_col = celestial_col + j;

                if(proj_row == grid_row && proj_col == grid_col)   {
                    return true;
                }
            }  
        }
    }
    return false;
}

bool AsteroidDash::check_collision_with_player(CelestialObject* celestial)   {
    int player_row = player->position_row;
    int player_col = player->position_col;

    int celestial_row = celestial->starting_row;
    int celestial_col = 30- (game_time - celestial->time_of_appearance);

    for (int i = 0; i < celestial->shape.size(); i++) {
        for (int j = 0; j < celestial->shape[i].size(); j++) {
           
            if (celestial->shape[i][j]) {
                int grid_row = celestial_row + i;
                int grid_col = celestial_col + j;
                
                if (grid_row >= player_row && grid_row < player_row + player->spacecraft_shape.size() &&
                    grid_col >= player_col && grid_col < player_col + player->spacecraft_shape[0].size()) {
                    return true;  
                }
            }
        }
    }
    return false;
    
}


// Function to update the space grid with player, celestial objects, and any other changes
// It is called in every game tick before moving on to the next tick.
void AsteroidDash::update_space_grid() {
    // TODO: Your code here
}

// Corresponds to the SHOOT command.
// It should shoot if the player has enough ammo.
// It should decrease the player's ammo
void AsteroidDash::shoot() {
    // TODO: Your code here
    if (player->current_ammo > 0)   {
        player->current_ammo--;

        int proj_row = player->position_row + player->spacecraft_shape.size()/2;
        int proj_col = player->position_col + player->spacecraft_shape[0].size();

        Projectile* new_proj = new Projectile(proj_row, proj_col);
        
        if (proj_row >= 0 && proj_row < space_grid.size() && proj_col >= 0 && proj_col < space_grid[0].size())  {
            Projectile new_proj(proj_row, proj_col);
            projectiles.push_back(new_proj);
        }
    }
}

// Destructor. Remove dynamically allocated member variables here.
AsteroidDash::~AsteroidDash() {
    // TODO: Your code here
    if (player == nullptr)   {
        delete player;
    }
}