#include <bits/stdc++.h>

#ifndef NFA_H
#define NFA_H

#define LAMBDA "\\L"
#define pb push_back
#define mp make_pair

using namespace std;


class NFA
{
public:
    NFA();
    virtual ~NFA();

    /*build graph using oring with all the chars in this vector and add it to definitions map */
    void set_difinition(string str, vector<char> arr) ;
    /*add graph to definitions map */
    void set_difinition(string str,vector<vector<pair<int , string> > >graph ) ;
    /*build graph using anding with all the chars in this vector and add the graph to nfa_graph and add to accept state map*/
    void set_key_word (vector<char>arr, int priority) ;
    /*add it to nfa_graph and to accept state map */
    void set_punctuations(char ch, int priority) ;
    /*add the new nfa_graph and add to accept state */
    void add_expresion(string  str, vector<vector<pair<int , string> > >graph, int priority) ;
    /*return graph from definitions map*/
    vector<vector<pair<int , string> > >get_definition(string str) ;
    /*oring two graph with each other and return bigger one */
    vector<vector<pair<int , string> > > OR ( vector<vector<pair<int , string> > > graph1 ,  vector<vector<pair<int , string> > >graph2) ;
    /*ANDING two graph with each other and return bigger one */
    vector<vector<pair<int , string> > > AND ( vector<vector<pair<int , string> > > graph1 ,  vector<vector<pair<int , string> > >graph2) ;
    /*closure takes  graph return bigger one (*)*/
    vector<vector<pair<int , string> > > closure ( vector<vector<pair<int , string> > > graph) ;
    /*positive_closure takes  graph return bigger one (+)*/
    vector<vector<pair<int , string> > > positive_closure ( vector<vector<pair<int , string> > >graph ) ;
    /*option operator  takes graph return bigger one (?)*/
    vector<vector<pair<int , string> > > option_operator ( vector<vector<pair<int , string> > >graph ) ;
    /*build graph O--string--O*/
    vector<vector<pair<int , string> > > constituent_subexpressions(string str) ;
    vector<vector<pair<int , string> > > multi_and (vector<char>arr);
    vector<vector<pair<int , string> > > multi_or (vector<char>arr);
    /*get nfa graph */
    vector<vector<pair<int , string> > > get_nfa_graph() ;
    /*return acceptance state*/
    map<int , pair<string, int> > get_accept_state_map() ;
    /*to print graph in debugging mode*/
    void print_graph( vector<vector<pair<int , string> > >graph) ;

protected:
private:
    /*--------------------------------------------------------------------*/
    /*map to store graph to each definition*/
    map<string , vector<vector<pair<int , string> > > >definition ;
    /*map to store accept state name to each index in nfa_graph */
    map<int , pair<string, int> > accept_state_map ;
    /*graph to store nodes and each state to other nodes */
    vector<vector<pair<int , string> > > nfa_graph ;
    /*--------------------------------------------------------------------*/
    /*add graph2 to graph1 with lambda in between */
    vector<vector<pair<int , string> > > add_graphs(vector<vector<pair<int , string> > >graph1 , vector<vector<pair<int , string> > >graph2) ;
};

#endif // NFA_H
