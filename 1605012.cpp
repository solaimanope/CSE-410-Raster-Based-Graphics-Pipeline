#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <stack>
#include <iomanip>
using namespace std;
const double PI = acos(-1);

struct Point {
	double x, y, z, w;
    Point() : x(0), y(0), z(0), w(0) { }
    Point(double x, double y, double z) : x(x), y(y), z(z), w(0) { }
    Point(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) { }
    Point operator+(const Point &o) const { return Point(x+o.x, y+o.y, z+o.z, w+o.w); }
    Point operator-(const Point &o) const { return Point(x-o.x, y-o.y, z-o.z, w-o.w); }
    Point operator*(double v) const { return {x*v, y*v, z*v, w*v}; }
    double length() const { return sqrt(x*x+y*y+z*z+w*w); }
    void normalize() {
        double d = length();
        x /= d, y /= d, z /= d, w /= d;
    }
    void wnormalize() {
        if (w == 0) return;
        x /= w, y /= w, z /= w;
        w = 1;
    }
};
istream &operator>>(istream &in, Point &p) {
    return in >> p.x >> p.y >> p.z;
}
ostream &operator<<(ostream &out, Point &p) {
    out << fixed << setprecision(7);
    return out << p.x << " " << p.y << " " << p.z;
}
Point cross(const Point &p, const Point &q) {
    return Point( p.y*q.z - p.z*q.y, p.z*q.x - p.x*q.z, p.x*q.y - p.y*q.x );
}
double dot(const Point &p, const Point &q) {
    return p.x*q.x+p.y*q.y+p.z*q.z+p.w*q.w;
}

struct Matrix {
    double m[4][4];
    Matrix() {
        memset(m, 0, sizeof m);
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
    }
    Point row(int r) const {
        return Point(m[r][0], m[r][1], m[r][2], m[r][3]);
    }
    Point col(int c) const {
        return Point(m[0][c], m[1][c], m[2][c], m[3][c]);
    }
    Matrix operator*(const Matrix &other) const {
        Matrix rt;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                rt.m[i][j] = dot(row(i), other.col(j));
            }
        }
        return rt;
    }
    Point operator*(const Point &p) const {
        Point rt;
        rt.x = dot(row(0), p);
        rt.y = dot(row(1), p);
        rt.z = dot(row(2), p);
        rt.w = dot(row(3), p);
        return rt;
    }
};

Point R(Point x, Point a, double angle) {
    angle = angle * PI / 180;
    return x*cos(angle) + a*(1-cos(angle))*dot(a, x) + cross(a, x)*sin(angle);
}

const string SAMPLE = "Sample/1/";
const string OUTPUT = "Output/1/";

int main() {
    ifstream sceneIn;
    sceneIn.open(SAMPLE+"scene.txt");

    Point eye, look, up;
    sceneIn >> eye >> look >> up;
    double fovY, aspectRatio, near, far;
    sceneIn >> fovY >> aspectRatio >> near >> far;

    stack< Matrix >stk;
    stk.emplace();

    string command;
    ofstream stage1Out;
    stage1Out.open(OUTPUT+"stage1.txt");
    while (sceneIn >> command) {
        if (command == "triangle") {
            for (int i = 0; i < 3; i++) {
                Point vertex;
                sceneIn >> vertex;
                vertex.w = 1;
                vertex = stk.top() * vertex;
                vertex.wnormalize();
                stage1Out << vertex << endl;
            }
            stage1Out << endl;
        } else if (command == "translate") {
            Matrix t;
            sceneIn >> t.m[0][3] >> t.m[1][3] >> t.m[2][3];
            stk.top() = stk.top() * t;
        } else if (command == "scale") {
            Matrix s;
            sceneIn >> s.m[0][0] >> s.m[1][1] >> s.m[2][2];
            stk.top() = stk.top() * s;
        } else if (command == "rotate") {
            Matrix r;
            double angle; Point axis;
            sceneIn >> angle >> axis;
            axis.normalize();

            Point C1 = R(Point(1, 0, 0, 0), axis, angle);
            r.m[0][0] = C1.x, r.m[1][0] = C1.y, r.m[2][0] = C1.z;
            Point C2 = R(Point(0, 1, 0, 0), axis, angle);
            r.m[0][1] = C2.x, r.m[1][1] = C2.y, r.m[2][1] = C2.z;
            Point C3 = R(Point(0, 0, 1, 0), axis, angle);
            r.m[0][2] = C3.x, r.m[1][2] = C3.y, r.m[2][2] = C3.z;

            stk.top() = stk.top() * r;
        } else if (command == "push") {
            stk.push(stk.top());
        } else if (command == "pop") {
            stk.pop();
        } else if (command == "end") {
            break;
        }
    }

    sceneIn.close();
    stage1Out.close();

    return 0;
}