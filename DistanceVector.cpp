// I chose to do this assignment in C++ as I am more familiar with C++ compared to other languages
// I used unordered map for this assignment as it is cleaner and more readable
// unordered map is more manageable for graph size that are unknown and growing
// while vectors are good for dense graphs and where speed is required. 

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

// Stores the cost for the route, it is bidirectional
unordered_map<char, unordered_map<char, int>> costedge;

// This is updated during runtime, and is used to store the minimum distance between edges
unordered_map<char, unordered_map<char, int>> distedge;

// Stores the neighbourhood edge for the router
unordered_map<char, unordered_map<char, char>> nextedge;

// Vector is used to store the values of costedge, during the process of gathering the inputs
vector<unordered_map<char, unordered_map<char, int>>> costvector;

// It is a set of Char representing the nodes of the router
set<char> router;

// ------------------------- ENUMERATION FOR FSM STATE -------------------------
// GET_EDGE     :  Initial state of the router which is used to collect the router nodes/edges
// START_GRAPH  :  This contains the initial values of the topology
// UPDATE_GRAPH :  If the cost of the edge/nodes are updated or any edge is removed it is done in this state

// States of FSM
enum STATE {
    GET_EDGE,      
    START_GRAPH,   
    UPDATE_GRAPH   
};

// ------------------------- INITIALISATION FUNCTION -------------------------

// Initialisation of the distance and neighbourhood edge.
// The map is initialised before beginning the calculation of distance vector

// INITIALISATION:
//   for all destinations y in N:
//      D_x(y) = c(x,y) /* If y not a neighbour, c(x,y) = Infinity */
//   for each neighbour w
//      D_w(y) = Infinity for all destinations y in N
//   for each neighbour w
//      send distance vector D_x = [D_x(y): y in N] to w



void Init_edges() {
    for (char u : router) {
        for (char v : router) {
            if (u == v) {
                // if source and destination is same (self) then there is no next edge and distance is 0
                distedge[u][v] = 0;
                nextedge[u][v] = '-';
            } else if (costedge[u].count(v)) {
                // if a connection exist then update the distance and neighbour edge
                distedge[u][v] = costedge[u][v];
                nextedge[u][v] = v;
            } else {
                // if there is no connection then it is initialised to Infinite and neighbour edge is not present
                distedge[u][v] = INF;
                nextedge[u][v] = '-';
            }
        }
    }
}

// ------------------------- PRINT DISTANCE TABLE -------------------------
// The task requires 2 tables to be printed one of them is the Distance table
// Here the distance map and time step are provided as an input.
// The function parses the map and provides the distance table at given step time
// It follows the format required in the assignment

