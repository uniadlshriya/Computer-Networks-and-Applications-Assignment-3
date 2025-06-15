// I have reused my code for Distance vector while making modifications for poisoned reverse.
// Since detailed comments are added into DistanceVector.cpp, I am mainly focusing on the differences here
//                                          Key Difference 
//  Feature                      Distance Vector                         Poisoned Reverse
// routing loops               No particular mechanism                 prevents by advertising unreachable nodes
// count to infinity           susceptible                             avoids or reduces the impact
// overhead                    DV shares only next hop information     PR advertises unreachable routes much larger routing table
// convergance time            slow                                    fast
// Robustness                  fragile                                 robust




#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

// Constant for Infinite, this is used to represent unreachable nodes
const int INF = numeric_limits<int>::max();

// ------------------------- GLOBAL DATA STRUCTURES -------------------------

// Stores the cost for the route, itis bidirectional
unordered_map<char, unordered_map<char, int>> costedge;

// This is updated during runtime, and is used to store minimum distance between edges
unordered_map<char, unordered_map<char, int>> distedge;

// Stores the neighbourhood edge for the router
unordered_map<char, unordered_map<char, char>> nextedge;

// Vector is used to store the values of costedge, during the process of gathering the inputs
vector<unordered_map<char, unordered_map<char, int>>> costvector;

// It is a set of Char representing the nodes of the router
set<char> router;

// ------------------------- ENUMERATION FOR FSM STATE -------------------------
// GET_EDGE     :  Initial state of the router which is used to collect the router nodes/edges
// START_GRAPH  :  This contains the Initial values of the topology
// UPDATE_GRAPH :  If the cost of the edge/nodes are updated or any edge is removed it is done in this state

enum STATE {
    GET_EDGE,
    START_GRAPH,
    UPDATE_GRAPH
};

// ------------------------- INITIALISATION FUNCTION -------------------------

// Initialisation of the distance and neighbourhood edge.
// The map is initialized before beginning the calculation of distance vector

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

// ------------------------- PRINT DISTANCE TABLE -------------------------
// Assignment requires 2 tables to be printed one of them is the Distance table
// Here the distance map and time step are provided as an input.
// The function parses the map and provides the distance table at given step time
// It follows the format required by the assignment

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
                     // Poisoned Reverse: check if there is a path via v and discard it
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

// ------------------------- PRINT ROUTING TABLE -------------------------
// The assignment requires printing of routing table
// routing table consists of neighbour edges if available and distance
// where the edge is unavailable, the distance is infinite

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

// Perform one full iteration of the Distance Vector algorithm with poisoned reverse
// In each iteration the distance is updated taking into account the destination, paths via v are discarded
// During each iteration initialisation of cost is done, if there is any change in cost
// or link is removed then the respective path is updated
// during the iteration previous cost is taken because the runDistanceVector is called after Initialisation
// hence the value is calculated for the next time stamp.
// At the end of the iteration the value of distedge which is the global table is updated.


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
                if (!costedge[a].count(v)) continue; // v must be a neighbour of a

                //  Poisoned Reverse: skip route info which goes back to the neighbour
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


// ------------------------- INPUT LOADER AND FSM -------------------------

// Read input, build initial graph and simulate routing table convergence
// Input is supplied in 3 steps cin till you get START is meant to collect the number of egdes/nodes
// between START - UPDATE are the initial costs.
// between UPDATE - END the cost and edges/nodes can be added or removed
// State machine is used iterate the inputs (state machines are more readable and it is easier to split the code)

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
