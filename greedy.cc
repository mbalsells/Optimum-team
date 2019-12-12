#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <cassert>
#include <time.h>

using namespace std;
typedef long long int ll;

struct player {
    string name, position, club;
    int price, points;          
};

string output_file; //name of the output file.
clock_t initial_time; //time when the program started.

vector <int> formation; 
// Number of required players for each position.

ll budget; //budget for the team.

// This function is used to sort players such that the first ones
// when taken give a better total score. 
bool comp (const player p1, const player p2) {
    double points1 = p1.points, points2 = p2.points;
    double price1 = p1.price, price2 = p2.price;

    //Expected price of a player. 
    double Expected_price = budget/11.0;

    //we slighly penalize the players with high price normalizing it using 
    //the expected price of a player.
    points1 *= (1 - price1/(3*Expected_price));
    points2 *= (1 - price2/(3*Expected_price));

    return points1 > points2;
}

// Sort players mainly by position but also points, price and name.
bool order_position (const player& p1, const player& p2) {
    map <string, int> classifier = {{"por", 0}, {"def", 1}, {"mig", 2}, {"dav", 3}};

    if (classifier[p1.position] != classifier[p2.position]) 
        return classifier[p1.position] < classifier[p2.position];
    if (p1.points != p2.points) return p1.points > p2.points;
    if (p1.price != p2.price) return p1.price < p2.price;
    return p1.name < p2.name;
}

// Given a team, the fucntion writes it in the document with the required format.
void print_solution(vector <player>& team){
    // we sort the team players according to the output rules.
    sort(team.begin(), team.end(), order_position);
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

    cout << points << endl; //OJO TREURE !!
    document << "Punts: " << points << endl;
    document << "Preu: " << price << endl;
    document.close();
}

// This function is used to eliminate players whose price is greater
// than the maxmium allowed and also, it sorts the players according
// to the comp function.
void prepare(vector <player>& players, int max_price){
    // First we eliminate players whose price is above the treshold.
    vector <player> aux;
    for (player p : players) if (p.price <= max_price) aux.push_back(p);
    players = aux;
    
    //Now we sort the players according to comp.
    sort(players.begin(), players.end(), comp);
}


// Given all the players, we select a team by taking players in order,
// as long as we have enought budget to take it and we don't have
// enough players with this position.
void greedy(vector <player>& players){
    map <string, int> classifier = {{"por", 0}, {"def", 1}, {"mig", 2}, {"dav", 3}};

    vector <int> formed(4, 0);
    vector <player> ans;
    for (int i = 0; i < players.size(); ++i){
        player current = players[i];
        int position = classifier[current.position];

        if (budget < current.price) continue;
        if (formed[position] == formation[position]) continue;
            
        budget -= current.price;
        ++formed[position];
        ans.push_back(current);
    }

    print_solution(ans);
}

// Given the input file of the test case and all the players, the function
// returns a good enough team, i.e. a team that satisfies all the
// constraints and has a high score.
void get_solution(string input_file, vector <player>& players){
    ifstream in(input_file);
    int N1, N2, N3, max_price_player;
    in >> N1 >> N2 >> N3 >> budget >> max_price_player;
    in.close();

    formation = {1, N1, N2, N3};
     
    prepare(players, max_price_player); //Clean and sort players.
    greedy(players);                    //Solve the problem.
}

// Given the file containing the data of the football players,
// the function returns all the players in the database.
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
    vector <player> players = read_data(argv[1]); //Read data base.
    get_solution(argv[2], players);               //Get a good team.
}
