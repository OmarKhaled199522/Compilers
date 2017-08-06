#include <bits/stdc++.h>
#include "Scanner.h"
#include "DfaConverter.h"
#include "minimizer.h"

using namespace std;

int main()
{
    cout << "start NFA" << endl;
    Scanner scan;
    vector<string> rules = scan.read_rules("rules.txt");
    scan.parse(rules);
    scan.print_nfa();
    cout << "start DFA" << endl;
    DfaConverter dfa;
    dfa.final_states_map = scan.get_accept_state_map();
    dfa.set_dfa(scan.get_nfa_graph());
    cout << "start Minimize" << endl;
    minimizer *m = new minimizer(dfa.states_after_dfa ,dfa.final_destinations_states);
    m->start_minimizer();
    cout <<"end Lexical Analyzer Generator \n";
    return 0 ;
}

