#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <set>
#include <limits>
#include <string>
#include <sstream>

using namespace std;

const int INF = numeric_limits<int>::max();

unordered_map<char, unordered_map<char, int>> costedge;
unordered_map<char, unordered_map<char, int>> distedge;
unordered_map<char, unordered_map<char, char>> nextedge;
set<char> router;

enum STATE {
    GET_EDGE,
    START_GRAPH,
    UPDATE_GRAPH
};

// Load input from file
void loadInput(const string& filename) {
    string rline;
    char r;
    char a, b;
    int c;
    ifstream rfile(filename);
    STATE = GET_EDGE;
    

    if (rfile.is_open()) {
       // Going Through the state machine
       while (getline(rfile, rline)) {
        if (rline.rfind("START", 0) == 0) {
            STATE = START_GRAPH;
            continue;
        } else if (line.rfind("UPDATE", 0) == 0) {
            state = UPDATE_GRAPH;
            continue;
        } else if (line.rfind("END", 0) == 0) {
            break;
        }

        istringstream splitline(line);
        char a, b;
        int c;

        if (state == GET_EDGE) {
            if (splitline >> r) {
                router.insert(r);
            }
        } else if (state == START_GRAPH || state == UPDATE_GRAPH) {
            // get the weight between 2 routers
            if (splitline >> a >> b >> c) {
                router.insert(a);
                router.insert(b);
                costedge[a][b] = c;
                costedge[b][a] = c;  // Undirected graph
            }
        }
    }
    file.close();
  } else {
    cout << "ERROR File cannot be opened\n";
    exit(1);
  }
}
