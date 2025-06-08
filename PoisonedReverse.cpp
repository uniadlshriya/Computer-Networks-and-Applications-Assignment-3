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
       cout << "Distance Table of router " << s << " at t=" << t << ":\n";
        cout << "     "; 
        for (char v : router)
            if (v != s) cout << v << "    " ;
         cout << "\n\n"; 

        for (char d : router) {
            if (d == s) continue;
               cout << d << "    "; 

            for (char v : router) {
                if (v == s) continue;

                if (t == 0) {
                    if (v == d && costedge[s].count(d)) {
                          cout << setw(3) << costedge[s][d] << "  "; 
                    } else {
                           cout << "INF  "; 
                    }
                } else {
                     // Poisoned Reverse: if s reaches d via v, poison it when sending to v
                    if (nextedge[s][d] == v) {
                        cout << "INF  ";
                        continue;
                    }

                    if (costedge[s].count(v) &&
                        snapshotDist.count(v) &&
                        snapshotDist.at(v).count(d) &&
                        snapshotDist.at(v).at(d) != INF) {

                        int dist = costedge[s][v] + snapshotDist.at(v).at(d);
                        cout << setw(3) << dist << "  ";
                    } else {
                        cout << "INF  ";
                    }
                }
            }
                cout << "\n"; 
        }
             cout << "\n\n";
    }
}

/* Print final routing table */
void printRoutingTable() {
    for (char s : router) {
        cout << "Routing Table of router " << s << ":\n"; 
        for (char d : router) {
            if (d == s)
                continue;
            if (distedge[s][d] == INF) {
                cout << d << ",INF,INF\n";
            } else {
                cout << d << "," << nextedge[s][d] << "," << distedge[s][d] << "\n";
            }
        }
        cout << "\n";
    }
}

void runDistanceVector(int t) {
    unordered_map<char, unordered_map<char, int>> prevDist = distedge;
    unordered_map<char, unordered_map<char, int>> newDist = distedge;
    unordered_map<char, unordered_map<char, char>> newNext = nextedge;

    for (char a : router) {
        for (char b : router) {
            if (a == b) continue;

            int minDist = (costedge[a].count(b) ? costedge[a][b] : INF);
            char bestNextHop = (costedge[a].count(b) ? b : '-');

            for (char v : router) {
                if (!costedge[a].count(v)) continue; // v must be a neighbor of a

                //  Poisoned Reverse: skip route info that goes back to the neighbor
                if (nextedge[a][b] == v) continue;

                if (!prevDist.count(v) || !prevDist[v].count(b) || prevDist[v][b] == INF) continue;

                int alt = costedge[a][v] + prevDist[v][b];
                if (alt < minDist) {
                    minDist = alt;
                    bestNextHop = v;
                }
            }

            newDist[a][b] = minDist;
            newNext[a][b] = (minDist == INF ? '-' : bestNextHop);
        }
    }

    printDistanceTable(t, prevDist);
    distedge = newDist;
    nextedge = newNext;
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
    bool updated = false;
   
    while (getline(cin, rline)) 
    {
        if (rline.rfind("START", 0) == 0) 
        {
            state = START_GRAPH;
            continue;
        } else if (rline.rfind("UPDATE", 0) == 0) 
        {
            state = UPDATE_GRAPH;
            costvector.push_back(costedge);
            continue;
        } else if (rline.rfind("END", 0) == 0)
        {
            if (updated) 
                costvector.push_back(costedge);
            break;
        }

        istringstream splitline(rline);

        if (state == GET_EDGE) 
        {
            if (splitline >> r) 
            {
                router.insert(r);
            }
        } else if (state == START_GRAPH || state == UPDATE_GRAPH) 
        {
            if (splitline >> a >> b >> c) 
            {
                router.insert(a);
                router.insert(b);
                    
                if (c == -1) {
                    costedge[a].erase(b);
                    costedge[b].erase(a);
                } else {
                    costedge[a][b] = c;
                    costedge[b][a] = c;
                }            
            }
            if (state == UPDATE_GRAPH)
                updated = true;
        }
    }
    
    for (std:: size_t k = 0; k < costvector.size(); k++ )
    {
        costedge = costvector[k];
        if (t == 0) {
            distedge.clear();
            nextedge.clear();
            Init_edges();
        }
        for (std::size_t j = 0; j < router.size(); j++) {
            if (t==0)
                printDistanceTable(t, distedge);
            else
                runDistanceVector(t);
            t++;
        }
        printRoutingTable();
    }
}
/* Main */
int main() {
    loadInput();
    return 0;
}