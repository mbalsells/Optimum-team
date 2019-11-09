#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <cassert>
#include <time.h>   
using namespace std;

struct player {
    string name, position, club;
    int price, points;
};

typedef vector <player> players; 
typedef vector <int> vi; 
typedef vector <vi> vvi; 
typedef vector <vvi> vvvi; 
typedef vector <vvvi> vvvvi;
typedef vector <vvvvi> vvvvvi;

string outputFile;
players everyone;
vector <int> formation; // has 3 position;
int cost_player;
vvvvi dp;
vvvvvi best;

const int INF = 2e9;

vector <string> encode = {"por", "def", "mig", "dav"};

void print_player(player p){
    cout << "Nom: " << p.name << endl;
    cout << "Posició: " << p.position << endl;
    cout << "Preu: " << p.price << endl;
    cout << "Club: " << p.club << endl;
    cout << "Punts: " << p.points << endl;
    cout << endl;
}

int rec(int points, int c, int left, int pos){
    //cout << points << ", " << c << " and " << left << " also " << pos << endl;
    if (points < 0) return 0;

    vi& team = best[points][c][left][pos];

    if (pos == everyone.size()){
        team = {};
        if (left > 0) return INF;
        if (points != 0) return INF;
        return 0;
    }

    int& ans = dp[points][c][left][pos];

    if (ans != -1) return ans;

    int newc = c, newleft = left;

    if (pos < everyone.size() - 1 and encode[c] != everyone[pos+1].position) {
        newleft = formation[c];
        ++newc;
    }

    ans = rec(points, newc, newleft, pos + 1);
    team = best[points][newc][newleft][pos+1];

    if (newc == c) --newleft; 

    if (left > 0 and everyone[pos].price <= cost_player and points >= everyone[pos].points) {
        int taken = everyone[pos].price + rec(points - everyone[pos].points, newc, newleft, pos + 1);
        if (taken < ans) {
            ans = taken;

            team = best[points - everyone[pos].points][newc][newleft][pos+1];
            team.push_back(pos);
        }
    }

    return ans;
}

void order_by(){ //WOOOOOEOEOEOOEOO WAAAA
    players AUX;

    for (int i = 0; i < everyone.size(); ++i){
        if (everyone[i].position == "por") AUX.push_back(everyone[i]);
    }
    for (int i = 0; i < everyone.size(); ++i){
        if (everyone[i].position == "def") AUX.push_back(everyone[i]);
    }
    for (int i = 0; i < everyone.size(); ++i){
        if (everyone[i].position == "mig") AUX.push_back(everyone[i]);
    }
    for (int i = 0; i < everyone.size(); ++i){
        if (everyone[i].position == "dav") AUX.push_back(everyone[i]);
    }

    everyone = AUX;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Syntax: " << argv[0] << " data_base.txt" << endl;
        exit(1);
    }

    vector <int> PTS;

    ifstream in(argv[1]);
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
        PTS.push_back(punts);
    }

    sort(PTS.begin(), PTS.end(), greater <int>());

    int sum = 0;
    for (int i = 0; i < 11; ++i) sum += PTS[i];
    cout << "::: " << sum << " !! " << endl;

    in.close();

    order_by();

    cout << " stop " << endl;   

    int N1, N2, N3, cost_team;
    while (cin >> N1 >> N2 >> N3 >> cost_team >> cost_player){
        clock_t t;
        t = clock();

        //cout << " woo " << endl;

        dp = vvvvi(3002, vvvi(4, vvi(6, vi(everyone.size() + 1, -1))));
        best = vvvvvi(3002, vvvvi(4, vvvi(6, vvi(everyone.size() + 1))));

        //cout << " okay  --> " << everyone.size() << endl;

        formation = {N1, N2, N3};

        vector <int> best_team = {};
        int maximum = -1;

        for (int i = 0; i < 3000; ++i){
            int cost = rec(i, 0, 1, 0);

            if (cost <= cost_team and cost != INF) { //not necessary really
                maximum = cost;
                best_team = best[i][0][1][0];
            } 
        }
        
        t = clock() - t;
        cout << " IT TAKES " << ((float)t)/CLOCKS_PER_SEC;

        cout << maximum << endl;
        for (int i : best_team) print_player(everyone[i]);
    } 
}
