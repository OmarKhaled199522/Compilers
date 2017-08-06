#include "NFA.h"

NFA::NFA()
{
    //ctor
    vector<pair<int , string> > v;
    nfa_graph.push_back(v) ;

}

NFA::~NFA()
{
    //dtor
}

/*add the new nfa_graph and add to accept state   */
void NFA::add_expresion(string  str, vector<vector<pair<int , string> > >graph, int priority)
{
    nfa_graph = add_graphs(nfa_graph , graph) ;
    if (str.size() != 0)
        accept_state_map[nfa_graph.size()-1] = make_pair(str, priority) ;
}

/*add graph2 to graph1 with lambda in between */
vector<vector<pair<int , string> > > NFA::add_graphs(vector<vector<pair<int , string> > >graph1 , vector<vector<pair<int , string> > >graph2)
{
    int size_different  = graph1.size() ;

    graph1.pb( vector<pair<int , string> > ()) ;

    graph1[0].push_back(make_pair(size_different + 0 , LAMBDA)) ;
    for (int i = 0 ; i < graph2.size(); i++)
    {
        for (int j = 0 ; j < graph2[i].size() ; j++)
        {
            pair<int , string> pi = graph2[i][j] ;
            graph1[i+size_different].push_back(make_pair(pi.first+size_different , pi.second)) ;
        }
        if (i != graph2.size()-1)
            graph1.pb( vector<pair<int , string> > ()) ;
    }
    return graph1 ;
}
/*return graph from definitions map*/
vector<vector<pair<int , string>>> NFA::get_definition(string str) {
    if (definition.find(str) != definition.end()) return definition[str]  ;
    return {} ;
}
/*oring two graph with each other and return bigger one */
vector<vector<pair<int , string> > > NFA::OR ( vector<vector<pair<int , string> > > graph1 ,  vector<vector<pair<int , string> > >graph2)
{
   vector<vector<pair<int , string> > > oring_graph ;
    oring_graph.pb(vector<pair<int , string> >()) ;

    oring_graph = add_graphs(oring_graph , graph1) ;
    oring_graph = add_graphs(oring_graph , graph2) ;
    oring_graph.pb(vector<pair<int , string> >()) ;
    oring_graph[graph1.size()].push_back(make_pair(oring_graph.size()-1 , LAMBDA)) ;
    oring_graph[graph1.size()+graph2.size()].push_back(make_pair(oring_graph.size()-1 , LAMBDA)) ;
    return oring_graph ;
}
/*ANDING two graph with each other and return bigger one */
vector<vector<pair<int , string> > > NFA::AND ( vector<vector<pair<int , string> > > graph1 ,  vector<vector<pair<int , string> > >graph2)
{
    int size_different  = graph1.size() ;

    for (int i = 0 ; i < graph2.size(); i++)
    {
        for (int j = 0 ; j < graph2[i].size() ; j++)
        {
            pair<int , string> pi = graph2[i][j] ;
            graph1[i+size_different-1].push_back(make_pair(pi.first+size_different-1 , pi.second)) ;
        }
        if (i != graph2.size()-1)
            graph1.pb(vector<pair<int , string> > () ) ;
    }
    return graph1 ;
}
/*closure takes  graph return bigger one (*)*/
vector<vector<pair<int , string> > > NFA::closure ( vector<vector<pair<int , string> > > graph)
{
    vector<vector<pair<int , string> > > closure_graph ;
    closure_graph.pb(vector<pair<int , string> >()) ;

    closure_graph = add_graphs(closure_graph, graph) ;

    closure_graph.pb(vector<pair<int , string> >()) ;

    closure_graph[closure_graph.size()-2].push_back(make_pair(closure_graph.size()-1 , LAMBDA)) ;
    closure_graph[0].push_back(make_pair(closure_graph.size()-1 , LAMBDA)) ;
    closure_graph[closure_graph.size()-2].push_back(make_pair(1 , LAMBDA)) ;

    return closure_graph ;
}
/*positive_closure takes  graph return bigger one (+)*/
vector<vector<pair<int , string> > > NFA::positive_closure ( vector<vector<pair<int , string> > >graph )
{
    return AND(graph , closure(graph)) ;
}
/*option operator  takes graph return bigger one (?)*/
vector<vector<pair<int , string> > > NFA::option_operator ( vector<vector<pair<int , string> > >graph )
{
    graph[0].push_back(make_pair(graph.size()-1 , LAMBDA)) ;
    return graph;
}
/*build graph O--string--O*/
vector<vector<pair<int , string> > > NFA::constituent_subexpressions(string str)
{
    vector<vector<pair<int , string> > > g;
    g.push_back(vector<pair<int , string> > ());
    g[0].push_back(make_pair(1 , str)) ;
    g.push_back(vector<pair<int , string> > ());
    return g;
}
/*get nfa graph */
vector<vector<pair<int , string> > > NFA::get_nfa_graph()
{
    return nfa_graph ;
}
/*return acceptance state*/
map<int , pair<string, int> > NFA::get_accept_state_map()
{
    return accept_state_map ;
}

