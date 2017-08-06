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
        /*read grammar from rules file*/
        vector<string> read_rules(string);
        /*parse grammar file and build nfa graph argument is of read_rules*/
        void parse (vector<string>);
        /*print graph for test*/
        void print_nfa();//for test
        /*return nfa graph */
        graph_t get_nfa_graph() ;
        /*return acceptance state*/
        map<int , pair<string, int> > get_accept_state_map() ;

    protected:

    private:
        NFA nfa;
        int priority;
        int line_number;
        ofstream scan_log;
        /** parse **/
        /*build nfa graph of definition*/
        void parse_definition (string , string);
        /*build nfa graph of expression*/
        void parse_expression (string , string);
        /*build nfa graph of punctuation*/
        void parse_punctuation (string);
        /*build nfa graph of keyword*/
        void parse_keyword (string);
        /** postfix **/
        /*return priority of each operator*/
        int operation_priority(char);
        /*check the word if it definition or not if not build it as multi anding*/
        graph_t build_word (string);
        /*set operand graph on map with its definition*/
        void set_operand (string* operand,map<string, graph_t >* graph_map, vector<string>* output);
        /*convert from infix expression to postfix and set graph operand map*/
        vector<string> convert_expression_to_postfix (string infix , map<string, graph_t >* graph_map);
        /** expression **/
        /*return nfa graph of expression which build from postfix format*/
        graph_t build_expression_graph (string str);
        /*return graph of (-) operator ex: A-Z */
        graph_t multi_or (graph_t, graph_t);
        //return vector of char from first to last
        vector<char> range_chars (char first, char last);
        /*check operations*/
        bool is_operation (char);
        /*return true if char = '|' or '-' */
        bool is_double_operation (char);
        /*return true if char = '*' or '+' or '?' */
        bool is_single_operation (char);

        // helper functions
        int skip_space (string, int);
        string rebuildexp (string);
        string char_to_string (char c);
        void replace_all(string& str, const string& from, const string& to);
        vector<char> str_to_vector (string);
        string check_infix (string infix);
        string anding (string str);

        // check error
        void false_rules();

        //test
        vector<string> split (string, char);
        void print_vector_of_string(vector<string> s);
};

#endif // Scanner_H
