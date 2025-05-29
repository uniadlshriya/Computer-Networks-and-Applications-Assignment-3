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
            } else if (costedge[u].count(v)) { /* if there is a direct edge */
                distedge[u][v] = costedge[u][v];
                nextedge[u][v] = v;
            } else {
                distedge[u][v] = INF; /* Initialize other paths to Infinite */
                nextedge[u][v] = '-';
            }
        }
    }
}

void printDistanceTable(int t) {
    for (char s : router) {
        cout << "Distance Table of router " << s << " at t=" << t << ":\n";
        cout << "    ";
        for (char d : router)
            if (d != s) cout << d << " ";
        cout << "\n";

        for (char v : router) {
            if (v == s) continue;
            cout << v << "   ";
            for (char d : router) {
                if (d == s) continue;

                if (t == 0) {
                    // At t=0 print only direct edge cost from s to d, else INF
                    if (v == d && costedge[s].count(d)) {
                        cout << setw(3) << costedge[s][d] << " ";
                    } else {
                        cout << "INF ";
                    }
                } else {
                    // For t>0 show cost through neighbor v: cost(s->v) + dist(v->d)
                    if (costedge[s].count(v) && distedge[v].count(d)) {
                        if (distedge[v][d] == INF)
                            cout << "INF ";
                        else
                            cout << setw(3) << costedge[s][v] + distedge[v][d] << " ";
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

void printRoutingTable() {
    for (char s : router) {
        cout << "Routing Table of router " << s << ":\n";
        for (char d : router) {
            if (d == s) continue;
            if (distedge[s][d] == INF)
                cout << d << ",INF,INF\n";
            else
                cout << d << "," << nextedge[s][d] << "," << distedge[s][d] << "\n";
        }
        cout << "\n";
    }
}

/* Distance Vector Algorithm */
void runDistanceVector() {
    bool updated;
    int t = 0; /* time t = 0 in the beginning */
    int minDist;
    char nextHop;
    printDistanceTable(t);
    do {
        t++;
        updated = false;
        for (char a : router) {
            for (char b : router) {
                if (a == b) continue;
                minDist = distedge[a][b];
                nextHop = nextedge[a][b];

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

/* Load input from stdin and process states */
void loadInput() {
    string rline;
    char r;
    char a, b;
    int c;
    STATE state = GET_EDGE;

    while (getline(cin, rline)) {
        if (rline.rfind("START", 0) == 0) {
            state = START_GRAPH;
            continue;
        } else if (rline.rfind("UPDATE", 0) == 0) {
            // Before applying updates, run DV on current graph and print results
            Init_edges();
            runDistanceVector();
            state = UPDATE_GRAPH;
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
                if (router.find(a) == router.end()) router.insert(a);
                if (router.find(b) == router.end()) router.insert(b);

                if (c == -1) {
                    costedge[a].erase(b);
                    costedge[b].erase(a);
                } else {
                    costedge[a][b] = c;
                    costedge[b][a] = c;
                }
            }
        }
    }

    // After all input processed, run DV on final graph and print results
    Init_edges();
    runDistanceVector();
}

int main () {
    loadInput();
    return 0;
}
