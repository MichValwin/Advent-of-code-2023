#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <vector>
#include <set>
#include <regex>
#include <cmath>
#include <unordered_map>
#include <queue>

using namespace std;

static const char* INPUT_FILE = "input.txt";
static const char* DEMO_FILE = "demo.txt";
static const bool USE_REAL_DATA = true;

// Helpers
vector<std::string> splitString(const string &str, const string &delimiter) {
    vector<std::string> strings;

    int start = 0;
    int end = str.find(delimiter);
    while (end != -1) {
        strings.push_back(str.substr(start, end - start));
        start = end + delimiter.size();
        end = str.find(delimiter, start);
    }
    strings.push_back(str.substr(start, end - start));

    return strings;
}

struct Race {
    int64_t time;
    int64_t distance;
};

struct IntervalWin {
    int64_t minPressed;
    int64_t maxPressed;
};

bool canWin(const Race& race, int64_t timePressing) {
    int64_t timeRemaining = race.time - timePressing;
    if(timePressing*timeRemaining > race.distance)return true;
    return false;
}

double solveQuadratic(double a, double b, double c, bool sign) {
    double sq = sqrt(pow(b,2) -4.0*a*c);
    if(sign) {
        return (-b + sq)/(2*a);
    }else{
        return (-b - sq)/(2*a);
    }
}

int64_t getWinsQuadratic(const Race& race) {

    int64_t lower = ceil(solveQuadratic(-1, race.time, -race.distance, true));
    int64_t high = floor(solveQuadratic(-1, race.time, -race.distance, false));

    int64_t wins = high - lower +1;
    return wins; 
}

int64_t getGold(const Race& race) {
    IntervalWin intervalWinning = {0, 0};
    // Get min time winning
    for(int64_t timePressing = 1; timePressing < race.time; timePressing++) {
        if(canWin(race, timePressing)) {
            intervalWinning.minPressed = timePressing;
            break;
        }
    }

    // Get max time winning
    for(int64_t timePressing = race.time-1; timePressing > 0; timePressing--) {
        if(canWin(race, timePressing)) {
            intervalWinning.maxPressed = timePressing;
            break;
        }
    }

    int64_t wins = intervalWinning.maxPressed - intervalWinning.minPressed + 1;
    return wins;
}

int64_t getSilver(const vector<Race>& races) {
    int64_t silver = 1;

    for(size_t i = 0; i < races.size(); i++) {
        int64_t wins = 0;
        for(int64_t timePressing = 1; timePressing < races[i].time-1; timePressing++) {
            if(canWin(races[i], timePressing))wins++;
        }
        silver *= wins;
    }

    return silver;
}

int main() {
    // Get input file
    fstream inputFile;
    string file = USE_REAL_DATA ? INPUT_FILE : DEMO_FILE;
    inputFile = fstream(file);
    if(!inputFile.is_open()) {
        cout << "Can't open " << file << endl;
        exit(0);
    }
    

    // Get times
    string line;
    getline(inputFile, line);
    vector<string> timesRaces = splitString(splitString(line, "Time:")[1], " ");

    getline(inputFile, line);
    vector<string> distanceRaces = splitString(splitString(line, "Distance:")[1], " ");
    
    std::vector<Race> racesSilver;
    for(const string& time: timesRaces) {
        if(!time.empty())racesSilver.push_back({(int64_t)atol(time.c_str()), 0});
    }
    int i = 0;
    for(const string& distance: distanceRaces) {
        if(!distance.empty()) {
            racesSilver[i].distance = (int64_t)atol(distance.c_str());
            i++;
        }
    }

    string timeTotal = "";
    for(const string& time: timesRaces) {
        if(!time.empty())timeTotal += time;
    }
    string distanceTotal = "";
    for(const string& distance: distanceRaces) {
        if(!distance.empty())distanceTotal += distance;
    }
    Race raceGold = {(int64_t)atol(timeTotal.c_str()), (int64_t)atol(distanceTotal.c_str())};


    //Debug
    /*
    for(int64_t i = 0; i < racesSilver.size(); i++) {
        cout << "Race " << i+1 << "{ t:" << racesSilver[i].time << ", d:" << racesSilver[i].distance << " }\n"; 
    }
    cout << "Race Gold: " << "{t: " << raceGold.time << ", d: " << raceGold.distance << "}\n"; 
    */

    cout << "Silver: " << getSilver(racesSilver) << "\n";
    cout << "Gold: "  << getGold(raceGold) << "\n";
    cout << "Gold Quadratic: " << getWinsQuadratic(raceGold) << "\n";
    return 0;
}