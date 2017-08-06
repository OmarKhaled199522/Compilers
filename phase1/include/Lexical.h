#ifndef Scanner_H
#define Scanner_H
#include "NFA.h"
#include <bits/stdc++.h>

using namespace std;

typedef vector<vector<pair<int , string> > > graph_t;

class Scanner
{
    public:
        Scanner();
        virtual ~Scanner();
        vector<string> read_rules(string);
        void parse (vector<string>);
        void print_nfa();//for test
          /*get nfa graph */
        graph_t get_nfa_graph() ;
        /*return acceptance state*/
        map<int , pair<string, int> > get_accept_state_map() ;

    protected:

    private:
        NFA nfa;
        int priority;
        int line_number;
        ofstream scan_log;
        // parse
        void parse_definition (string , string);
        void parse_expression (string , string);
        void parse_punctuation (string);
        void parse_keyword (string);
        //expression
        graph_t build_expression_graph (string str);
        graph_t build_word (string);
        graph_t multi_or (graph_t, graph_t);
        //get range
        vector<char> range_chars (char, char);
        //check operations
        bool is_operation (char);
        bool is_double_operation (char);
        bool is_single_operation (char);
        //postfix
        int operation_priority(char);
        void set_operand (string* operand,map<string, graph_t >* graph_map, vector<string>* output);
        vector<string> convert_expression_to_postfix (string infix , map<string, graph_t >* graph_map);
        // helper functions
        int skip_space (string, int);
        string rebuildexp (string);
        string char_to_string (char c);
        void replace_all(string& str, const string& from, const string& to);
        vector<char> str_to_vector (string);
        vector<string> split (string, char);
        string check_infix (string infix);
        string anding (string str);

        // check error
        bool check_RHS (string str);
        void false_rules();
        //test
        void print_vector_of_string(vector<string> s);
};

#endif // Scanner_H
