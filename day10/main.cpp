#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <set>
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

struct Position {
    uint32_t x;
    uint32_t y;

    bool operator==(const Position& other) {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) {
        return x != other.x || y != other.y;
    }

    bool operator<(const Position& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};

struct Offset {
    int32_t x;
    int32_t y;

    bool operator==(const Offset& other) {
        return x == other.x && y == other.y;
    }
};

struct ConnectionPipe {
    char pipeChar;
    Offset offsetConnection1;
    Offset offsetConnection2;

    ConnectionPipe(char pipe, Offset off1, Offset off2): pipeChar(pipe), offsetConnection1(off1), offsetConnection2(off2) {};
};

struct TilePipe {
    ConnectionPipe* type;
    Position position;
};

unordered_map<char, ConnectionPipe*> typesConnections;

void printMapWithConnections(const char* map, uint32_t width, uint32_t height) {
    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            char tile = map[x + y*width];
            switch (tile) {
                case '|':
                    cout << "║";
                    break;
                case '-':
                    cout << "═";
                    break;
                case 'L':
                    cout << "╚";
                    break;
                case 'J':
                    cout << "╝";
                    break;
                case '7':
                    cout << "╗";
                    break;
                case 'F':
                    cout << "╔";
                    break;
                case '.':
                    cout << ".";
                    break;
                case 'S':
                    cout << 'S';
                    break;
                case ' ':
                    cout << ' ';
                    break;
                default:
                    cout << "WAAAAAAAT?????";
            }
        }
        cout << "\n";
    }
}

Position getStartPosition(const char* map, uint32_t width, uint32_t height) {
     for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            if(map[x + y*width] == 'S')return Position{x, y};
        }
    }
    cout << "This clearly should not happend" << "\n";
    return {0,0};
}

struct PipesConnections {
    PipesConnections* previous;
    PipesConnections* next;
    
    TilePipe* pipe;
};

// checks if can go to a pipe from a previous position
bool canGoToPipe(char tile, const Offset& fromPosition, const unordered_map<char, ConnectionPipe*> typesConn) {
    Offset invPos = {-fromPosition.x, -fromPosition.y};
    ConnectionPipe* pipeToCheck = typesConn.at(tile);

    if(pipeToCheck->offsetConnection1 == invPos || pipeToCheck->offsetConnection2 == invPos)return true;

    return false;
}

char getCharMap(const char* map, const Position& pos, uint32_t width) {
    return map[pos.x + pos.y*width];
}

TilePipe* getTilePipe(const char* map, const Position& pos, uint32_t width, const unordered_map<char, ConnectionPipe*>& typesConnections) {
    char next = getCharMap(map, pos, width);
    ConnectionPipe* typeConnNext = typesConnections.at(next);

    TilePipe* tilePipe = new TilePipe();
    tilePipe->position = pos;
    tilePipe->type = typeConnNext;

    return tilePipe;
}

void connectLinkedListPipes(PipesConnections* startTileConn, const char* map, uint32_t width, const unordered_map<char, ConnectionPipe*>& typesConnections) {
    // Previous, current and next are filled
    Position startPosition = startTileConn->pipe->position;
    PipesConnections* curr = startTileConn->next;

    Position nextPos = curr->pipe->position;

    
    while(startPosition != nextPos) {
        // Get next offsetConnection
        Offset offsetNext;

        Offset offsetPrevious;
        offsetPrevious.x = curr->previous->pipe->position.x - curr->pipe->position.x;
        offsetPrevious.y = curr->previous->pipe->position.y - curr->pipe->position.y;
        if(curr->pipe->type->offsetConnection1 == offsetPrevious) {
            offsetNext = curr->pipe->type->offsetConnection2;
        }else{
            offsetNext = curr->pipe->type->offsetConnection1;
        }
        
        nextPos.x = curr->pipe->position.x + offsetNext.x;
        nextPos.y = curr->pipe->position.y + offsetNext.y;
        if(nextPos != startPosition) {
            // Add pipe
            TilePipe* nextTilePipe = getTilePipe(map, nextPos, width, typesConnections);
            cout << "Adding pipe: " << nextTilePipe->type->pipeChar << "\n";
            PipesConnections* nextConn = new PipesConnections();
            nextConn->pipe = nextTilePipe;
            nextConn->previous = curr;
            nextConn->next = nullptr;

            curr->next = nextConn;

            curr = nextConn;
        }else{
            // complete linked list
            curr->next = startTileConn;
            startTileConn->previous = curr;
        }
    }
}



