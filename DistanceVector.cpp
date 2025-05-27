#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <set>
#include <limits>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iomanip>

using namespace std;

const int INF = numeric_limits<int>::max();

unordered_map<char, unordered_map<char, int>> costedge;
unordered_map<char, unordered_map<char, int>> distedge;
unordered_map<char, unordered_map<char, char>> nextedge;
set<char> router;


/* Main to read the file from command line*/
int main () {
    cout << "Hello World \n";

}