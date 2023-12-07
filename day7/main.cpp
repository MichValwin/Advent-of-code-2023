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

using Hand = char[5];
const static char ORDER_POWER_SILVER[] = {'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'};
const static char ORDER_POWER_GOLD[] = {'A', 'K', 'Q', 'T', '9', '8', '7', '6', '5', '4', '3', '2', 'J'};

static uint32_t powerMap[256];
static uint32_t powerMapJokers[256];

enum class HandType : uint8_t{
    fiveKind = 6, fourKind = 5, fullHouse = 4, ThreeKind = 3, TwoPair = 2, OnePair = 1, HighCard = 0
};

const HandType powerOrder[] = {HandType::fiveKind, HandType::fourKind, HandType::fullHouse, HandType::ThreeKind, HandType::TwoPair, HandType::OnePair, HandType::HighCard};

struct HandBid {
    Hand hand;
    uint32_t bid;
    HandType type;

    void print() const{
        for(uint8_t i = 0; i < 5; i++) {
            cout << hand[i];
        }
        cout << " " << bid << " " << typeid(type).name();
    }
};

bool compareHandsReverse(const HandBid& a, const HandBid& b) {
    // Smallest COMES FIRST
    uint8_t typeA = static_cast<uint8_t>(a.type);
    uint8_t typeB = static_cast<uint8_t>(b.type);

    if(typeA < typeB) {
        return true;
    }else if(typeA > typeB) {
        return false;
    }else if(typeA == typeB) {
        // Compare card by card
        for(uint8_t i = 0; i < 5; i++) {
            uint32_t pA = powerMap[(uint8_t)a.hand[i]];
            uint32_t pB = powerMap[(uint8_t)b.hand[i]];
            //if(powerOther < powerThis)return false;
            if(pA < pB) {
                return true;
            }else if(pA > pB) {
                return false;
            }
        }
    }
    return true;
}

bool compareHandsReverseJoker(const HandBid& a, const HandBid& b) {
    // Smallest COMES FIRST
    uint8_t typeA = static_cast<uint8_t>(a.type);
    uint8_t typeB = static_cast<uint8_t>(b.type);

    if(typeA < typeB) {
        return true;
    }else if(typeA > typeB) {
        return false;
    }else if(typeA == typeB) {
        // Compare card by card
        for(uint8_t i = 0; i < 5; i++) {
            uint32_t pA = powerMapJokers[(uint8_t)a.hand[i]];
            uint32_t pB = powerMapJokers[(uint8_t)b.hand[i]];
            //if(powerOther < powerThis)return false;
            if(pA < pB) {
                return true;
            }else if(pA > pB) {
                return false;
            }
        }
    }
    return true;
}

uint64_t getSilver(const vector<HandBid>& hands) {
    vector<HandBid> handsSorted;
    for(const HandBid& h: hands)handsSorted.push_back(h);
    
    sort(handsSorted.begin(), handsSorted.end(), compareHandsReverse);

    uint64_t power = 0;
    for(size_t i = 0; i < handsSorted.size(); i++) {
        power += (i+1) * handsSorted[i].bid;
    }

    return power;
}

uint64_t getGold(const vector<HandBid>& hands) {
    vector<HandBid> handsSorted;
    for(const HandBid& h: hands)handsSorted.push_back(h);
    
    sort(handsSorted.begin(), handsSorted.end(), compareHandsReverseJoker);

    uint64_t power = 0;
    for(size_t i = 0; i < handsSorted.size(); i++) {
        power += (i+1) * handsSorted[i].bid;
    }

    return power;
}

HandType getHandType(const Hand& hand) {
    uint32_t numCardsByPower[13];
    for(uint8_t i = 0; i < 13; i++)numCardsByPower[i] = 0;

    // Get nums cards
    for(uint8_t i = 0; i < 5; i++) {
        // get pos
        uint8_t j = 0;
        while(ORDER_POWER_SILVER[j] != hand[i]) {
            j++;
        }
        numCardsByPower[j]++;
    }

    bool thereAreThree = false;
    uint32_t numPairs = 0;
    for(uint8_t i = 0; i < 13; i++) {
        // Five of a kind
        if(numCardsByPower[i] == 5)return HandType::fiveKind;
        // Four of a kind
        if(numCardsByPower[i] == 4)return HandType::fourKind;

        if(numCardsByPower[i] == 3)thereAreThree = true;
        if(numCardsByPower[i] == 2)numPairs++;
    }

    if(thereAreThree && numPairs == 1)return HandType::fullHouse;

    if(thereAreThree && numPairs == 0)return HandType::ThreeKind;

    if(numPairs == 2)return HandType::TwoPair;
    if(numPairs == 1)return HandType::OnePair;

    return HandType::HighCard;
}

