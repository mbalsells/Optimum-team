#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <cassert>
#include <time.h>
#include <chrono>
#include <random>
#include <set>

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
mt19937 rng; //Random generator 

vector <vector <player> > classified_players(4);
// Classified_players[position] has all the players that have that position,
// considering that the position ranges from 0 to 3 where 0 is goalkeeper,
// 1 is defense, 2 is midfielder and 3 is forward.

vector <int> formation; 
// Number of required players for each position.

int max_price_team; //our budget

vvi population; //the current population (each vector represents the positions of the players).

// Helper for the sort, whose arguments are an index, a vector and the lowest index to be ordered.
void insert(int i, vi& team, int k) {
    while (team[i] < team[i-1] and i > k) {
        swap(team[i], team[i-1]);
        --i;
    }
}

// Function to sort relatively small vectors, where insertion sort is more efficient.
void insertion_sort(int j, vi& team, int k) {
    for (int i = k; i < j; ++i) {
        insert(i, team, k);
    }
}

// Function to reorder the players in a team in lexicographical order within the same position.
void reorder(vi& team) {
    // Ordering the defense
    insertion_sort(formation[1] + 1, team, 1);
    // Ordering the midfield
    insertion_sort(formation[2] + formation[1] + 1, team, formation[1] + 2);
    // Ordering the forwards
    insertion_sort(formation[3] + formation[2] + formation[1] + 1, team, formation[2] + formation[1] + 2);
}

// Function which given an index, returns the index of the player's position:
// 0 for goalkeeper, 1 for defense, 2 for midfielder and 3 for forward.
int position(int i) {
    if (i == 0) return 0;
    if (i < formation[1] + 1) return 1;
    if (i < formation[1] + formation[2] + 1) return 2;
    return 3;
}

int team_points(const vi& team){
    int ans = 0;

    for (int i = 0; i < 11; ++i){
        ans += classified_players[position(i)][team[i]].points;  
    }
    return ans;
}

int team_price(const vi& team){
    int ans = 0;

    for (int i = 0; i < 11; ++i){
        ans += classified_players[position(i)][team[i]].price;  
    }
    return ans;
}

// Given the document in which the output will be written and a
// team, writes the team in the document with the required format.
void print_solution(vi& team){
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
            document << member.name;
        }
        document << endl;
    }

    document << "Punts: " << team_points(team) << endl;
    document << "Preu: " << team_price(team) << endl;
    document.close();
}

// Function used to sort teams with descending order with respect to the points.
bool comp(const vi& team1, const vi& team2){
    return team_points(team1) > team_points(team2);
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
    if (team_price(team) > max_price_team) return false;

    set <pair <int, int> > taken_players;

    for (int i = 0; i < 11; ++i){
        int pos = position(i);
        int index = team[i];

        if (taken_players.find({pos, index}) != taken_players.end()) return false;
        taken_players.insert({pos, index});
    }
    return true;
}

//Adds a random valid permutation
vi add_random(){
    vi newteam;

    for (int i = 0; i < 11; ++i){
        newteam.push_back(rng() % classified_players[position(i)].size()); 
    }

    if (not valid_permutation(newteam)) return add_random();
    return newteam;
}

// Auxiliary function to the crossover function, which takes in two players and randomly outputs
// one of the two players to choose said player in the recombination. The probability of choosing 
// each player is a function of their points and prices.
bool coin_flip(const player& p1, const player& p2) {
    double p = p1.points/(1 + p1.price);
    double T = p + p2.points/(1 + p2.price);
    return (p/T > (double (rng() % INF)/INF)); 
}

// Function to execute the crossing between 2 teams, returning an offspring team.
vi crossover(vi& T1, vi& T2){
    vi new_team(11);
    for (int i = 0; i < 11; ++i) {
        // We find the players associated with each index in order to compare them.
        player p1 = classified_players[position(i)][T1[i]];
        player p2 = classified_players[position(i)][T2[i]];
        if (coin_flip(p1, p2)) new_team[i] = T1[i];
        else new_team[i] = T2[i];
    }
    return new_team;
}

//returns a mutation (or not) of a team.
vi mutation(vi& team){
    double p = 0.15;

    vi mutate = team;

    for (int i = 0; i < 11; ++i){
        double random = (double (rng() % INF)/INF);
        if (random < p) mutate[i] = rng() % classified_players[position(i)].size(); 
    }

    return mutate;
}


void solve(){
    population.clear();
    int max_points = 0;
    int population_size = 5e3;

    //while (true){ // or in general time < 1 minute or while there has been no improvement in Y iterations
        while (population.size() < population_size) population.push_back(add_random());
        sort(population.begin(), population.end(), comp);
        
        if (team_points(population.front()) > max_points) {
            max_points = team_points(population.front());
            print_solution(population.front());
        }

        vvi new_population;

        for (int i = 0; i < 500; ++i) {
            if (i == 0 or new_population.back() != population[i]){
                new_population.push_back(population[i]);
            }
        }

        for (int i = 0; i < 500; ++i){
            vi mutate = mutation(population[i]);
            if (valid_permutation(mutate)) new_population.push_back(mutate);
        }
        
        for (int i = 0; i < 100; ++i){
            for (int j = 0; j < 100; ++j){
                if (i == j) continue;

                vi child = crossover(population[i], population[j]);
                if (valid_permutation(child)) new_population.push_back(child);
            }
        }

        population = new_population;
    //}
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
    rng = mt19937(chrono::steady_clock::now().time_since_epoch().count());
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
