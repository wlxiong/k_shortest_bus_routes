#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
using namespace std;

const short MAX = 128, MAXP = 9999, MAXL = 999, M = 1 << 14;

class LINE          //bus line class
{
public:
    short ticket, type, nUp, nDown;
    short up[MAX], down[MAX];

    LINE() {
        nUp = 0;
        nDown = 0;
    }
};

class STOP          //bus stop class
{
public:
    short n;
    short lines[MAX], iUp[MAX], iDown[MAX];

    STOP() {        //initialize the stop object
        int i;
        n = 0;
        for (i = 0; i < MAX; i++) {
            iUp[i] = MAX;
            iDown[i] = MAX;
        }
    }
};

class NODE          //node class in the searching space
{
public:
    short n;
    bool ex;
    short cost[MAX], pre[MAX], pos[MAX], pline[MAX], it[MAX];

    void init() {
        n = 0;
        ex = false;
    }
};

ifstream fin;    //bus network data file
LINE Lines[MAXL];
STOP Stops[MAXP];
NODE Nodes[MAXP];
short adj_time = 3,     //traveling time between adjacent stops
      change_time = 5;  //transferring time
//  ticket_time=10; //value-time ratio
short adj[MAXP][MAXP], reach[MAXP], iter[MAXP], maxp, maxl;

void init_adj()
{
    // initialize the adjacent matrix
    int i, j, k, x, y;

    for (i = 0; i <= maxp; i++) {
        for (j = 0; j <= maxp; j++)
            adj[i][j] = M;
        adj[i][i] = 0;
    }

    for (k = 1; k <= maxl; k++) {
        for (i = 0; i < Lines[k].nUp; i++) {
            x = Lines[k].up[i];
            for (j = i + 1; j < Lines[k].nUp; j++) {
                y = Lines[k].up[j];
                adj[y][x] = 1;
            }
        }
        for (i = 0; i < Lines[k].nDown; i++) {
            x = Lines[k].down[i];
            for (j = i + 1; j < Lines[k].nDown; j++) {
                y = Lines[k].down[j];
                adj[y][x] = 1;
            }
        }
    }
}

void dijkstra(short n, short s, short map[MAXP][MAXP], short cost[MAXP])
{
    // compute the minimum number of transfers between s and any other node
    short i;
    typedef pair<short, short> node;
    set<node> Q;
    set<node>::iterator p;

    init_adj();
    for (i = 1; i <= n; i++)
        cost[i] = M;
    cost[s] = 0;

    Q.insert(node(0, s));
    while (Q.empty() == false) {
        node top = *Q.begin();
        Q.erase(Q.begin());
        int index = top.second;

        for (i = 1; i <= n; i++) {
            int weight = map[index][i];
            if (cost[i] > cost[index] + weight) {
                if (cost[i] != M)
                    Q.erase(Q.find(node(cost[i], i)));
                cost[i] = cost[index] + weight;
                Q.insert(node(cost[i], i));
            }
        }
    }

}

vector<string> split(const string &s, char delim){
    stringstream ss(s);
    vector<string> items;
    string item;
    while(getline(ss, item, delim))
        items.push_back(item);
    return items;
}

void trim(string &s){
    string::iterator left, right;
    for(left = s.begin(); left != s.end() && isspace(*left); left++)
        ;
    s.erase(s.begin(), left);
    for(right = s.end() - 1; right != s.begin() && isspace(*right); right--)
        ;
    s.erase(right+1, s.end());
}

bool readline(ifstream &input, string &buf){
    getline(input, buf);
    trim(buf);
    return !input.eof();
}

bool parsestop(string &buf, vector<int> &stops){
    vector<string> tokens = split(buf, '-');
    vector<string>::iterator it;
    string s;
    for(it = tokens.begin(); it != tokens.end(); it++){
        trim(*it);
        if (*it->begin() == 'S')
            s = string(it->begin()+1, it->end());
        else if (isdigit(*it->begin()))
            s = string(*it);
        else
            return false;
        stops.push_back(atoi(s.c_str()));
    }
    return true;
}

template<class T>
void dispvector(vector<T> &v){
    typename vector<T>::iterator it;
    if (v.empty())
        return;
    it = v.begin();
    cout << *it++;
    for( ; it != v.end(); it++)
        cout << ", " << *it;
    cout << endl;
}

