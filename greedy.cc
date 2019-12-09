#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <cassert>
#include <time.h>

using namespace std;
typedef vector <int> vi; 
typedef vector <vi> vvi; 
typedef vector <vvi> vvvi; 
typedef vector <vvvi> vvvvi;
typedef vector <vvvvi> vvvvvi;

const int INF = 2e9;

struct player {
    string name, position, club;
    int price, points;          
};

string output_file; //Name of the output file
clock_t initial_time; //Time when the program started


vector <vector <player> > classified_players(4);
// Classified_players[position] has all the players that have that position,
// considering that the position ranges from 0 to 3 where 0 is goalkeeper,
// 1 is defense, 2 is midfielder and 3 is forward.

vector <int> formation; 
// Number of required players for each position.


bool comp(const player& p1, const player& p2){
    if (p1.points != p2.points) return p1.points > p2.points;
    if (p1.price != p2.price) return p1.price < p2.price;
    return p1.name < p2.name;
}

bool order_position(const player& p1, const player& p2){
    map <string, int> classifier = {{"por", 0}, {"def", 1}, {"mig", 2}, {"dav", 3}};

    if (classifier[p1] != classifier[p2]) return classifier[p1] < classifier[p2];
    if (p1.points != p2.points) return p1.points > p2.points;
    if (p1.price != p2.price) return p1.price < p2.price;
    return p1.name < p2.name;
}


// Given the document in which the output will be written and a
// team, writes the team in the document with the required format.
void print_solution(vector <int>& team){
    sort(team.begin(), sort.end(), order_position());
    vector <string> encode = {"POR", "DEF", "MIG", "DAV"};
    
    int points = 0, price = 0; //Total points and price of the team.

    ofstream document;
    document.open(output_file);
    document.setf(ios::fixed); //One decimal for the execution time.
    document.precision(1);

    document << double(clock() - initial_time)/CLOCKS_PER_SEC << endl;

    
    int index = 0;
    for (int pos = 0; pos < 4; ++pos){
        document << encode[pos] << ": ";

        for (int j = 0; j < formation[pos]; ++j){
            if (j > 0) document << ";";

            //Current player.
            player member = team[index++];
            points += member.points;
            price += member.price;

            document << member.name;
        }
        document << endl;
    }

    document << "Punts: " << points << endl;
    document << "Preu: " << price << endl;
    document.close();
}

void initialize(const vector <player>& players, int N1, int N2, int N3, int max_price){
    formation = {1, N1, N2, N3};
    
    player_position_separator(players, max_price); //separate players by position
}

// Given the maximum_price that a team can have, and having initialized all the
// global variables, this function finds the best price with which all possible scores can
// be obtained, overwiting the best solution found if a better one comes up.
void greedy(vector <player>& players, int budget){
    map <string, int> classifier = {{"por", 0}, {"def", 1}, {"mig", 2}, {"dav", 3}};

    vector <int> formed(4, 0);
    vector <player> ans;
    for (int i = 0; i < players.size(); ++i){
        player current = players[i];
        int position = classifier[current.pos];

        if (budget >= current.price) continue;
        if (formed[position] == formation[position]) return;
            
        budget -= current.price;
        ++formed[position];
        ans.push_back(ans);
    }

    print_solution(ans);
}


// Given the input file of the test case and all the players, the function
// returns the optimum team, i.e. the one that satisfies all the
// constraints and has the highest total score. Also, out of all the ones
// that share such a score, we break ties by taking the cheapest one.
void get_solution(string input_file, const vector <player>& players){
    ifstream in(input_file);
    int N1, N2, N3, max_price_team, max_price_player;
    in >> N1 >> N2 >> N3 >> max_price_team >> max_price_player;
    in.close();

    initialize(players, max_price_player); //Initialize variables.
    greedy(players, max_price_team); //Solve the problem.
}

// Given the file with the data of the football players, the function returns
// all the players in the database.
vector <player> read_data(string input_file){
    vector <player> players;

    ifstream in(input_file);
    while (not in.eof()) {
        string nom, posicio, club;
        int punts, preu;
        getline(in,nom,';');        if (nom == "") break;
        getline(in,posicio,';');
        in >> preu;
        char aux; in >> aux;
        getline(in,club,';');
        in >> punts;
        string aux2;
        getline(in,aux2);
        players.push_back({nom, posicio, club, preu, punts});
    }  

    return players;
}

int main(int argc, char** argv) {
    initial_time = clock();

    if (argc != 4) {
        cout << "3 arguments required!" << endl;
        cout << "the data_base, the test_case and the output file" << endl;
        exit(1);
    }

    output_file = argv[3];

    vector <player> players = read_data(argv[1]);
    get_solution(argv[2], players); 
}