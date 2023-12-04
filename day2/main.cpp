#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include <filesystem>

static const char* INPUT_FILE = "input.txt";
static const char* DEMO_FILE = "demo.txt";
static const bool USE_REAL_DATA = true;

// Helpers
std::vector<std::string> splitString(const std::string &str, const std::string &delimiter) {
    std::vector<std::string> strings;

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

struct SubsetCubes {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
};

struct Game {
    uint32_t id;
    std::vector<SubsetCubes> subsets;
};

enum class Color {
    red, green, blue
};

void ltrim(std::string& str) {
    if(str[0] == ' ')str.erase(0, 1);
}

uint32_t getSilver(std::vector<Game>& games, SubsetCubes& possible) {
    std::vector<uint32_t> gameIDPossible;

    for(const Game& game: games) {
        bool gamePossible = true;
        for(const SubsetCubes& subset: game.subsets) {
            if(subset.blue > possible.blue || subset.green > possible.green || subset.red > possible.red){
                gamePossible = false;
            }
        }

        if(gamePossible) {
            gameIDPossible.push_back(game.id);
        }
    }

    uint32_t sumId = 0;
    for(uint32_t gameId: gameIDPossible) {
        sumId += gameId;
    }
    return sumId;
}

uint64_t getGold(std::vector<Game>& games) {
    std::vector<uint32_t> gameIDPossible;
    std::vector<SubsetCubes> minimumSetGames;

    for(const Game& game: games) {
        SubsetCubes minimumSubset = {0, 0, 0};
        for(const SubsetCubes& subset: game.subsets) {
            if(minimumSubset.red < subset.red)minimumSubset.red = subset.red;
            if(minimumSubset.green < subset.green)minimumSubset.green = subset.green;
            if(minimumSubset.blue < subset.blue)minimumSubset.blue = subset.blue;
        }

        minimumSetGames.push_back(minimumSubset);
    }

    uint64_t sumGold = 0;
    for(const SubsetCubes& subset: minimumSetGames) {
        sumGold += subset.red * subset.green * subset.blue;
    }
    return sumGold;
}



int main() {
    // Get input file
    std::fstream inputFile;
    std::string file = USE_REAL_DATA ? INPUT_FILE : DEMO_FILE;
    inputFile = std::fstream(file);
    if(!inputFile.is_open()) {
        std::cout << "Can't open " << file << std::endl;
        exit(0);
    }

    std::vector<Game> games;

    // Parse
    std::string line;
    while(std::getline(inputFile, line)) {
        std::vector<std::string> gameLine = splitString(line, ":");
        uint32_t gameId;
        std::sscanf(gameLine[0].c_str(), "%*s %d", &gameId);

        // Get subsets
        std::vector<std::string> subsets = splitString(gameLine[1], ";");
        std::vector<SubsetCubes> subs;
        for(size_t i = 0; i < subsets.size(); i++) {
            // Get each color in subset
            std::vector<std::string> eachColor = splitString(subsets[i], ",");
            SubsetCubes subset = {0,0,0};

            for(std::string& colorStr: eachColor) { 
                ltrim(colorStr);
                uint32_t quantity = std::atoi(splitString(colorStr, " ")[0].c_str());

                //std::sscanf(colorStr.c_str(), "%d %20s%n", &quantity);
                if(colorStr.find("red") != std::string::npos) {
                    subset.red = quantity;
                }else if(colorStr.find("green") != std::string::npos) {
                    subset.green = quantity;
                }else{
                    subset.blue = quantity;
                }
            }

            subs.push_back(subset);
        }

        Game game = {gameId, subs};
        games.push_back(game);
    }


    SubsetCubes possibleSilver = {12, 13, 14};
    std::cout << "Silver: " << getSilver(games, possibleSilver) << "\n";
    std::cout << "Gold: " << getGold(games) << "\n";
    return 0;
}