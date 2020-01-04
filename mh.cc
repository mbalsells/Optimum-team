#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <cassert>
#include <time.h>
#include <chrono>
#include <random>

using namespace std;
typedef vector <int> vi; 
typedef vector <vi> vvi;

const int INF = 1e9;

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

int max_price_team; //our budget

vvi population; //the current population (each vector represents the positions of the players).


// Given the document in which the output will be written and a
// team, writes the team in the document with the required format.
void print_solution(vector <int>& team){
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
            player member = classified_players[pos][team[index++]];
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

// Function which given an index, returns the index of the player's position:
// 0 for goalkeeper, 1 for defense, 2 for midfielder and 3 for forward.
int position(int i) {
    if (i == 0) return 0;
    if (i < N1 + 1) return 1;
    if (i < N1 + N2 + 1) return 2;
    return 3;
}

// Function used to sort players with descending order with respect to the
// points and ascending with respect to the price and name (in this order). 
bool comp(const player& p1, const player& p2){
    if (p1.points != p2.points) return p1.points > p2.points;
    if (p1.price != p2.price) return p1.price < p2.price;
    return p1.name < p2.name;
}

// Function that separates the players by the position they play in.
// We only take those players whose price is not above Max_Price.
void player_position_separator (const vector <player>& players, const int Max_Price) {
    map <string, int> classifier = {{"por", 0}, {"def", 1}, {"mig", 2}, {"dav", 3}};

    for (const player& p : players) {
        if (p.price > Max_Price) continue;

        int pos = classifier[p.position];
        classified_players[pos].push_back(p);
    }
}

//checks whether the cost of a team is below our budget.
bool valid_permutation(vi& team){


}

//Adds a random valid permutation
vi add_random(){
    vi newteam;
    for (int i = 0; i < formation.size(); ++i){
        for (int j = 0; j < formation[i]; ++j){
            newteam.push_back(rng() % classifier[i].size()); 
        }
    }

    if (not valid_permutation(newteam)) return add_random;
    return newteam;
}

// Auxiliary function to the crossover function, which takes in two players and randomly outputs
// one of the two players to choose said player in the recombination. The probability of choosing 
// each player is a function of their points and prices.
int coin_flip(const player& p1, const player& p2) {
    double p = p1.points/p1.price;
    double T = p + p2.points/p2.price;
    if (p/T > /* Sample from U(0, 1) */) return p1
    return p2;
}

// Function to execute the crossing between 2 teams, returning an offspring team.
vi crossover(vi& T1, vi& T2){
    vi new_team(11);
    for (int i = 0; i < 11; ++i) {
        // We find the players associated with each index in order to compare them.
        player p1 = classified_players[position(i1)][T1[i]], p2 = classified_players[position(i2)][T2[i]];
        new_team[i] = coin_flip(p1, p2);
    }
    return new_team;
}

//returns a mutation (possibly more than one) of a team.
vi mutation(vi& team){
    double p = 0.15;

    vi mutate = team;
    int sum = 0;
    for (int i = 0; i < formation.size(); ++i){
        for (int j = 0; j < formation[i]; ++j){
            //int points = classified_players[i][team[sum++]].points;
            
            double random = (double (rng() % INF)/INF);
            if (random < p) mutate[sum] = rng() % classifier[i].size(); 
            ++sum;
        }
    }
    
    return mutate;
}


void solve(int max_price){
    initialize population 2000
    int max_points = 0;

    while (true){ // or in general time < 1 minute or while there has been no improvement in Y iterations
        while (population < size) add_random

        sort by points
        if best one has more than max_points keep those points and overwrite answer

        get best X and no repeats
        add to new vector
        add all mutations
        add crosover top 100
    }
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

// Given the input file of the test case and all the players, the function
// returns the optimum team, i.e. the one that satisfies all the
// constraints and has the highest total score. Also, out of all the ones
// that share such a score, we break ties by taking the cheapest one.
void get_solution(string input_file, const vector <player>& players){
    ifstream in(input_file);
    int N1, N2, N3, max_price_player;
    in >> N1 >> N2 >> N3 >> max_price_team >> max_price_player;
    in.close();

    formation = {1, N1, N2, N3};
    player_position_separator(players, max_price_player); //Separate players by position and
                                                          //elimiante those below the threshold.
    
    solve(); //Solve the problem.
}

int main(int argc, char** argv) {
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
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