HandType getHandTypeJokers(const Hand& hand) {
    uint32_t numCardsByPower[13];
    for(uint8_t i = 0; i < 13; i++)numCardsByPower[i] = 0;

    // Get nums cards
    for(uint8_t i = 0; i < 5; i++) {
        // get pos
        uint8_t j = 0;
        while(ORDER_POWER_GOLD[j] != hand[i]) {
            j++;
        }
        numCardsByPower[j]++;
    }

    uint8_t numJokers = numCardsByPower[12];
    // Convert jokers into best card
    uint8_t positionWMoreCards = 0;
    uint8_t moreCards = 0;
    for(uint8_t i = 0; i < 12; i++) {
        if(numCardsByPower[i] > moreCards){
            moreCards = numCardsByPower[i];
            positionWMoreCards = i;
        }
    }
    // If jokers are not the best card, sum the jokers to the best position
    if(positionWMoreCards != 12) {
        numCardsByPower[positionWMoreCards] += numJokers;
        numCardsByPower[12] = 0; // remove jokers
    }

    bool thereAreThree = false;
    uint32_t numPairs = 0;
    for(uint8_t i = 0; i < 13; i++) {
        // Five of a kind
        if(numCardsByPower[i] == 5)return HandType::fiveKind;
        // Four of a kind
        if(numCardsByPower[i] == 4)return HandType::fourKind;

        if(numCardsByPower[i] == 3)thereAreThree = true;
        if(numCardsByPower[i] == 2)numPairs++;
    }

    if(thereAreThree && numPairs == 1)return HandType::fullHouse;

    if(thereAreThree && numPairs == 0)return HandType::ThreeKind;

    if(numPairs == 2)return HandType::TwoPair;
    if(numPairs == 1)return HandType::OnePair;

    return HandType::HighCard;
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

    // Build power map
    for(uint32_t i = 0; i < 256; i++) powerMap[i] = 0;
    //'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'
    powerMap['2'] = 1;
    powerMap['3'] = 2;
    powerMap['4'] = 3;
    powerMap['5'] = 4;
    powerMap['6'] = 5;
    powerMap['7'] = 6;
    powerMap['8'] = 7;
    powerMap['9'] = 8;
    powerMap['T'] = 9;
    powerMap['J'] = 10;
    powerMap['Q'] = 11;
    powerMap['K'] = 12;
    powerMap['A'] = 13;
    for(uint32_t i = 0; i < 256; i++) powerMapJokers[i] = 0;
    //'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'
    powerMapJokers['J'] = 1;
    powerMapJokers['2'] = 2;
    powerMapJokers['3'] = 3;
    powerMapJokers['4'] = 4;
    powerMapJokers['5'] = 5;
    powerMapJokers['6'] = 6;
    powerMapJokers['7'] = 7;
    powerMapJokers['8'] = 8;
    powerMapJokers['9'] = 9;
    powerMapJokers['T'] = 10;
    powerMapJokers['Q'] = 11;
    powerMapJokers['K'] = 12;
    powerMapJokers['A'] = 13;
    
    vector<HandBid> vecHands;
    vector<HandBid> vecHandsJoker;

    // Get times
    string line;
    while(getline(inputFile, line)) {
        HandBid handB;
        vector<string> split = splitString(line, " ");
        if(split[0].length() != 5) {
            exit(1);
        }
        for(u_int8_t i = 0; i < 5; i++) {
            handB.hand[i] = split[0][i];
        }
        handB.bid = atoi(split[1].c_str());
        handB.type = getHandType(handB.hand);

        vecHands.push_back(handB);
    }

    for(const HandBid& hand: vecHands) {
        HandBid jokerHand;
        for(u_int8_t i = 0; i < 5; i++) jokerHand.hand[i] = hand.hand[i];
        jokerHand.bid = hand.bid;
        jokerHand.type = getHandTypeJokers(jokerHand.hand);
        vecHandsJoker.push_back(jokerHand);
    }

    // Debug
    for(const HandBid& hand: vecHandsJoker) {
        hand.print();
        cout << "\n";
    }

    cout << "Silver: " << getSilver(vecHands) << "\n";
    cout << "Gold: "  << getGold(vecHandsJoker) << "\n";
    return 0;
}