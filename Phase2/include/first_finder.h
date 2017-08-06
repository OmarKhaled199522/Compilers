#ifndef FIRST_FINDER_H
#define FIRST_FINDER_H
#include "bits/stdc++.h"
#include "element.h"
#include "follow_finder.h"

using namespace std;

class first_finder
{
    public:
        first_finder(vector<element*>);
        virtual ~first_finder();
        map<string , map<string , vector<string> > > predictive_table;
        void print(map<string , map<string , vector<string> > > predictive_table);
    protected:

    private:
        void calculate_first();
        vector<element*> non_terminals;
        map<element* , set<string> > non_teminals_first;
        unsigned max_lenght = 0;
        set<string> calculate_element_first(element*);
        set <string> get_sets_union(set<string> , set<string>);
        map<string , bool> finished;
        //the first map represents the non terminals and the inner map
        //represents the terminals and the vector is the productions
        follow_finder *_follow_finder ;
        void complete_table();

};

#endif // FIRST_FINDER_H