void makeLinkedListPipes(PipesConnections* startPipe, const char* map, uint32_t width, const unordered_map<char, ConnectionPipe*>& typesConnections) {
    // Get next
    Offset offNext = startPipe->pipe->type->offsetConnection1;
    Position nextPos = startPipe->pipe->position;
    nextPos.x += offNext.x;
    nextPos.y += offNext.y;

    TilePipe* nextTilePipe = getTilePipe(map, nextPos, width, typesConnections);

    PipesConnections* nextPipe = new PipesConnections();
    nextPipe->pipe = nextTilePipe;
    nextPipe->previous = startPipe;
    nextPipe->next = nullptr;

    startPipe->next = nextPipe;

    // Connect pipes
    connectLinkedListPipes(startPipe, map, width, typesConnections);
}

uint32_t getSilver(const PipesConnections* start)  {
    PipesConnections* fromPrev = start->previous;
    PipesConnections* fromNext = start->next;

    uint32_t steps = 0;
    while(fromPrev != fromNext) {
        fromPrev = fromPrev->previous;
        fromNext = fromNext->next;
        steps++;
    }

    return steps;
}

void setMapInflated(const Position& posMapNormal, char* mapInfl, uint32_t widthInfl, uint32_t heightInfl, char tile) {
    Position leftCorner = {posMapNormal.x*3, posMapNormal.y*3};
    
    // set corners
    mapInfl[leftCorner.x + leftCorner.y*widthInfl] = ' ';
    mapInfl[leftCorner.x+2 + leftCorner.y*widthInfl] = ' ';
    mapInfl[leftCorner.x + (leftCorner.y+2)*widthInfl] = ' ';
    mapInfl[leftCorner.x+2 + (leftCorner.y+2)*widthInfl] = ' ';

    // set sides
    mapInfl[leftCorner.x+1 + leftCorner.y*widthInfl] = ' ';
    mapInfl[leftCorner.x + (leftCorner.y+1)*widthInfl] = ' ';
    mapInfl[leftCorner.x+2 + (leftCorner.y+1)*widthInfl] = ' ';
    mapInfl[leftCorner.x+1 + (leftCorner.y+2)*widthInfl] = ' ';

    // set center
    mapInfl[leftCorner.x+1 + (leftCorner.y+1)*widthInfl] = tile;

    switch (tile) {
        case '|':
            mapInfl[leftCorner.x+1 + (leftCorner.y)*widthInfl] = '|';
            mapInfl[leftCorner.x+1 + (leftCorner.y+2)*widthInfl] = '|';
            break;
        case '-':
            mapInfl[leftCorner.x + (leftCorner.y+1)*widthInfl] = '-';
            mapInfl[leftCorner.x+2 + (leftCorner.y+1)*widthInfl] = '-';
            break;
        case 'L':
            mapInfl[leftCorner.x+1 + (leftCorner.y)*widthInfl] = '|';
            mapInfl[leftCorner.x+2 + (leftCorner.y+1)*widthInfl] = '-';
            break;
        case 'J':
            mapInfl[leftCorner.x+1 + (leftCorner.y)*widthInfl] = '|';
            mapInfl[leftCorner.x + (leftCorner.y+1)*widthInfl] = '-';
            break;
        case '7':
            mapInfl[leftCorner.x+1 + (leftCorner.y+2)*widthInfl] = '|';
            mapInfl[leftCorner.x + (leftCorner.y+1)*widthInfl] = '-';
            break;
        case 'F':
            mapInfl[leftCorner.x+1 + (leftCorner.y+2)*widthInfl] = '|';
            mapInfl[leftCorner.x+2 + (leftCorner.y+1)*widthInfl] = '-';
            break;
        case '.':
            break;
        default:
            cout << "ERROR\n";
            break;
    }
}

bool isInside(const Position& p, uint32_t width, uint32_t height) {
    return p.x < width && p.y < height;
}

