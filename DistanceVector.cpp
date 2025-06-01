#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

const int INF = numeric_limits<int>::max();

unordered_map<char, unordered_map<char, int>> costedge;
/* added 2 unordered map for costs before update and cost after update
unordered_map<char, unordered_map<char, int>> costbeforeupdateedge;
unordered_map<char, unordered_map<char, int>> costafterupdateedge; */
unordered_map<char, unordered_map<char, int>> distedge;
unordered_map<char, unordered_map<char, char>> nextedge;
vector<unordered_map<char, unordered_map<char, int>>> costvector;
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
                distedge[u][v] = 0;
                nextedge[u][v] = '-';
            } else if (costedge[u].count(v)) {
                distedge[u][v] = costedge[u][v];
                nextedge[u][v] = v;
            } else {
                distedge[u][v] = INF;
                nextedge[u][v] = '-';
            }
        }
    }
}

// Print distance table for each router , also passing snapshot of distedge
void printDistanceTable(int t, const unordered_map<char, unordered_map<char, int>>& snapshotDist) {
    for (char s : router) {
       /* cout << "Distance Table of router " << s << " at t=" << t << ":\n";
        cout << "     "; */
        for (char v : router)
            if (v != s) /* cout << v << "    " */;
         cout << "\n\n"; 

        for (char d : router) {
            if (d == s) continue;
            /* cout << d << "    "; */

            for (char v : router) {
                if (v == s) continue;

                if (t == 0) {
                    if (v == d && costedge[s].count(d)) {
                        /* cout << setw(3) << costedge[s][d] << "  "; */
                    } else {
                        /* cout << "INF  "; */
                    }
                } else {
                    if (costedge[s].count(v) &&
                        snapshotDist.count(v) &&
                        snapshotDist.at(v).count(d) &&
                        snapshotDist.at(v).at(d) != INF) {
                        int dist = costedge[s][v] + snapshotDist.at(v).at(d);
                        /* cout << setw(3) << dist << "  "; */
                    } else {
                       /* cout << "INF  "; */
                    }
                }
            }
            /* cout << "\n"; */
        }
        /* cout << "\n\n"; */
    }
}

/* Print final routing table */
void printRoutingTable() {
    for (char s : router) {
        /* cout << "Routing Table of router " << s << ":\n"; */
        for (char d : router) {
            if (d == s)
                continue;
            if (distedge[s][d] == INF) {
                /* cout << d << ",INF,INF\n"; */
            } else {
                /* cout << d << "," << nextedge[s][d] << "," << distedge[s][d] << "\n"; */
            }
        }
        /* cout << "\n"; */
    }
}

void runDistanceVector(int t) {
    // Take a deep copy snapshot before updates
    unordered_map<char, unordered_map<char, int>> prevDist = distedge; // Old Distance before update
    unordered_map<char, unordered_map<char, int>> newDist = distedge; // New Distance after update
    unordered_map<char, unordered_map<char, char>> newNext = nextedge; // New Edge

    for (char a : router) {
        for (char b : router) {
            if (a == b) continue;

            int minDist = prevDist[a][b];  // Start with current known distance
            char bestNextHop = nextedge[a][b];

            for (char v : router) {
                if (costedge[a].count(v) && prevDist[v][b] != INF) {
                    int alt = costedge[a][v] + prevDist[v][b];
                    if (alt < minDist) {
                        minDist = alt;
                        bestNextHop = v;
                    }
                }
            }
            
            newDist[a][b] = minDist;
            newNext[a][b] = bestNextHop;
        }
    }

    // Only now commit the updates
    printDistanceTable(t, prevDist);
    distedge = newDist;
    nextedge = newNext;

    //printDistanceTable(t);
    printRoutingTable();
}

/* Load input from stdin continue getting till you get END */
/* Create FSM based on the data from stdin */
/* There are 3 main states, and updates needs to be done based on the state*/
void loadInput() {
    string rline;
    char r, a, b;
    int c;
    int t = 0;
    int count = 0;
    STATE state = GET_EDGE;
   
    while (getline(cin, rline)) {
        cout << "Input is " << rline << "\n";
        if (rline.rfind("START", 0) == 0) {
            state = START_GRAPH;
            continue;
        } else if (rline.rfind("UPDATE", 0) == 0) {
            state = UPDATE_GRAPH;
            costvector.push_back(costedge);
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
            if (splitline >> a >> b >> c) {
                router.insert(a);
                router.insert(b);
                if (c == -1) {
                    costedge[a].erase(b);
                    costedge[b].erase(a);
                } else {
                    costedge[a][b] = c;
                    costedge[b][a] = c;
                }
                if (state == START_GRAPH) 
                   count++;
                if (state == UPDATE_GRAPH)
                    costvector.push_back(costedge);
            }
        }
    }
    for (std::size_t i = 0; i < costvector.size(); i++)
    {
        costedge = costvector[i];
        if (t == 0) {
            distedge.clear();
            nextedge.clear();
            Init_edges();
            printDistanceTable(t, distedge);
        }
        if (i == 0) {
            for (int j = 0; j < count; j++) {
                t++;
                runDistanceVector(t);
            }
        }
        t++;
        runDistanceVector(t);
    } 
}

/* Main */
int main() {
    loadInput();
    return 0;
}

