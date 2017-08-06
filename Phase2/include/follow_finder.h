#ifndef FOLLOW_FINDER_H
#define FOLLOW_FINDER_H
#include <bits/stdc++.h>
#include "element.h"

using namespace std;

class follow_finder
{
    public:
        follow_finder(map<element* , set<string> > , vector<element*>);
        virtual ~follow_finder();
        vector<string> follow(string );
        set<string> first(element* );
    protected:
    private:
        vector<element*> grammar; //vector have all nonterminal
        map<string, set<string> > follow_list;
        string lock_value;
        map<element* , set<string> > non_terminals_first;
        set<string> set_follow (string value);
};

#endif // FOLLOW_FINDER_H
