#include <iostream>
#include <fstream>
#include <vector> 
#include <algorithm>
#include <iomanip>

#include <math.h>

using namespace std;

struct point {
    int x;
    int y;
    double dist; // From origin
};
typedef struct point point;


double area(int a, int b, int c, int d, int e, int f) {
    return abs(((c - a) * (f - b) - (d - b) * (e - a))) / 2.0;
}


double distance(int a, int b, int c, int d) {
    return sqrt((c - a) * (c - a) + (d - b) * (d - b));
}


void processPoints(string filename) {
    ifstream myfile;
    myfile.open(filename);

    int N; // Number of points, one point per row
    myfile >> N;

    cout << "Reading points from " << filename << endl;

    vector<point> points;

    int a, b;  // Coords (a, b) of one point
    for (int i = 0; i < N; i++) {
        myfile >> a >> b;
        point p;
        p.x = a;
        p.y = b;
        p.dist = distance(0, 0, a, b);
        points.push_back(p);
    }
    myfile.close();

    sort(points.begin(), points.end(),
        [](point p1, point p2) {return p1.dist < p2.dist; });

    cout << std::fixed;
    cout << std::setprecision(3);
    for (point p : points) cout << p.x << " " << p.y << " " << p.dist << endl;

}

int main() {
    processPoints("points30.txt");
}