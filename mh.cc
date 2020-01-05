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

string output_file; //Name of the output file.
clock_t initial_time; //Time when the program started.
mt19937 rng; //Random number generator.

vector <vector <player> > classified_players(4);
// Classified_players[position] has all the players that have that position,
// considering that the position ranges from 0 to 3 where 0 is goalkeeper,
// 1 is defense, 2 is midfielder and 3 is forward.

vector <int> formation; 
// Number of required players for each position.

int max_price_team; //our budget

vvi population; 
// This is the current population, each vector represents a team ("individual").
// A team is represented as a vector of integers: the first formation[0] integers are
// the indices that the goalkeepers have in the classified_players[0] vector. the next
// formation[1] integers are the indices that the defenses have in the 
// classified_players[1] vector, and so on.


// Function which given the index that a player has in a team, returns the index of the player's 
// position: 0 for goalkeeper, 1 for defense, 2 for midfielder and 3 for forward.
int position(int i) {
    if (i == 0) return 0;
    if (i < formation[1] + 1) return 1;
    if (i < formation[1] + formation[2] + 1) return 2;
    return 3;
}

// Function which given a team returns the number of points of the whole team.
int team_points(const vi& team){
    int ans = 0;

    for (int i = 0; i < 11; ++i){
        ans += classified_players[position(i)][team[i]].points;  
    }
    return ans;
}

// Function which given a team returns the price of the whole team.
int team_price(const vi& team){
    int ans = 0;

    for (int i = 0; i < 11; ++i){
        ans += classified_players[position(i)][team[i]].price;  
    }
    return ans;
}


// Helper for the sort, whose arguments are an index, a vector and the lowest index to be ordered.
void insert(int i, vi& team, int k) {
    while (team[i] < team[i-1] and i > k) {
        swap(team[i], team[i-1]);
        --i;
    }
}

// Function to sort relatively small vectors, where insertion sort is more efficient.
void insertion_sort(int k, vi& team, int j) {
    for (int i = k; i < j; ++i) {
        insert(i, team, k);
    }
}

// Function to reorder the players in a team in numerical order within the same position.
void reorder(vi& team) {
    // Ordering the defense
    insertion_sort(1, team, formation[1] + 1);
    // Ordering the midfield
    insertion_sort(formation[1] + 1, team, formation[2] + formation[1] + 1);
    // Ordering the forwards
    insertion_sort(formation[2] + formation[1] + 1, team, formation[3] + formation[2] + formation[1] + 1);
}


// Given the document in which the output will be written and a
// team, writes the team in the document with the required format.
void print_solution(vi& team){
    vector <string> encode = {"POR", "DEF", "MIG", "DAV"};
    
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

// Function used to compare players with respect to their price.
bool less_price(const player& p1, const player& p2){
    return p1.price < p2.price;
}

// Function that separates the players by the position they play in.
// We only take those players whose price is not above Max_Price.
// It also sorts the players of each position increasingly by price.
void player_position_separator (const vector <player>& players, const int Max_Price) {
    map <string, int> classifier = {{"por", 0}, {"def", 1}, {"mig", 2}, {"dav", 3}};

    for (const player& p : players) {
        if (p.price > Max_Price) continue;

        int pos = classifier[p.position];
        classified_players[pos].push_back(p);
    }

    for (int i = 0; i < 4; ++i){
        sort(classified_players[i].begin(), classified_players[i].end(), less_price);
    }
}

// Checks whether a team is valid, i.e. that its cost is below our budget and 
// that it doesn't have repeated players. The other conditions are always satisfied.
bool valid_permutation(vi& team){
    if (team_price(team) > max_price_team) return false;
    reorder(team);

    int sum = 0;
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < formation[i]; ++j){
            if (j > 0 and team[sum] == team[sum-1]) return false; //repeated player
            ++sum; 
        }
    }
    
    return true;
}

