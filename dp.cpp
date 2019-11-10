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
typedef vector <player> players;

vector <players> classified_players;
// classified_players[position] has all the players that have that position,
// considering that the position ranges from 0 to 3 where 0 is goalkeeper,
// 1 is defense, 2 is midfielder and 3 is forward.

vector <int> formation; 
// number of needed players for each position

vvvvi dp;
// Is the memoization vector. Where dp[p][pos][left_players][k]
// represents the minimum price required to make a valid team of exactly p
// points, but formed only by the required amount of players of the positions
// greater than pos and exactly left_players that belong to the position pos,
// among which all are located in classified_players[pos][j] with j >= k.

vvvvvi best;
// given a state of the dp, returns a vector with the indexes where the
// players of the optimum team are located.

// Function used to sort players with descending order with respect to the
// points and ascending with respect to the price and name (in this order). 
bool comp(const player& p1, const player& p2){
    if (p1.points != p2.points) return p1.points > p2.points;
    if (p1.price != p2.price) return p1.price < p2.price;
    return p1.name < p2.name;
}

// Function that separates the players by the position they play, it also 
// sorts the players of each category according to the comp function.
// finally, we only take those players whose price is not above Max_Price
void player_position_separator (const players& everyone, const int Max_Price) {
    map <string, int> classifier = {{"por", 0}, {"def", 1}, {"mig", 2}, {"dav", 3}};

    for (const player& p : everyone) {
        if (p.price > Max_Price) continue;

        int pos = classifier[p.position];
        classified_players[pos].push_back(p);
    }

    for (int pos = 0; pos < 4; ++pos) 
        sort(classified_players[pos].begin(), classified_players[pos].end(), comp);
}

// Computes the maximum number of points that we can get assuming there is no
// budget limit.
int max_points_compute(){
    int ans = 0;
    for (int pos = 0; pos < 4; ++pos){
        for (int j = 0; j < formation[pos]; ++j)
            ans += classified_players[pos][j].points;
    }

    return ans;
}

// Computes the maximumum number of players that can share the same position
int max_players_compute(){
    int ans = 0;
    for (int pos = 0; pos < 4; ++pos){
        ans = max(ans, (int) classified_players[pos].size());
    }

    return ans;
}

// This function is used to initiallize the variables used in the program,
// it is the case of the formation vector, the classified_players matrix,
// and it fixes the states of the dynamic.
void initialize(const players& everyone, int N1, int N2, int N3, int max_price_player){
    formation = {1, N1, N2, N3, 0};
    classified_players = vector <players> (4);

    player_position_separator(everyone, max_price_player);

    int max_points = max_points_compute() + 1;
    int max_players = max_players_compute() + 1;
    int max_taken = max(N1, max(N2, N3)) + 1;

    dp = vvvvi(max_points, vvvi(5, vvi(max_taken, vi(max_players, -1))));
    best = vvvvvi(max_points, vvvvi(5, vvvi(max_taken, vvi(max_players, {}))));
}

// This recurrent function is the main funcion of the dynamic. Given a amount of
// points, a position, the number of players left to take of that position, the
// index that the player we are considering has in the classified_players[pos] 
// vector and the maximum price that a team can have. Returns the minimum price
// needed to make a team of that exact amount of points, of price less than the
// threshold, which has as members the required number of players for each
// position > pos and exactly left players that have as position pos.
int rec(int points, int pos, int left, int k, int max_price){
    vi& team = best[points][pos][left][k];
    int& ans = dp[points][pos][left][k];
    if (ans != -1) return ans;
    
    if (pos == 4) return ans = (points == 0 ? 0 : INF); // all players considered
    if (left > classified_players[pos].size() - 1 - k) return ans = INF;
        
    player& current_player = classified_players[pos][k];

    int newpos = pos, newleft = left, newk = k + 1, newpoints = points;
    if (newk == classified_players[pos].size()){
        newpos = pos + 1;
        newleft = formation[newpos];
        newk = 0;
    }

    if (newpos == pos){
        // we decide not to take the current_player in our team.
        ans = rec(points, newpos, newleft, newk, max_price);
        team = best[points][newpos][newleft][newk];
    }
    
    newpoints -= current_player.points;
    if (newpoints >= 0) {
        if (--newleft == 0){
            newpos = pos + 1;
            newk = 0;
            newleft = formation[newpos];
        }

        // we decide to take the current_player in our team
        int taken = current_player.price + rec(newpoints, newpos, newleft, newk, max_price);
        
        if (taken < ans and taken <= max_price) {
            ans = taken;
            team = best[newpoints][newpos][newleft][newk];
            team.push_back(k);
        }
    }
    return ans;
}

// Given the maximum_price that a team can have, and having initialized all the
// global variables, returns the team with the highest amount of points, which 
// price is below the threshold.
players solve(int max_price){
    vector <int> best_team;

    for (int points = 0; points < dp.size(); ++points){
        int cost = rec(points, 0, 1, 0, max_price);
        if (cost <= max_price) best_team = best[points][0][1][0];
    }

    players ans;
    for (int pos = 0; pos < 4; ++pos){
        for (int j = 0; j < formation[pos]; ++j){
            ans.push_back(classified_players[pos][best_team.back()]);
            best_team.pop_back();
        }
    }

    return ans;
}

// Given the document in which the output will be introduced and a
// team, introduces the team in the document with the required format
void print_solution(ofstream& document, players& team){
    vector <string> encode = {"POR", "DEF", "MIG", "DAV"};
    int points = 0, price = 0;

    int index = 0;
    for (int pos = 0; pos < 4; ++pos){
        document << encode[pos] << ": ";

        for (int j = 0; j < formation[pos]; ++j){
            if (j > 0) document << ";";
            points += team[index].points;
            price += team[index].price;

            document << team[index++].name;
        }

        document << endl;
    }

    document << "Punts: " << points << endl;
    document << "Preu: " << price << endl;
}

// given the file with the data of the football players, returns
// all the players of the data_base.
players read_data(string input_file){
    players everyone;

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
        everyone.push_back({nom, posicio, club, preu, punts});
    }  

    return everyone;
}

// Given the input file of the test case and all the players,
// returns the optimum team, i.e. the one that satisfies all the
// constraints and has the highest punctuation. 
players get_solution(string input_file, players& everyone){
    ifstream in("public_benchs/" + input_file);
    int N1, N2, N3, max_price_team, max_price_player;
    in >> N1 >> N2 >> N3 >> max_price_team >> max_price_player;
    in.close();

    initialize(everyone, N1, N2, N3, max_price_player);

    return solve(max_price_team);
}

// Given the file where the output must be printed, the optimum team, and
// the execution time that has been needed to get the answer, it
// introduces the output in the output file with the required format.
void write_soltuion(string output_file, players& best_team, double time){
    ofstream document;
    document.open(output_file);
    document.setf(ios::fixed);
    document.precision(1);

    document << time/CLOCKS_PER_SEC << endl;
    print_solution(document, best_team);
    document.close();
}

int main(int argc, char** argv) {
    clock_t time = clock();

    if (argc != 4) {
        cout << "3 arguments required!" << endl;
        cout << "the data_base, the test_case and the output file" << endl;
        exit(1);
    }

    players everyone = read_data(argv[1]);
    players best_team = get_solution(argv[2], everyone);
    write_soltuion(argv[3], best_team, clock() - time);
}