uint32_t getGold(const char* map, uint32_t width, uint32_t height) {
    uint32_t tilesTotal = 0;
    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            if(map[x + width*y] == '.') {
                tilesTotal++;
            }
        }
    }

    uint32_t tilesOutside = 0;
    set<Position> tilesSearched;
    queue<Position> positionsToSearch;

    positionsToSearch.push({0,0}); // put left north corner
    tilesSearched.insert({0,0});

    while(!positionsToSearch.empty()) {
        Position posToCheck = positionsToSearch.front();
        positionsToSearch.pop();
        
        if(map[posToCheck.x + width*posToCheck.y] == '.')tilesOutside++;

        

        // get next tiles
        Position nextP;
        for(int8_t y = -1; y < 2; y++) {
            for(int8_t x = -1; x < 2; x++) {
                nextP.x = posToCheck.x + x;
                nextP.y = posToCheck.y + y;
                if(isInside(nextP, width, height)) {
                    if(tilesSearched.find(nextP) == tilesSearched.end()) { // If the next position to check is not in our set
                        char nextTile = map[nextP.x + width*nextP.y];
                        if(nextTile == ' ' || nextTile == '.') {
                            positionsToSearch.push(nextP);
                            tilesSearched.insert(nextP);
                        }
                    }
                    
                }
            }
        }
    }


    return tilesTotal - tilesOutside;
}

bool isPositionInsideLoop(const PipesConnections* start, const Position& p) {
    PipesConnections* curr = start->next;
    if(start->pipe->position == p) return true;

    while(curr != start) {
        if(curr->pipe->position == p)return true;
        curr = curr->next;
    }
    return false;
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
    
    // Pipe connections
    typesConnections.insert({'|', new ConnectionPipe('|', {0, -1}, {0, 1})});
    typesConnections.insert({'-', new ConnectionPipe('-', {-1, 0}, {1, 0})});
    typesConnections.insert({'L', new ConnectionPipe('L', {1, 0}, {0, -1})}); // North-east
    typesConnections.insert({'J', new ConnectionPipe('J', {-1, 0}, {0, -1})}); // North-west
    typesConnections.insert({'7', new ConnectionPipe('7', {-1, 0}, {0, 1})}); // South-west
    typesConnections.insert({'F', new ConnectionPipe('F', {1, 0}, {0, 1})}); // South-east
    

    uint32_t width = 0, height = 0;
    char* map;

    // Get width and height
    std::string line;
    if(std::getline(inputFile, line)) width = line.length();
    while(std::getline(inputFile, line)) {
        height++;
    }
    height++;


    // Inflate sides
    width += 2;
    height += 2;
    map = new char[width*height];
    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            map[x + width*y] = ' ';
        }
    }


    // Read map
    inputFile.clear();
    inputFile.seekg(0);
    uint32_t y = 1;
    while(std::getline(inputFile, line)) {
        for(uint32_t x = 0; x < width-2; x++) {
            char t = line[x];
            map[x+1 + width*y] = t;
        }
        y++;
    }

    // Get start position
    Position startPosition = getStartPosition(map, width, height);
   

    //demo hardcoded
    //map[startPosition.x + startPosition.y*width] = '7';

    //input hardcoded
    map[startPosition.x + startPosition.y*width] = '|';

   

    PipesConnections* start;


    TilePipe* startTile = new TilePipe();
    startTile->position = startPosition;
    // Get What type of pipe is ours

    //N
    
    //S

    //W

    //E


    //demo hardcoded
    //startTile->type = typesConnections.at('7');

    //input hardcoded
    startTile->type = typesConnections.at('|');

    // Double linked list
    start = new PipesConnections();
    start->pipe = startTile;

    makeLinkedListPipes(start, map, width, typesConnections);

    printMapWithConnections(map, width, height);
    
    // put points on tiles that do not belong to the connection for gold
    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            Position p = {x, y};
            if(!isPositionInsideLoop(start, p)) {
                if(map[x + width*y] != '.' || map[x + width*y] != ' '){
                    map[x + width*y] = '.';
                }
            }
        }
    }



    // Inflate map
    uint32_t widthInfl = width*3;
    uint32_t heightInfl = height*3;
    // Inflate map
    char* mapInfl = new char[widthInfl*heightInfl];
    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            char tile = map[x + width*y];
            setMapInflated({x, y}, mapInfl, widthInfl, heightInfl, tile);
        }
    }

    printMapWithConnections(mapInfl, widthInfl, heightInfl);

    cout << "Silver: " <<  getSilver(start) <<"\n";
    cout << "Gold: "  << getGold(mapInfl, widthInfl, heightInfl) << "\n";

    // Clean up
    delete[] map;

    return 0;
}