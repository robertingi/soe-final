#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdlib>



using namespace std;

unsigned int T_SUPPORT = 3;
float T_CONFIDENCE = 65.0;

struct Edge
{
  float conf_a; //confidence value for first node in edge (support({A,B})/support({A}))
  float conf_b; //(support({A,B})/support({B}))
  unsigned int support; //support value
  Edge():support(0){} //init value
};

struct Node
{
  set<string> connections; //hold connections
  unsigned int support;   //support value
  Node():support(0){}
};

//dictionaries
map<string, set<string> > all_scopes; //bind functions to a scope
map<string, Node> all_nodes; //bind function to a node
map<pair<string, string>, Edge> all_edges; //bind function pairs to an edge

pair<string, string> pairFunc(const string& A, const string& B)
{
  return (A < B ? pair<string, string>(A, B) : pair<string, string>(B, A));
}

//parses the call graph
void parse()
{
  for (string call; getline(cin, call);)
  {
    //if not a function call
    if (call.size() < 33 || call[30] != '\'')
      continue;

    //get the scope functions
    set<string>* functions = &all_scopes[call.substr(31, call.find_first_of('\'', 31) - 31)];

    //read function calls
    while (getline(cin, call) && !call.empty())           //read lines from call graph
    {
      size_t callsindex = call.find("calls function '");  //find a function call
      if (callsindex == -1)                               //if not found
        continue;

      //get the name of the function
      string name = call.substr(callsindex + 16, call.find_first_of('\'', callsindex + 16) - (callsindex + 16));
      //insert into scope functions
      functions->insert(name);
    }

    //pair up and store connections
    vector<string> functionsVector(functions->begin(), functions->end());
    for (size_t i = 0; i < functionsVector.size(); i++)
    {
      string first = functionsVector[i];
      all_nodes[first].support++; //increase support value for function

      for (size_t j = i + 1; j < functionsVector.size(); j++)
      {
        string second = functionsVector[j];

        all_nodes[first].connections.insert(second); //connect functions
        all_nodes[second].connections.insert(first);
        all_edges[pairFunc(first, second)].support++; //create edge
      }
    }
  }

}

void confidences()
{
  map<pair<string, string>, Edge>::iterator iter; //iterate through edges
  for (iter = all_edges.begin(); iter != all_edges.end(); iter++)
  {
    pair<string, string> pair = iter->first;
    Edge* edge = &iter->second;
    edge->conf_a = ( edge->support / (float) all_nodes[pair.first].support ) * 100;
    edge->conf_b = ( edge->support / (float) all_nodes[pair.second].support ) * 100;
  }
}

void bugs()
{
  map<string, set<string> >::iterator iter;
  for (iter = all_scopes.begin(); iter != all_scopes.end(); iter++) //iterate through all scopes
  {
    string scope = iter->first; //scope
    const set<string>* functions = &iter->second; //functions in the scope

    //loop through functions in the scope
    for (set<string>::iterator scope_iter = functions->begin(); scope_iter != functions->end(); scope_iter++)
    {
      string function = *scope_iter;
      const set<string>* connections = &all_nodes[function].connections; //get connections of the function

      //loop through function's connections
      for (set<string>::iterator conn_iter = connections->begin(); conn_iter != connections->end(); conn_iter++)
      {
        string connection_function = *conn_iter;
        pair<string, string> pair = pairFunc(function, connection_function); //pair functions
        Edge* edge = &all_edges[pair]; //get edge for the pair
        float conf;
        if (function < connection_function)
        {
          conf = edge->conf_a;
        }
        else
        {
          conf = edge->conf_b;
        }
        if (edge->support >= T_SUPPORT && conf >= T_CONFIDENCE && functions->find(connection_function) == functions->end()) //check for a bug
        {
          cout << "bug: " << function << " in " << scope << ", pair: (" << pair.first << ", " << pair.second << "), support: " << edge->support << ", confidence: " << fixed << setprecision(2) << conf << "%" << endl;
        }
      }
    }
  }
}

int main(int argc, char** argv) {
  switch (argc)
  {
    case 3: T_CONFIDENCE = atof(argv[2]);
    case 2: T_SUPPORT = atoi(argv[1]);
  }

  parse();
  confidences();
  bugs();

  return 0;
}
