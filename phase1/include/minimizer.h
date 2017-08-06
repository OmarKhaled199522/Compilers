#ifndef MINIMIZER_H
#define MINIMIZER_H
#include "bits/stdc++.h"
#include "patterns_recognizer.h"
using namespace std;

class minimizer
{
    public:
        minimizer(vector< vector< pair<int ,string> > > , vector< pair<int , string> > );
        vector< map<string, int> > final_automata;
        void test_print();
        void start_minimizer();
    protected:
    private:
        vector< vector< pair<int ,string> > > transition_table; //vector of  <state no , pattern>
        vector< pair<int , string> > acceptance_states ;
        map<int,string> final_acceptance_states ;//<state no, pattern>
        vector< vector<int> > groups;
        map <int , int> state_group;//maps a state to its group
        map <string , int> pattern_group;
        set<string> inputs;
        vector< vector<int> > separate_states(vector< vector<int> > , vector<int> );
        vector<int> get_destination(int );
        vector <map<string, int> >table_maps;//given a weight it knows the destination
        vector<bool> minimization_state;
        int final_start_state;
        void initialize_groups();
        void do_partitioning();
        void extract_inputs();
        bool is_finally_minimized();
        void update_states_groups();
        void build_final_automata();
        void extract_final_acceptance_states();
        void get_start_state();
};

#endif // MINIMIZER_H
