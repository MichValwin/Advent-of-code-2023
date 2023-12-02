#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <regex>
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


void printGame(const Game& game) {
    std::cout << "Game " << game.id << ": ";
    for(const SubsetCubes& subsets: game.subsets) {
        if(subsets.red != 0) std::cout << subsets.red << " red, ";
        if(subsets.green != 0) std::cout << subsets.green << " green, ";
        if(subsets.blue != 0) std::cout << subsets.blue << " blue, ";
        std::cout << ";";
    }
}

/*
// Split Game 1: from the other part with: Game (\d*):(.*)
Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
to
 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
// Split by ;
 3 blue, 4 red
 1 red, 2 green, 6 blue
 2 green

// Split by ,
 3 blue
 4 red
 1 red
 2 green
 6 blue
 2 green

// Get numbers and colors with:    .?(\d*) (blue|red|green)
*/
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
        std::regex gameSeparator(R"(Game (\d*):(.*))");
        std::regex quantityColor(R"(.?(\d*) (blue|red|green))");

        // Split game and sets
        std::smatch matches;
        bool isThereMatch = std::regex_match(line, matches, gameSeparator);
        if(!isThereMatch){
            std::cout << "Can't find a match for game. Exiting.\n";
            exit(1);
        }

        uint32_t gameId = std::atoi(matches[1].str().c_str());
        std::vector<std::string> subsets = splitString(matches[2], ";");
        std::vector<SubsetCubes> subsetsForGame;
        for(const std::string& subset: subsets) {
            // Get each color
            std::vector<std::string> eachColor = splitString(subset, ",");
            SubsetCubes subsetGame = {0,0,0};

            for(const std::string& colorStr: eachColor) {
                std::smatch matches;
                bool isThereMatch = std::regex_match(colorStr, matches, quantityColor);
                if(!isThereMatch) {
                    std::cout << "Can't find a single match for a color in the subset. (is this an error?) Exiting." << "\n";
                    exit(1);
                }
                uint32_t quantity = std::atoi(matches[1].str().c_str());
                const std::string color = matches[2].str();
                if(color == "red") {
                    subsetGame.red = quantity;
                }else if(color == "green") {
                    subsetGame.green = quantity;
                }else{
                    subsetGame.blue = quantity;
                }
            }
            subsetsForGame.push_back(subsetGame);
        }

        games.push_back({gameId, subsetsForGame});
    }

    /*
    for(const Game& game: games) {
        printGame(game);
        std::cout << "\n";
    }
    */
   

    SubsetCubes possibleSilver = {12, 13, 14};
    std::cout << "Silver: " << getSilver(games, possibleSilver) << "\n";
    std::cout << "Gold: " << getGold(games) << "\n";
    return 0;
}