bool load()
{
    // load the data in file busline.txt
    short line, stop, i, j;
    string buf;

    maxp = 0;   // the max stop nubmer
    maxl = 0;   // the max line number
    // read the first line
    while(readline(fin, buf)){
    if (buf.find("END") != string::npos)
        return true;
    if (buf[0] != 'L')
        return false;
    string s(buf.begin()+1, buf.end());
    line = atoi(s.c_str());
    // set the type to the default value
    Lines[line].type = 1;
    if (line > maxl)
        maxl = line;
    // read the second line
    readline(fin, buf);
    // set the ticket type
    Lines[line].ticket = atoi(buf.c_str());
    // read the third line: list of stops
    readline(fin, buf);
    // parse the list of stops
    vector<int> vstops;
    parsestop(buf, vstops);
    dispvector(vstops);
    vector<int>::iterator it;
    for(it = vstops.begin(); it != vstops.end(); it++){
        stop = *it;
        // add stop to the up bound of the line
        Lines[line].up[ Lines[line].nUp ] = stop;
        // add line to the stop's line list
        Stops[stop].lines[ Stops[stop].n ] = line;
        // save the position of stop along the line
        Stops[stop].iUp[ Stops[stop].n ] = Lines[line].nUp;
        // increase counts
        Lines[line].nUp++;
        Stops[stop].n++;
    }
    if(fin.peek() == 'S'){
        // read the forth line
        readline(fin, buf);
        // parse the list of stops in the other direction
        vector<int> vstops;
        parsestop(buf, vstops);
        dispvector(vstops);
        vector<int>::iterator it;
        for(it = vstops.begin(); it != vstops.end(); it++){
            stop = *it;
            // add stop to the down bound of the line
            Lines[line].down[ Lines[line].nDown ] = stop;
            if (Stops[stop].n < 1) {
                // if there is no line passing stop, just add this first one
                Stops[stop].lines[ Stops[stop].n ] = line;
                Stops[stop].n++;
            } else {
                if (Stops[stop].lines[ Stops[stop].n - 1 ] == line)
                    // update the position of stop along the line
                    Stops[stop].iDown[ Stops[stop].n - 1 ] = Lines[line].nDown;
                else {
                    // save the position of stop along the line
                    Stops[stop].iDown[ Stops[stop].n ] = Lines[line].nDown;
                    Stops[stop].n++;
                }
            }
            Lines[line].nDown++;
        }
    }
    else{
        // check if this is a cyclic bus line
        if ( Lines[line].up[0] == Lines[line].up[ Lines[line].nUp - 1 ]) {
            // mark the line type as 0 : cyclic
            Lines[line].type = 0;
            // if the up bound stops of the line are 1 2 3 4 5 6
            // then the for loop augments the list of stops to
            // 1 2 3 4 5 6 1 2 3 4 5 6
            for (i = 0, j = Lines[line].nUp; i < Lines[line].nUp - 1; i++, j++)
                Lines[line].up[j] = Lines[line].up[i];
        } else {
            // if the bus line is not cyclic, then
            // the down bound list is a reverse list of the up bound
            for (i = Lines[line].nUp - 1, j = 0; i >= 0; i--, j++) {
                stop = Lines[line].up[i];
                Lines[line].down[j] = stop;
                Stops[stop].iDown[ Stops[stop].n - 1 ] = j;
            }
            Lines[line].nDown = Lines[line].nUp;
        }
    }
    }
    return true;
}

bool input(short &o, short &d, short &k, short &g)
{
    // input the O&D, maximum number of transfers and k
    cout << "\n>The origin and the destination, separated by a blank: ";
    if (!(cin >> o >> d)) {
        cout << endl;
        return false;
    } else {
        cout << ">The maximum number of transfers: ";
        cin >> g;
        cout << ">Search the first k shortest routes,then k= ";
        cin >> k;
        return true;
    }
}

void output(short O, short D)
{
    // output the first k shortest routes
    vector<short> out;
    vector<short>::reverse_iterator rp;
    vector< pair<short, short> > s;
    vector< pair<short, short> >::iterator p;
    short i, pre, pos, tmp;

    cout << endl;
    if (Nodes[D].n == 0)
        cout << "No available path!\n";
    else {
        for (i = 0; i < Nodes[D].n; i++)
            s.push_back(pair<short, short>(Nodes[D].cost[i], i));
        sort(s.begin(), s.end());

        for (p = s.begin(), i = 1; p != s.end(); p++, i++) {
            cout << setfill(' ');
            cout << "Path" << setw(2) << i << " (Time" << setw(3) << Nodes[D].cost[p->second] << ", Transfer" << setw(2) << Nodes[D].it[p->second] << ')';
            pre = D;
            pos = p->second;
            out.clear();
            while (pre != O) {
                out.push_back(pre);
                out.push_back(Nodes[pre].pline[pos]);
                tmp = pre;
                pre = Nodes[pre].pre[pos];
                pos = Nodes[tmp].pos[pos];
            }
            cout << setfill('0');
            cout << ": S" << setw(4) << O;
            for (rp = out.rbegin(); rp != out.rend(); rp += 2)
                cout << "-L" << setw(3) << *rp << "-S" << setw(4) << *(rp + 1);
            cout << endl;
        }
    }

}

