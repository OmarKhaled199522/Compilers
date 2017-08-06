#ifndef PATTERNS_RECOGNIZER_H
#define PATTERNS_RECOGNIZER_H
#include "bits/stdc++.h"
using namespace std;
class patterns_recognizer
{
    public:
        patterns_recognizer(map<int,string>,vector< map<string, int> >,set<string>,int);
        void start_recognizing();
    protected:
    private:
        void read_file();
        void parse_file();
        void parse_line(string);
        void parse_connected_string(string);
        set<string> inputs;
        vector<string> input_file_lines;
        vector<string> output_file_lines;
        vector<string> output_lexemes ;
        map<int,string> final_acceptance_states ;
        vector< map<string, int> > final_automata;
        map <int , int> state_group;
        int final_start_state;
        void print_symbol_table();

};

#endif // PATTERNS_RECOGNIZER_H
