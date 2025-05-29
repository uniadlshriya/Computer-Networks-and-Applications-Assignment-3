#include <iostream>
#include <unordered_map>
#include <set>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

const int INF = numeric_limits<int>::max();

unordered_map<char, unordered_map<char, int>> costedge;
/* added 2 unordered map for costs before update and cost after update*/
unordered_map<char, unordered_map<char, int>> costbeforeupdateedge;
unordered_map<char, unordered_map<char, int>> costafterupdateedge;
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

/* Print distance table at time t */
void printDistanceTable(int t) {
    for (char s : router) {
        cout << "Distance Table of router " << s << " at t=" << t << ":\n";
        cout << "    ";
        for (char d : router)
            if (d != s) cout << d << " ";
        cout << "\n";

        for (char v : router) {
            if (v == s)
                continue;
            cout << v << "   ";
            for (char d : router) {
                if (d == s) continue;

                if (t == 0) {
                    if (v == d && costedge[s].count(d)) {
                        cout << setw(3) << costedge[s][d] << " ";
                    } else {
                        cout << "INF ";
                    }
                } else {
                    // Only consider if v is a neighbor of s
                    if (costedge[s].count(v) && distedge[v].count(d)) {
                        int dist = (distedge[v][d] == INF) ? INF : costedge[s][v] + distedge[v][d];
                        if (dist == INF)
                            cout << "INF ";
                        else
                            cout << setw(3) << dist << " ";
                    } else {
                        cout << "INF ";
                    }
                }
            }
            cout << "\n";
        }
        cout << "\n";
    }
}

/* Print final routing table */
void printRoutingTable() {
    for (char s : router) {
        cout << "Routing Table of router " << s << ":\n";
        for (char d : router) {
            if (d == s)
                continue;
            if (distedge[s][d] == INF)
                cout << d << ",INF,INF\n";
            else
                cout << d << "," << nextedge[s][d] << "," << distedge[s][d] << "\n";
        }
        cout << "\n";
    }
}

/* Distance Vector Algorithm */
void runDistanceVector(int t) {
    bool updated;
    /*printDistanceTable(t); */ 

    do {
        updated = false;

        for (char a : router) {
            for (char b : router) {
                if (a == b) continue;

                int minDist = distedge[a][b];
                char nextHop = nextedge[a][b];

                for (char v : router) {
                    if (costedge[a].count(v) && distedge[v][b] != INF) {
                        int newDist = costedge[a][v] + distedge[v][b];
                        if (newDist < minDist) {
                            minDist = newDist;
                            nextHop = v;
                            updated = true;
                        }
                    }
                }

                distedge[a][b] = minDist;
                nextedge[a][b] = nextHop;
            }
        }
        printDistanceTable(t);
    } while (updated);

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
    STATE state = GET_EDGE;
   
    while (getline(cin, rline)) {
        if (rline.rfind("START", 0) == 0) {
            state = START_GRAPH;
            continue;
        } else if (rline.rfind("UPDATE", 0) == 0) {
            state = UPDATE_GRAPH;
            /*costbeforeupdateedge = costedge;  Storing cost at time T=0*/
            distedge.clear();
            nextedge.clear();
            Init_edges();
            printDistanceTable(0);
            continue;
        } else if (rline.rfind("END", 0) == 0) {
            costafterupdateedge = costedge; /* Storing cost at time t=1 this will be used for processing later*/;
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
                if (state == UPDATE_GRAPH) {
                    t++;
                    distedge.clear();
                    nextedge.clear();
                    Init_edges();
                    runDistanceVector(t);
                }
            }
        }
    }
}

/* Main */
int main() {
    loadInput();
    return 0;
}

