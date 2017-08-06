#ifndef SCANNER_GAMMAR_H
#define SCANNER_GAMMAR_H
#include <bits/stdc++.h>
#include "element.h"

using namespace std;

class scanner_gammar
{
    public:
        scanner_gammar();
        virtual ~scanner_gammar();
        vector<string> read_file(string path);
        vector<element*> convert (vector<vector<string> > rhss, vector<string> nonTerminal);
        vector<string> get_grammar(vector<string> file, vector<vector<string> >* rhss);
        void print_grammar(vector<element*> grammar);

    protected:

    private:
        string get_nonTerminal(string *line);
        vector<string> split (string str , char spliter);
        int find_element (vector<element*> grammar, element elem);


};

#endif // SCANNER_GAMMAR_H