void search()
{
    // search the first k shortest routes
    short O, D, K, G, s, i, ii, j, k;
    short iline, ipos, inode, icost, max, n;
    queue<short> Q;

    while (input(O, D, K, G)) {
        for (i = 0; i < maxp; i++)
            Nodes[i].init();
        dijkstra(maxp, D, adj, iter);

        Nodes[O].cost[ Nodes[O].n ] = -change_time;
        Nodes[O].pre[ Nodes[O].n ] = -1;
        Nodes[O].pline[ Nodes[O].n ] = -1;
        Nodes[O].it[ Nodes[O].n ] = -1;
        Nodes[O].n++;
        Q.push(O);
        while (Q.empty() == false) {
            s = Q.front();
            Q.pop();
            if (Nodes[s].ex || s == D)
                continue;
            else
                Nodes[s].ex = true;

            for (i = 0; i < Stops[s].n; i++) {
                iline = Stops[s].lines[i];
                for (j = 0; j < Nodes[s].n; j++) {
                    if (Nodes[s].it[j] + iter[s] <= G) {

                        if (iline != Nodes[s].pline[j]) {
                            if (Lines[iline].type == 1) {
                                ipos = Stops[s].iUp[i];
                                for (ii = ipos + 1; ii < Lines[iline].nUp; ii++) { //Upline
                                    inode = Lines[iline].up[ii];
                                    icost = Nodes[s].cost[j] + (ii - ipos) * adj_time + change_time;
                                    n = Nodes[inode].n;

                                    if (n < K) {
                                        Q.push(inode);
                                        Nodes[inode].it[n] = Nodes[s].it[j] + 1;
                                        Nodes[inode].pre[n] = s;
                                        Nodes[inode].pos[n] = j;
                                        Nodes[inode].pline[n] = iline;
                                        Nodes[inode].cost[n] = icost;
                                        Nodes[inode].n++;
                                    } else {
                                        max = (int)(max_element(Nodes[inode].cost, &Nodes[inode].cost[n]) - Nodes[inode].cost);
                                        if (Nodes[inode].cost[max] > icost) {
                                            Q.push(inode);
                                            Nodes[inode].it[max] = Nodes[s].it[j] + 1;
                                            Nodes[inode].pre[max] = s;
                                            Nodes[inode].pos[max] = j;
                                            Nodes[inode].pline[max] = iline;
                                            Nodes[inode].cost[max] = icost;
                                        }
                                    }
                                }
                                ipos = Stops[s].iDown[i];
                                for (ii = ipos + 1; ii < Lines[iline].nDown; ii++) { //Downline
                                    inode = Lines[iline].down[ii];
                                    icost = Nodes[s].cost[j] + (ii - ipos) * adj_time + change_time;
                                    n = Nodes[inode].n;

                                    if (n < K) {
                                        Q.push(inode);
                                        Nodes[inode].it[n] = Nodes[s].it[j] + 1;
                                        Nodes[inode].pre[n] = s;
                                        Nodes[inode].pos[n] = j;
                                        Nodes[inode].pline[n] = iline;
                                        Nodes[inode].cost[n] = icost;
                                        Nodes[inode].n++;
                                    } else {
                                        max = (int)(max_element(Nodes[inode].cost, &Nodes[inode].cost[n]) - Nodes[inode].cost);
                                        if (Nodes[inode].cost[max] > icost) {
                                            Q.push(inode);
                                            Nodes[inode].it[max] = Nodes[s].it[j] + 1;
                                            Nodes[inode].pre[max] = s;
                                            Nodes[inode].pos[max] = j;
                                            Nodes[inode].pline[max] = iline;
                                            Nodes[inode].cost[max] = icost;
                                        }
                                    }
                                }
                            } else {
                                ipos = Stops[s].iUp[i];
                                for (ii = ipos + 1, k = 0; k < Lines[iline].nUp; k++, ii++) { //Circle
                                    inode = Lines[iline].up[ii];
                                    icost = Nodes[s].cost[j] + (ii - ipos) * adj_time + change_time;
                                    n = Nodes[inode].n;

                                    if (n < K) {
                                        Q.push(inode);
                                        Nodes[inode].it[n] = Nodes[s].it[j] + 1;
                                        Nodes[inode].pre[n] = s;
                                        Nodes[inode].pos[n] = j;
                                        Nodes[inode].pline[n] = iline;
                                        Nodes[inode].cost[n] = icost;
                                        Nodes[inode].n++;
                                    } else {
                                        max = (int)(max_element(Nodes[inode].cost, &Nodes[inode].cost[n]) - Nodes[inode].cost);
                                        if (Nodes[inode].cost[max] > icost) {
                                            Q.push(inode);
                                            Nodes[inode].it[max] = Nodes[s].it[j] + 1;
                                            Nodes[inode].pre[max] = s;
                                            Nodes[inode].pos[max] = j;
                                            Nodes[inode].pline[max] = iline;
                                            Nodes[inode].cost[max] = icost;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        output(O, D);
    }

}

int main(int argc, char *argv[])
{
    cout << "A Query Program for Urban Public Transportation Network\n";
    cout << "Using k-shortest paths algorithm\n\n";
    cout << " Transferring time: ";
    cin >> change_time;
    cout << " Traveling time of adjacent stops: ";
    cin >> adj_time;
    if (argc < 2)
        fin.open("busline.txt");
    else
        fin.open(argv[1]);
    if (fin.fail()) {
        cout << "\nCan't open file!\n";
        return false;
    } else {
        cout << "\nIt will take a few seconds to load the network data...\n";
    }

    if(load())
        search();
    else
        cout << "\nError in loading data!\n";

    return 0;
}