void printDistanceTable(int t, const unordered_map<char, unordered_map<char, int>>& snapshotDist) {
    for (char s : router) {
        cout << "Distance Table of router " << s << " at t=" << t << ":\n";
        cout << "     ";
        // print header (destination routers)
        for (char v : router)
            if (v != s) cout << v << "    ";
        cout << "\n\n";

        // looping over the destination router node
        for (char d : router) {
            if (d == s) continue;
            cout << d << "    ";

            // neighbour router node
            for (char v : router) {
                if (v == s) continue;

                // inital values only shows direct links
                if (t == 0) {
                    if (v == d && costedge[s].count(d)) {
                        cout << setw(3) << costedge[s][d] << "  ";
                    } else {
                        cout << "INF  ";
                    }
                } else {
                    // after initial state take cost of neighbour edge
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
// The assignment requires printing of the routing table
// routing table consists of neighbour edges if available and distance
// where the edge is unavailable, the distance is infinite

// print the final routing table of each router
void printRoutingTable() {
    for (char s : router) {
        cout << "Routing Table of router " << s << ":\n";
        for (char d : router) {
            if (d == s) continue;

            // if the distance is infinite then print infinite
            if (distedge[s][d] == INF) {
                cout << d << ",INF,INF\n";
            } else {
                // print: Destination, NextHop, Distance
                cout << d << "," << nextedge[s][d] << "," << distedge[s][d] << "\n";
            }
        }
        cout << "\n";
    }
}

// ------------------------- DISTANCE VECTOR UPDATE STEP -------------------------

// LOOP
//   wait (until I see a link cost change to some neighbour w or until 
//      I receive a distance vector from some neighbour w)
//   for each y in N:
//      D_x(y) = min_v{c(x,v) + D_v(y)}
//
//   if D_x(y) changed for any destination y
//      send distance vector D_x = [D_x(y): y in N] to all neighbours.
//
// FOREVER

// Perform one full iteration of the Distance Vector algorithm
// In each iteration the distance is updated taking into account the cost via edge
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

            // initialise with direct cost if it exists
            int minDist = (costedge[a].count(b) ? costedge[a][b] : INF);
            char bestNextHop = (costedge[a].count(b) ? b : '-');

            // try to find a shorter path through neighbours
            for (char v : router) {
                if (!costedge[a].count(v)) continue; // v must be neighbour
                if (!prevDist.count(v) || !prevDist[v].count(b) || prevDist[v][b] == INF) continue;

                int alt = costedge[a][v] + prevDist[v][b];
                if (alt < minDist) {
                    minDist = alt;
                    bestNextHop = v;
                }
            }

            // update new distance and next hop
            newDist[a][b] = minDist;
            newNext[a][b] = (minDist == INF ? '-' : bestNextHop);
        }
    }

    // print distance table before updating distedge
    printDistanceTable(t, prevDist);

    // update global tables
    distedge = newDist;
    nextedge = newNext;
}

// ------------------------- INPUT LOADER AND FSM -------------------------

// Read input, build initial graph and simulate routing table convergence
// Input is supplied in 3 steps cin till you get START is meant to collect the number of egdes/nodes
// between START - UPDATE are the initial costs.
// between UPDATE - END the cost and edges/nodes can be added or removed.
// State machine is used iterate the inputs (state machines are more readable and are easier to split the code)

void loadInput() {
    string rline;
    char r, a, b;
    int c;
    int t = 0; // Time step
    STATE state = GET_EDGE;
    bool updated = false;

    while (getline(cin, rline)) {
        // check control lines (START/UPDATE/END)
        if (rline.rfind("START", 0) == 0) {
            state = START_GRAPH;
            continue;
        } else if (rline.rfind("UPDATE", 0) == 0) {
            state = UPDATE_GRAPH;
            costvector.push_back(costedge); // store initial distance
            continue;
        } else if (rline.rfind("END", 0) == 0) {
            if (updated)
                costvector.push_back(costedge); // final update to the costs and nodes
            break;
        }

        istringstream splitline(rline);

        // get the edges/nodes in a router
        if (state == GET_EDGE) {
            if (splitline >> r) {
                router.insert(r);
            }
        }
        // parse links in START_GRAPH or UPDATE_GRAPH
        else if (state == START_GRAPH || state == UPDATE_GRAPH) {
            if (splitline >> a >> b >> c) {
                router.insert(a);
                router.insert(b);

                // remove link if cost = -1
                if (c == -1) {
                    costedge[a].erase(b);
                    costedge[b].erase(a);
                } else {
                    // update/add bidirectional cost
                    costedge[a][b] = c;
                    costedge[b][a] = c;
                }
            }
            if (state == UPDATE_GRAPH)
                updated = true;
        }
    }

    // simulate each snapshot of the graph (start + updates) 
    for (std::size_t k = 0; k < costvector.size(); k++) {
        costedge = costvector[k];

        if (t == 0) {
            // first initialisation
            distedge.clear();
            nextedge.clear();
            Init_edges();
        }

        // run distance vector for as many rounds as routers (convergence)
        for (std::size_t j = 0; j < router.size(); j++) {
            if (t == 0)
                printDistanceTable(t, distedge);
            else
                runDistanceVector(t);
            t++;
        }

        // show final routing table
        printRoutingTable();
    }
}

// ------------------------- MAIN FUNCTION -------------------------

int main() {
    loadInput(); // Start the simulation
    return 0;
}
