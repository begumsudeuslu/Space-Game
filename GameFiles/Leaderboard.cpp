#include "Leaderboard.h"

// Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
// variable will point to the highest all-times score, and all other scores will be reachable from it
// via the "next_leaderboard_entry" member variable pointer.
void Leaderboard::read_from_file(const string &filename) {
    // TODO: Your code here
    ifstream file(filename);
    if (!file)  {
        cerr << "file couldnt open: " << filename << endl;
        return;
    }

    string line;
    while(getline(file, line))  {
        size_t first_space = line.find(' ');
        size_t second_space = line.find(' ', first_space + 1);

        unsigned long score = stoul(line.substr(0, first_space));
        time_t last_played = static_cast<time_t>(stoll(line.substr(first_space  +1, second_space - first_space -1)));
        string player_name = line.substr(second_space+1);

        auto *new_entry = new LeaderboardEntry(score, last_played, player_name);

        new_entry->next=head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    }
    file.close();
}


// Write the latest leaderboard status to the given file in the format specified in the PA instructions
void Leaderboard::write_to_file(const string &filename) {
    // TODO: Your code here
    ofstream file(filename);
    if(!file.is_open()) {
        cerr << "file couldnt open: " << filename << endl;
        return;
    }

    LeaderboardEntry *current = head_leaderboard_entry;

    while(current != nullptr)  {
        file  << current->score << " " << current->last_played << " " << current->player_name << endl;
        current = current->next;
    }

    file.close();

}

// Print the current leaderboard status to the standard output in the format specified in the PA instructions
void Leaderboard::print_leaderboard() {
    // TODO: Your code here
    cout << "Leaderboard" << endl;
    cout << "-----------" << endl;

    LeaderboardEntry *current = head_leaderboard_entry;
    int count = 1;

    while(current != nullptr)  {
        char timestamp_buffer[20];
        struct tm* time_info = localtime(&current->last_played);
        strftime(timestamp_buffer, sizeof(timestamp_buffer), "%H:%M:%S/%d.%m.%Y", time_info);

        cout << count << ". "  << current->player_name << " " << current->score << " " << timestamp_buffer << endl;

        current = current->next;
        count ++;
    }
}

//  Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
//  is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
//  top 10 all-time high-scores should be kept in descending order by the score).
void Leaderboard::insert(LeaderboardEntry *new_entry) {
    // If the leaderboard is empty or the new score is higher than the first entry, insert at the top
    if (!head_leaderboard_entry || new_entry->score > head_leaderboard_entry->score) {
        new_entry->next = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        LeaderboardEntry *current = head_leaderboard_entry;

        // Traverse the leaderboard to find the correct position
        for (; current->next && (current->next->score > new_entry->score || current->next->score == new_entry->score); current=current->next) {
            //current = current->next;
        }

        // Insert the new entry
        new_entry->next = current->next;
        current->next = new_entry;
    }

    // Ensure the leaderboard size does not exceed 10 entries
    int count = 1;
    LeaderboardEntry *acurrent = head_leaderboard_entry;
    while (acurrent && acurrent->next) {
        if (count++ == 10) {
            // Remove the last entry if there are more than 10
            LeaderboardEntry *temp = acurrent->next;
            acurrent->next = nullptr;
            delete temp;  // Make sure to delete the entry to avoid memory leak
            break;
        }
        acurrent = acurrent->next;
    }
}

// Free dynamically allocated memory used for storing leaderboard entries
Leaderboard::~Leaderboard() {
    // TODO: Your code here    while(head_leaderboard_entry)  {
    while(head_leaderboard_entry)  {
        LeaderboardEntry *temp = head_leaderboard_entry;
        head_leaderboard_entry = head_leaderboard_entry->next;
        delete temp;
    }
}
