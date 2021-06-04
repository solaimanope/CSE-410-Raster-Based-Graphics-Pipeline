#include <iostream>
#include <fstream>
using namespace std;

const string SAMPLE = "Sample/1/";
int main() {
    ifstream sceneIn;
    sceneIn.open(SAMPLE+"scene.txt");

    string line;
    while (getline(sceneIn, line)) {
        cout << line << endl;
    }

    return 0;
}