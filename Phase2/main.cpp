#include <iostream>
#include <bits/stdc++.h>
#include <bits/stdc++.h>
#include "element.h"
#include "scanner_gammar.h"
#include "first_finder.h"
#include "scanner.h"
#include "DfaConverter.h"
#include "minimizer.h"
#include "next_token.h"
#include "LeftAdjust.h"
#include "Panic_Mode_Recovery.h"

using namespace std;

int main()
{
    cout << "Start NFA" << endl;
    Scanner scan;
    vector<string> rules = scan.read_rules("rules.txt");
    scan.parse(rules);
    scan.print_nfa();
    cout << "Start DFA" << endl;
    DfaConverter dfa;
    dfa.final_states_map = scan.get_accept_state_map();
    dfa.set_dfa(scan.get_nfa_graph());
    cout << "Start Minimization" << endl;
    minimizer *m = new minimizer(dfa.states_after_dfa ,dfa.final_destinations_states);
    m->start_minimizer();
    cout <<"End Lexical Analyzer Generator \n";

    scanner_gammar scann;
    vector<string> file = scann.read_file("grammar.txt");
    vector<vector<string> > rhss;
    cout << "Start read" <<endl;
    vector<string> nonTerminal = scann.get_grammar(file, &rhss);
    LeftAdjust l;
    cout << "Start left recursion" <<endl;
    l.get_left_recursion(rhss, nonTerminal);
    cout << "Start left factoring" <<endl;
    l.get_left_factoring(rhss, nonTerminal);
    cout << "Start convert" <<endl;
    vector<element*> grammar = scann.convert (rhss, nonTerminal);
    //scann.print_grammar(grammar);
    cout << "Start first&follow" <<endl;
    first_finder *f = new first_finder(grammar);
    //it's to be moved to the stack class
    cout << "Start Recovery" <<endl;
    next_token *next = new next_token(m->final_tokens);
    string start_state = nonTerminal[0];
    f->print(f->predictive_table) ;
    Panic_Mode_Recovery *pa_checker = new Panic_Mode_Recovery(next ,start_state , f->predictive_table) ;
    pa_checker->Panic_Mode_Checker() ;
    cout << "End Parser Generator" <<endl;
    return 0 ;
}

