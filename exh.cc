#include <iostream>
#include <vector>
#include <fstream>
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

string output_file; //name of the output file
clock_t initial_time; //time when the program started


vector <vector <player> > classified_players(4);
// Classified_players[position] has all the players that have that position,
// considering that the position ranges from 0 to 3 where 0 is goalkeeper,
// 1 is defense, 2 is midfielder and 3 is forward.

vector <int> formation; 
// Number of required players for each position.

vvvvi dp;
// Is the memoization vector. Where dp[p][pos][left_players][k]
// represents the minimum price required to make a valid team of exactly p
// points, but formed only by the required amount of players of the positions
// greater than pos and exactly left_players belonging to the position pos,
// among which, all are located in classified_players[pos][j] with j >= k.

vvvvvi best;
// Given a state of the dp, returns a vector with the indexes where the
// players of the optimum team are located. The indexes correspond to the 
// positions of the strikers, midfielders, defenders and the goalkeeper, in this
// order.


// Given the document in which the output will be written and a
// team, writes the team in the document with the required format.
void print_solution(vector <int>& team){
    vector <string> encode = {"POR", "DEF", "MIG", "DAV"};
    int points = 0, price = 0; //Total points and price of the team

    ofstream document;
    document.open(output_file);
    document.setf(ios::fixed); //one decimal place for the execution time
    document.precision(1);

    document << double(clock() - initial_time)/CLOCKS_PER_SEC << endl;

    int index = 10;
    for (int pos = 0; pos < 4; ++pos){
        document << encode[pos] << ": ";

        for (int j = 0; j < formation[pos]; ++j){
            if (j > 0) document << ";";

            //Current player 
            player member = classified_players[pos][team[index--]];
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


// Function used to sort players with descending order with respect to the
// points and ascending with respect to the price and name (in this order). 
bool comp(const player& p1, const player& p2){
    if (p1.points != p2.points) return p1.points > p2.points;
    if (p1.price != p2.price) return p1.price < p2.price;
    return p1.name < p2.name;
}

// Function that separates the players by the position they play in, it also 
// sorts the players of each category according to the comp function.
// Finally, we only take those players whose price is not above Max_Price.
void player_position_separator (const vector <player>& players, const int Max_Price) {
    map <string, int> classifier = {{"por", 0}, {"def", 1}, {"mig", 2}, {"dav", 3}};

    for (const player& p : players) {
        if (p.price > Max_Price) continue;

        int pos = classifier[p.position];
        classified_players[pos].push_back(p);
    }
}


// This function is used to initiallize the variables used in the program.
// It is the case of the formation vector, the classified_players matrix,
// and it fixes the states of the memoization vector.
void initialize(const vector <player>& players, int N1, int N2, int N3, int max_price){
    formation = {1, N1, N2, N3};
    
    player_position_separator(players, max_price); //separate players by position

    int max_points = 1; //Maximum points that could be obtained if we had no budget limit
    int max_players = 0; //Maximum number of players that a position can have
    int max_taken = max(N1, max(N2, N3)) + 1; //Maximum number of players that can be
                                              //required from one of the positions

    for (int pos = 0; pos < 4; ++pos){
        sort(classified_players[pos].begin(), classified_players[pos].end(), comp);
        max_players = max(max_players, 2 + (int) classified_players[pos].size());

        for (int j = 0; j < formation[pos]; ++j) //Select the ones with more points
            max_points += classified_players[pos][j].points;
    }

    //Initialize the memoization vectors
    dp = vvvvi(max_points, vvvi(4, vvi(max_taken, vi(max_players, -1))));
    best = vvvvvi(max_points, vvvvi(4, vvvi(max_taken, vvi(max_players, {}))));
}

// This recursive function is the main funcion of the program, and works following
// the dynamic programming paradigm: given an amount of points, a position, the 
// number of players left to take from said position, the index of the player 
// currently being considered in the classified_players[pos] vector and the maximum
// price that can be spent on a team. It returns the minimum price needed to make a
// team with the exact amount of points required and with a price less than the budget 
// threshold, which has as members the required number of players for each position > pos
// and the left variable holds how many players are left with position pos.
// If there is no such team, it returns INF.
int min_price(int points, int pos, int left, int k, int max_price){
    vi& team = best[points][pos][left][k];
    int& ans = dp[points][pos][left][k];
    
    if (ans != -1) return ans; //We already computed this case!
    
    int max_players = classified_players[pos].size(); //Number of players that have that position.
    if (pos == 3 and k == max_players) return ans = (points == 0 ? 0 : INF); //All players seen.
    
    if (k == max_players) { //We are in the next position!
        ++pos;
        k = 0;
        left = formation[pos];
    }

    if (left > classified_players[pos].size() - 1 - k) return ans = INF;
        //If we need to take more players of this position than there are available return INF.

    player& current_player = classified_players[pos][k];
    
    // We decide not to take the current_player in our team.
    ans = min_price(points, pos, left, k + 1, max_price);
    team = best[points][pos][left][k + 1];
    
    int leftpoints = points - current_player.points; //remaining points if the player is taken.
    if (left > 0 and leftpoints >= 0){

        // Now we decide to take the current_player in our team.
        int taken = current_player.price + min_price(leftpoints, pos, left - 1, k + 1, max_price);

        if (taken < ans and taken <= max_price) { //if we get a better result by taking it:
            ans = taken;
            team = best[leftpoints][pos][left - 1][k + 1];
            team.push_back(k); //Put its index in the best team.
        }
    }

    return ans;
}

// Given the maximum_price that a team can have, and having initialized all the
// global variables, this function finds the best price with which all possible scores can
// be obtained, overwiting the best solution found if a better one comes up.
void solve(int max_price){
    for (int points = 0; points < dp.size(); ++points){
        int cost = min_price(points, 0, 1, 0, max_price);
        if (cost <= max_price) print_solution(best[points][0][1][0]);
        //Each time we find a valid team with more points we update the output.
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
    int N1, N2, N3, max_price_team, max_price_player;
    in >> N1 >> N2 >> N3 >> max_price_team >> max_price_player;
    in.close();

    initialize(players, N1, N2, N3, max_price_player); //Initialize variables.
    solve(max_price_team); //Solve the problem.
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