// Given a position (0 for goalkeeper, 1 for defense, and so on) and the current cost of
// a team, returns the greatest index of the player of such position that, when taken,
// the new cost of the team won't exceed the budget.  
int range(int pos, int current_cost){
    player null = {"", "", "", 0, 0};
    null.price = max_price_team - current_cost;
    auto it = lower_bound(classified_players[pos].begin(), classified_players[pos].end(), null, less_price);
    return it - classified_players[pos].begin(); 
}

// This function returns a random valid team.
vi add_random(){
    vi new_team(11, 0);
    int start = rng() % 11;
    // We start choosing the players from a random index of the new_team vector so
    // that we are as likely to start choosing goalkeepers than any other position.

    while (not valid_permutation(new_team)){
        int total_cost = 0; //cost of the team.

        for (int i = 0; i < 11; ++i){
            int j = (i + start) % 11;
            int pos = position(j);

            //choose random player who won't exceed budget.
            new_team[j] = rng() % range(pos, total_cost);
            total_cost += classified_players[pos][new_team[j]].price;
        }
    }

    return new_team;
}

// Auxiliary function to the crossover function, which takes in two players and randomly outputs
// one of the two players to choose said player in the recombination. The probability of choosing 
// each player is a function of their points and prices.
bool coin_flip(const player& p1, const player& p2) {
    double p = p1.points/(1.0 + p1.price);
    double T = p + p2.points*(1.0 + p1.price);
    return (p/T > (double (rng() % INF)/INF)); 
}

// Function to execute the crossing between 2 teams, returning an offspring team.
vi crossover(vi& T1, vi& T2){
    vi new_team(11);
    for (int i = 0; i < 11; ++i) {
        //We find the players associated with each index in order to compare them.
        player p1 = classified_players[position(i)][T1[i]];
        player p2 = classified_players[position(i)][T2[i]];
        if (coin_flip(p1, p2)) new_team[i] = T1[i];
        else new_team[i] = T2[i];
    }
    return new_team;
}

// Given a team returns a mutation of that team. Changing exactly one of its players.
vi mutation(vi& team){
    vi mutate = team;
    int mutated_player = rng() % 11;
    int pos = position(mutated_player);
    int r = range(pos, team_price(team) - classified_players[pos][mutated_player].price);

    mutate[mutated_player] = rng() % r;
    return mutate;
}

void genetic_algorithm(){
    population.clear();
    int max_points = 0; //Maximum amount of points reached so far.
    int population_size = 1400; //Size of the population
    int last_improvement = 0;

    while (last_improvement++ < 500){ //If in 500 iterations there has been no improvement stop.
        //While there is not enough population, add random individuals (teams).
        while (population.size() < population_size) population.push_back(add_random());
        
        //Sort the population accoding to a fitting function, in this case, number of points.
        sort(population.begin(), population.end(), comp);
        
        //If the best team has more points than the record so far, update and overwrite.
        if (team_points(population.front()) > max_points) {
            last_improvement = 0; //reset.
            max_points = team_points(population.front());
            print_solution(population.front());
        }

        vvi new_population; //This will be the next generation.

        //The best 500 different individuals will pass to next generation.
        for (int i = 0; i < population.size() and new_population.size() < 500; ++i) {
            if (i == 0 or new_population.back() != population[i]){
                new_population.push_back(population[i]);
            }
        }

        //A mutation of the best 500 parents will also pass to next generation (if they are valid).
        for (int i = 0; i < 500; ++i){
            vi mutate = mutation(population[i]);
            if (valid_permutation(mutate)) new_population.push_back(mutate);
        }
        
        //The best 20 pairwise distinct parents will recombine. Valid childs pass to next generation.
        for (int i = 0; i < 20; ++i){
            for (int j = 0; j < 20; ++j){
                if (i == j) continue;

                vi child = crossover(population[i], population[j]);
                if (valid_permutation(child)) new_population.push_back(child);
            }
        }

        population = new_population; //Update population.
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
    
    genetic_algorithm(); //Solve the problem using a genetic algorithm.
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
