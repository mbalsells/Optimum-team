#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <ctime>

using namespace std;

struct player {
    string name, position, club;
    int price, points;
};

typedef vector<int> VI;
typedef vector<player> players;

// Data structures used.
string outputFile;
players best_team(11);
players everyone;
vector<players> ordered_players(4); // Players separated by their positions.
int Budget, Max_Price, Money_Spent = 0, Max_Points = -1;
VI formation(4); // Team formation consisting of 10 players plus a goalie.

const int INF = 2e9;

// Vector containing the possible positions of a player.
vector <string> encode = {"por", "def", "mig", "dav"};


// Checks the position of a player, and if it matches a given position, it gets added 
// into the matrix of ordered players in its corresponding category.
void position_checker (const player& p, const int i, const string& position, const int Max_Price) {
    if (p.position == position) {
        if (p.price < Max_Price) {
            ordered_players[i].push_back(p);
        }
    }
}


// Function that separates the players by the position they play, which is useful for 
// their handling in the backtracking function later on.
void player_position_separator (int Max_Price) {
    for (player& p : everyone) {
        for (int i = 0; i < 4; ++i) position_checker(p, i, encode[i], Max_Price);
    }
}


// Function that returns the sum of the points of a teams' players.
int team_points (const players& T) {
    int sum = 0;
    for (const player& p : T) sum += p.points;
    return sum;
}


// Backtracking function to generate all possible valid teams.
void Find_Best_Team_ES (int category, int k, int Money_Spent, VI& formation, players& current_team, const int Budget, const int Max_Price) {
	// If the team is complete and valid, check whether it improves upon the previously found team.
    if (category == 4) {
        int current_points = team_points(current_team);
        if (current_points > Max_Points) {
            best_team = current_team;
            Max_Points = current_points;
        }
        return;
    }
    // After checking for all the players in a category, we move onto the next category.
    if (k == ordered_players[category].size()) {
    	// Check for a complete squad that uses up all the places in a category.
    	if (formation[category] != 0) {
    		return;
    	}
        Find_Best_Team_ES(category + 1, 0, Money_Spent, formation, current_team, Budget, Max_Price);
        return;
    }
    if (Money_Spent < Budget) {
    	if (formation[category] == 0) Find_Best_Team_ES(category + 1, 0, Money_Spent, formation, current_team, Budget, Max_Price);
    	if (ordered_players[category][k].price < Max_Price) {
    		// We add a player to our team, increase the money spent and continue onwards.
    		current_team.push_back(ordered_players[category][k]);
            --formation[category];
			Money_Spent += ordered_players[category][k].price;
    		Find_Best_Team_ES(category, k + 1, Money_Spent, formation, current_team, Budget, Max_Price);
    		// We don't add a player to our team, and continue onwards.
    		current_team.pop_back();
            ++formation[category];
    		Money_Spent -= ordered_players[category][k].price;
    	}
        Find_Best_Team_ES(category, k + 1, Money_Spent, formation, current_team, Budget, Max_Price);
    }    
}


// Function to calculate the money spent on a team.
int Team_Price (const players& team) {
    int sum = 0;
    for (const player& p : team) sum += p.price;
    return sum;
}


// Function to print the players in a team.
void print_team (const players& team) {
	cout << "POR: " << team[0].name << endl;
	cout << "DEF: ";
	for (int i = 0; i < formation[1]; ++i) {
		cout << team[1 + i].name << ';';
	}
	cout << endl << "MIG: ";
	for (int i = 0; i < formation[2]; ++i) {
		cout << team[1 + formation[1] + i].name << ';';
	}
	cout << endl << "DAV: ";
	for (int i = 0; i < formation[3]; ++i) {
		cout << team[1 + formation[1] + formation[2] + i].name << ';';
	}
	cout << endl << "Punts: " << Max_Points << endl;
    cout << "Preu:" << Team_Price(best_team) << endl;
}


int main (int argc, char** argv) {
    if (argc != 2) {
        cout << "Syntax: " << argv[0] << " data_base.txt" << endl;
        exit(1);
    }
    ifstream in(argv[1]);
    while (not in.eof()) {
        string nom, posicio, club;
        int punts, preu;
        getline(in,nom,';');    if (nom == "") break;
        getline(in,posicio,';');
        in >> preu;
        char aux; in >> aux;
        getline(in,club,';');
        in >> punts;
        string aux2;
        getline(in,aux2);
        everyone.push_back({nom, posicio, club, preu, punts});
    }
    in.close();
    int N1, N2, N3;
    cin >> N1 >> N2 >> N3 >> Budget >> Max_Price;
    formation = {1, N1, N2, N3};
    players current_team;
	player_position_separator(Max_Price);
	Find_Best_Team_ES(0, 0, 0, formation, current_team, Budget, Max_Price);
	print_team(best_team);
}
