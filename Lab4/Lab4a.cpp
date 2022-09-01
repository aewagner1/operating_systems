#include <iostream>
#include <fstream>
#include <vector> 
#include <algorithm>
#include <iomanip>

#include <math.h>

using namespace std;


// define triangle struct
struct triangle {
    double area;
    double distA; // (a, b) => (c, d)
    double distB; // (c, d) => (e, f)
    double distC; // (e, f) => (a, b)
};
typedef struct triangle triangle;

// calculate area of triangle
double area(int a, int b, int c, int d, int e, int f) {
    return abs(((c - a) * (f - b) - (d - b) * (e - a))) / 2.0;
}


double distance(int a, int b, int c, int d) {
    return sqrt((c - a) * (c - a) + (d - b) * (d - b));
}


void processTriangles(string filename) {
 ifstream myfile;
    myfile.open(filename);

    int N; // Number of triangles, 3 points per row
    myfile >> N;

    cout << "Reading points from " << filename << endl;

    vector<triangle> triangles;

    int a, b, c, d, e, f;  // Cords (a, b), (c, d), (e, f) of 3 points
    for (int i = 0; i < N; i++) {
        myfile >> a >> b >> c >> d >> e >> f;
        triangle t;
        t.distA = distance(a, b, c, d);
        t.distB = distance(c, d, e, f);
        t.distC = distance(a, b, e, f);
        if (t.distA > t.distC)
            swap(t.distA, t.distC);
        if (t.distA > t.distB)
            swap(t.distA, t.distB);
        if (t.distB > t.distC)
            swap(t.distB, t.distC);
        t.area = area(a, b, c, d, e, f);
        triangles.push_back(t);
    }
    myfile.close();

    sort(triangles.begin(), triangles.end(),
        [](triangle t1, triangle t2) {return t1.area < t2.area; });

    cout << std::fixed;
    cout << std::setprecision(3);
    for (triangle t : triangles) cout <<t.distA << " " << t.distB << " " << t.distC << " " << t.area << endl;



}

int main() {
    processTriangles("triangles30.txt");
}