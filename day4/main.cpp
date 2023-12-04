#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <regex>
#include <cmath>

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



struct Card{
    std::vector<uint32_t> winningNums;
    std::vector<uint32_t> allNums;

    void print() {
        for(size_t i = 0; i < winningNums.size(); i++) {
            std::cout << winningNums[i] << " ";
        }
        std::cout << "| ";
        for(size_t i = 0; i < allNums.size(); i++) {
            std::cout << allNums[i] << " ";
        }
    }
};

uint32_t getMatchesCard(const Card& card) {
    uint32_t matches = 0;

    std::set<uint32_t> winningNums(card.winningNums.begin(), card.winningNums.end());
    for(uint32_t n: card.allNums) {
        if(winningNums.find(n) != winningNums.end()) {
            matches++;
        }
    }
    return matches;
}

uint32_t getSilver(const std::vector<Card>& cards) {
    uint32_t points = 0;
    
    for(const Card& card: cards) {
        uint32_t matches = getMatchesCard(card);
        if(matches > 2) {
            matches = std::pow(2, matches-1);
        }
        points += matches;
    }

    return points;
}

uint32_t getGold(const std::vector<Card>& cards) {
    uint32_t totalCards = 0;

    std::vector<uint32_t> instancesCards(cards.size());
    for(uint32_t i = 0; i < cards.size(); i++)instancesCards[i] = 1;

    for(uint32_t i = 0; i < cards.size(); i++) {
        for(uint32_t numInst = 0; numInst < instancesCards[i]; numInst++) {
            uint32_t matches = getMatchesCard(cards[i]);

            for(uint32_t j = 0; j < matches; j++){
                instancesCards[i+1+j]++;
            }
        }
    }

    for(uint32_t instances: instancesCards) {
        totalCards += instances;
    }

    return totalCards;
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
    
    std::vector<Card> cards;

    // Get width and height
    std::string line;
    while(std::getline(inputFile, line)) {
        std::string numbersCard = splitString(line, ":")[1];
        std::vector<std::string> separateNums = splitString(numbersCard, "|");
        if(separateNums.size() != 2) {
            std::cout << "There is no | to split??. Existing" << "\n";
            exit(1);
        }

        // Get winning numbers
        std::vector<std::string> vecStrWinning = splitString(separateNums[0], " ");
        std::vector<std::string> vecStrAllHave = splitString(separateNums[1], " ");


        std::vector<uint32_t> winningNums;
        for(uint32_t i = 0; i < vecStrWinning.size(); i++) {
            if(vecStrWinning[i].size() > 0) {
                winningNums.push_back(std::atoi(vecStrWinning[i].c_str()));
            }
        }

        std::vector<uint32_t> allNumsHave;
        for(uint32_t i = 0; i < vecStrAllHave.size(); i++) {
            if(vecStrAllHave[i].size() > 0) {
                allNumsHave.push_back(std::atoi(vecStrAllHave[i].c_str()));
            }
        }

        cards.push_back({winningNums, allNumsHave});
    }
    
    // Print cards
    /*
    for(size_t i = 0; i < cards.size(); i++) {
        std::cout << "Card " << i+1 << ": ";
        cards[i].print();
        std::cout << "\n";
    }
    */


    std::cout << "Silver: " << getSilver(cards) << "\n";
    std::cout << "Gold: " << getGold(cards) << "\n";
    return 0;
}