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

/* Initialize dist and nextHop */
void Init_edges() {
    for (char u : router) {
        for (char v : router) {
            if (u == v) {
                distedge[u][v] = 0; /* same router */
                nextedge[u][v] = '-';
            } else if (costedge[u].count(v)) { /* if there is a value */
                distedge[u][v] = costedge[u][v]; 
                nextedge[u][v] = v;
            } else {
                distedge[u][v] = INF; /* Initialize other paths to Infinite */
                nextedge[u][v] = '-';
            }
            cout << " source " << u << " destination " << v << " cost " << distedge[u][v] << " nextedge " << nextedge[u][v] << "\n";
        }
    }
}

/* Load input from file */
/* Create FSM based on the data from the file*/
/* There are 3 main states, and updates needs to be done based on the state*/
void loadInput(const string& filename) {
    string rline;
    char r;
    char a, b;
    int c;
    ifstream rfile(filename);
    STATE state = GET_EDGE;
    

    if (rfile.is_open()) {
       // Going Through the state machine
       while (getline(rfile, rline)) {
        if (rline.rfind("START", 0) == 0) {
            state = START_GRAPH;
            continue;
        } else if (rline.rfind("UPDATE", 0) == 0) {
            state = UPDATE_GRAPH;
            Init_edges();
            continue;
        } else if (rline.rfind("END", 0) == 0) {
            break;
        }

        istringstream splitline(rline);

        if (state == GET_EDGE) {
            if (splitline >> r) {
                router.insert(r);
            }
        } else if (state == START_GRAPH || state == UPDATE_GRAPH) {
            // get the weight between 2 routers
            if (splitline >> a >> b >> c) {
            if (router.find(a) == router.end()) { /* Insert node only if it is not existing*/
                  router.insert(a);
                }
                if (router.find(b) == router.end()) {
                   router.insert(b);
                }
                if (c == -1) {
                  costedge[a].erase(b);
                  costedge[b].erase(a);  /* Erase weight if it is -1*/
                } else {
                  costedge[a][b] = c;
                  costedge[b][a] = c;
                }
            }
        }
    }
    rfile.close();
    Init_edges();
  } else {
    cout << "ERROR File cannot be opened\n";
    exit(1);
  }
}

/* Main to read the file from command line*/
int main (int argc, char* argv[]) {
    if (argc != 2) {
        return(1);
    }

    loadInput(argv[1]);
    return(0);
}