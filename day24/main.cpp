#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <vector>
#include <regex>
#include <unordered_map>
#include <set>
#include <cmath>
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
    int64_t x;
    int64_t y;
    int64_t z;

    bool operator==(const Position& p) const {
        return x == p.x && y == p.y && z == p.z;
    }

    bool operator!=(const Position& p) const {
        return x != p.x || y != p.y || z != p.z;
    }

    bool operator<(const Position& p) const {
        return x < p.x || (x == p.x && y < p.y) || (x == p.x && y == p.y && z < p.z) ;
    }
};

struct Delta {
    double x;
    double y;
    double z;

    bool operator==(const Delta& p) const {
        return x == p.x && y == p.y && z == p.z;
    }

    bool operator!=(const Delta& p) const {
        return x != p.x || y != p.y || z != p.z;
    }

    bool operator<(const Delta& p) const {
        return x < p.x || (x == p.x && y < p.y) || (x == p.x && y == p.y && z < p.z) ;
    }
};

struct Ray {
    Position pos;
    Delta delta;

    void print() const{
        cout << pos.x << ", " << pos.y << ", " << pos.z << " @ " << delta.x << ", " << delta.y << ", " << delta.z; 
    }
};

struct PositionHash {
    std::size_t operator()(const Position& p) const {
        return hash<uint32_t>{}(p.x) ^ hash<uint32_t>{}(p.y) ^ hash<uint32_t>{}(p.z);
    }
};

double getMagnitude2D(const Ray& r) {
    return std::sqrt(r.delta.x * r.delta.x + r.delta.y * r.delta.y);
}

Delta getNormalizedDir2D(const Ray& r) {
    double magnitude = getMagnitude2D(r);
    return {r.delta.x / magnitude, r.delta.y / magnitude, 0.0};
}

struct Offset{
    int32_t x;
    int32_t y;

    bool operator==(const Offset& p) const {
        return x == p.x && y == p.y;
    }

    bool operator!=(const Offset& p) const {
        return x != p.x || y != p.y;
    }

    bool operator<(const Offset& p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
};

double crossProd(double x1, double x2, double y1, double y2) {
    return (x1 * y2) - (y1 * x2);
}

Delta calculateRayIntersection(const Ray& r1, const Ray& r2) {
    Delta dir1 = getNormalizedDir2D(r1);
    Delta dir2 = getNormalizedDir2D(r2);

    double determinant = crossProd(dir1.x, dir1.y, dir2.x, dir2.y);

    if (determinant == 0) {
        return {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), 0};
    }

    Delta delta = {(double)r2.pos.x - r1.pos.x, (double)r2.pos.y - r1.pos.y, 0};

    double t = crossProd(delta.x, delta.y, dir2.x, dir2.y) / determinant;
    double u = crossProd(delta.x, delta.y, dir1.x, dir1.y) / determinant;

    if(t < 0 && u < 0) {
         return {-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), 0};
    }

    if(t < 0) {
        return {-std::numeric_limits<double>::infinity(), 0, 0};
    }

    if(u < 0) {
        return {0, -std::numeric_limits<double>::infinity(), 0};
    }

    // Calculate the intersection point
    Delta intersection = {r1.pos.x + t * dir1.x, r1.pos.y + t * dir1.y, 0};
    return intersection;
}

int64_t getSilver(const vector<Ray>& rays) {
    int64_t silver = 0;
    int64_t MIN = 200000000000000;
    int64_t MAX = 400000000000000;

    for(size_t i = 0; i < rays.size(); i++) {
        for(size_t j = i+1; j < rays.size(); j++) {
            if(i == j)continue;

            Delta intersectPoint = calculateRayIntersection(rays[i], rays[j]);
            cout << "Hailstone A: ";
            rays[i].print();
            cout << "\nHailstone B: ";
            rays[j].print();
            cout << "\n";
            
            if(intersectPoint.x != std::numeric_limits<double>::infinity()) {
                if(intersectPoint.x == -std::numeric_limits<double>::infinity() &&
                    intersectPoint.y == -std::numeric_limits<double>::infinity()) {
                    cout << "Hailstones' paths crossed in the past for both hailstones.";
                }else if(intersectPoint.x == -std::numeric_limits<double>::infinity()) {
                    cout << "Hailstones' paths crossed in the past for hailstone A.";
                }else if(intersectPoint.y == -std::numeric_limits<double>::infinity()) {
                    cout << "Hailstones' paths crossed in the past for hailstone B.";
                }else if(intersectPoint.x >= MIN && intersectPoint.x <= MAX  
                && intersectPoint.y >= MIN && intersectPoint.y <= MAX) {
                    silver++;
                    cout << "Hailstones' paths will cross inside the test area (at x=" << intersectPoint.x << ", " << "y=" << intersectPoint.y << ").";
                }else{
                    cout << "Hailstones' paths will cross outside the test area (at x=" << intersectPoint.x << ", " << "y=" << intersectPoint.y << ").";
                }
            }else{
                cout << "Hailstones' paths are parallel; they never intersect.";
            }

            cout << "\n\n";
        }
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

    vector<Ray> rays;

    string line;
    while(std::getline(inputFile, line)) {
        vector<string> posDeltaSplit = splitString(line, " @ ");
        vector<string> positions = splitString(posDeltaSplit[0], ",");
        vector<string> deltas = splitString(posDeltaSplit[1], ",");

        Ray ray;
        ray.pos.x = stol(positions[0]);
        ray.pos.y = stol(positions[1]);
        ray.pos.z = stol(positions[2]);
        ray.delta.x = stoi(deltas[0]);
        ray.delta.y = stoi(deltas[1]);
        ray.delta.z = stoi(deltas[2]);

        rays.push_back(ray);
    }

    // debug
    cout << "Rays: " << "\n";
    for(const Ray& r: rays) {
        r.print();
        cout << "\n";
    }

    cout << "Silver: " << getSilver(rays) << "\n";
    cout << "Gold: " << "\n"; // Run it on an online solver, guess I should know how to solve this types of systems
    return 0;
}