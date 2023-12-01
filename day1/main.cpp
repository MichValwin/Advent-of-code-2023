#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>

static const char* INPUT_FILE = "input.txt";
static const char* DEMO_FILE = "demo.txt";
static const bool USE_REAL_DATA = true;

struct StringToChar{
    std::string str;
    char c;
};

const std::string NUMBERS[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};


uint32_t getSilver(std::vector<std::string>& lines) {
    for(std::string& line: lines) {
        std::cout << line << "\n";
    }

    std::vector<uint32_t> calibrationPerLine;
    for(std::string& line: lines) {
        char first = 0;
        char last = 0;
        for(char d: line) {
            if(std::isdigit(d)) {
                if(first == 0)first = d;
                last = d;
            }
        }
        if(first == last) {
            uint32_t number = (first-'0')*10 + (first-'0');
            calibrationPerLine.push_back(number);
        }else{
            uint32_t number = (first-'0')*10 + (last-'0');
            calibrationPerLine.push_back(number);
        }
    }

    std::cout << "----- \n";

    uint32_t silver = 0;
    for(uint32_t i: calibrationPerLine) {
        std::cout << i << "\n";
        silver += i;
    }
    return silver;
}

std::string substitudeString(const std::string& str, uint32_t start, uint32_t end, char substitude) {
    std::string before = str.substr(0, start);
    std::string after = str.substr(end, str.length());
    return before + substitude + after;
}


uint32_t getGold(std::vector<std::string>& lines) {
    std::vector<std::string> linesGoodNumbers;
    for(std::string& line: lines) {
        bool isThereSomething = true;
        size_t indexFound = std::string::npos;
        
        while(isThereSomething) {
            isThereSomething = false;
            std::vector<size_t> indexFounds;
            std::vector<char> charFounds;

            for(int i = 0; i < 9; i++) {
                const std::string toSearch = NUMBERS[i];
                indexFound = line.find(toSearch);
                if(indexFound != std::string::npos) {
                    indexFounds.push_back(indexFound);
                    isThereSomething = true;
                    char d = (i+1)+'0';
                    charFounds.push_back(d);
                }
            }
            
            if(isThereSomething) {
                size_t firstFound = std::string::npos;
                char subs = '0';
                for(size_t i = 0; i < indexFounds.size(); i++) {
                    if(indexFounds[i] < firstFound) {
                        firstFound = indexFounds[i];
                        subs = charFounds[i];
                    }
                }
                uint32_t indexChar = subs-'0'-1;
                line = substitudeString(line, firstFound, firstFound+NUMBERS[indexChar].length(), subs);
            }
        }
    }

    return getSilver(lines);
}

struct PositionChar{
    char c;
    size_t position;
};

uint32_t actualGold(std::vector<std::string>& lines) {
    const std::string lookAtTableNumbs[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
    const std::string lookAtTableString[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    
    uint32_t gold = 0;
    for(std::string& line: lines) {
        std::vector<PositionChar> positions;
        for(const std::string& numbs : lookAtTableNumbs) {
            size_t pos = line.find(numbs);
            if(pos != std::string::npos)positions.push_back({numbs[0], pos});
        }
        for(uint32_t i = 0; i < 9; i++) {
            size_t pos = line.find(lookAtTableString[i]);
            char c = (char)(i+1)+'0';
            if(pos != std::string::npos)positions.push_back({c, pos});
        }

         for(const std::string& numbs : lookAtTableNumbs) {
            size_t pos = line.rfind(numbs);
            if(pos != std::string::npos)positions.push_back({numbs[0], pos});
        }
        for(uint32_t i = 0; i < 9; i++) {
            size_t pos = line.rfind(lookAtTableString[i]);
            char c = (char)(i+1)+'0';
            if(pos != std::string::npos)positions.push_back({c, pos});
        }

        char firstChar = '0';
        size_t firsPosition = std::string::npos;
        char lastChar = '0';
        size_t lastPosition = 0;
        for(PositionChar& positionChar: positions) {
            if(positionChar.position < firsPosition){
                firsPosition = positionChar.position;
                firstChar = positionChar.c;
            }
            if(positionChar.position >= lastPosition){
                lastPosition = positionChar.position;
                lastChar = positionChar.c;
            }
        }
        uint32_t num = (firstChar-'0')*10 + (lastChar-'0');
        std::cout << "num: " << num << "\n";
        gold += num;

    }


    return gold;
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


    std::vector<std::string> lines;

    // Parse
    std::string line;
    while(std::getline(inputFile, line)) {
        lines.push_back(line);
    }

   // std::cout << "Silver: " << getSilver(lines) << "\n";
    uint32_t gold = actualGold(lines);
    std::cout << "Gold: " << gold << "\n";
    return 0;
}