/*add it to nfa_graph and to accept state map */
void NFA::set_punctuations(char ch, int priority)
{
    vector<char> temp_vec ;
    temp_vec.pb(ch);
    set_key_word(temp_vec, priority) ;
    temp_vec.clear() ;
}
/*build graph using anding with all the chars in this vector and add the graph to nfa_graph and add to accept state map*/
void NFA::set_key_word (vector<char>arr, int priority)
{
    if (arr.size() == 0) return ;

    nfa_graph.pb(vector<pair<int , string> >()) ;
    string str = "1" ;
    str[0] = arr[0] ;
    nfa_graph[0].pb(mp(nfa_graph.size()-1 , LAMBDA)) ;
    nfa_graph.pb(vector<pair<int , string> >()) ;
    nfa_graph[nfa_graph.size()-2].pb(mp(nfa_graph.size()-1 , str)) ;
    for (int i = 1 ; i < arr.size() ; i++)
    {
        nfa_graph.pb(vector<pair<int , string> >()) ;
        str[0] = arr[i] ;
        nfa_graph[nfa_graph.size()-2].pb(mp(nfa_graph.size()-1 , str)) ;
    }

    string state(arr.begin(),arr.end());
    accept_state_map[nfa_graph.size()-1] = make_pair(state, priority) ;

    str.clear() ;
    state.clear() ;
}

/*build graph using oring with all the chars in this vector and add it to definitions map */
void NFA::set_difinition(string str, vector<char> arr)
{
    vector<vector<pair<int , string> > > graph(arr.size() * 2 + 2);
    for (int i = 0 ; i < arr.size() ; i++ )
    {
        graph[0].push_back(make_pair(i+1 , LAMBDA)) ;
        string r = "1"  ;
        r[0] =  arr[i] ;
        graph[i+1].push_back(make_pair(i+1+arr.size() , r)) ;
        graph[i+1+arr.size()].push_back(make_pair(graph.size()-1 , LAMBDA)) ;
    }
    set_difinition(str , graph) ;
}
/*add graph to definitions map */
void NFA::set_difinition(string str,vector<vector<pair<int , string> > >graph )
{
    definition[str] = graph ;
}

void NFA::print_graph(vector<vector<pair<int , string> > >graph )
{

    for (int i = 0 ; i < graph.size() ; i++)
    {
        int boolean = 0 ;
        for (int j = 0 ; j < graph[i].size() ; j++)
        {
            boolean = 1 ;
            cout << "from " << i <<  " -> " <<  graph[i][j].first << "  with value = " << graph[i][j].second << endl;
        }
        if (!boolean) cout << "from " << i << " no edges" << endl;
    }
}
 /*anding between vector character*/
vector<vector<pair<int , string> > > NFA::multi_and (vector<char>arr)
{
    if (arr.size() == 0) return {};
    vector<vector<pair<int , string> > > graph  ;
    string str = "1";
    for (int i = 0 ; i < arr.size() ; i++)
    {
        graph.pb(vector<pair<int , string> >()) ;
        str[0] = arr[i] ;
        graph[i].pb(mp(i+1 , str)) ;
    }
    graph.pb(vector<pair<int , string> >()) ;
    return graph ;
}
 /*oring between vector character*/
vector<vector<pair<int , string> > > NFA::multi_or (vector<char>arr)
{
    vector<vector<pair<int , string> > > graph(arr.size() * 2 + 2);
    for (int i = 0 ; i < arr.size() ; i++ )
    {
        graph[0].push_back(make_pair(i+1 , LAMBDA)) ;
        string r = "1"  ;
        r[0] =  arr[i] ;
        graph[i+1].push_back(make_pair(i+1+arr.size() , r)) ;
        graph[i+1+arr.size()].push_back(make_pair(graph.size()-1 , LAMBDA)) ;
    }
    return graph ;
}
