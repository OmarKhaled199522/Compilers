#include "follow_finder.h"

follow_finder::follow_finder(map<element*, set<string> >  non_terminals_first, vector<element*> grammar)
{
    this->non_terminals_first = non_terminals_first;
    this->grammar = grammar;
}

follow_finder::~follow_finder()
{
    //dtor
}

set<string> follow_finder::first (element* non_term)
{
    map<element*, set<string> >::iterator it = non_terminals_first.find(non_term);
    return it->second;
}

vector<string> follow_finder::follow (string value)
{
    set<string> s = set_follow (value);
    vector<string> v( s.begin(), s.end());
    return v;
}

set<string> follow_finder::set_follow (string value)
{
    //check map
    map<string, set<string> >::iterator it = follow_list.find(value);
    if (it != follow_list.end())
    {
        return it->second;
    }
    lock_value = lock_value == "" ? value : lock_value;
    set<string> follows;
    for(unsigned i=0; i < grammar.size() ; i++)
    {
        vector<vector<element*> > rhs = grammar[i]->right_side_elements;
        for(unsigned j= 0; j < rhs.size(); j++)
        {
            for(unsigned k=0; k < rhs[j].size(); k++)
            {
                if(rhs[j][k]->value == value)
                {
                    if(k == rhs[j].size()-1 && grammar[i]->value != value
                            && grammar[i]->value != lock_value) // deadlock
                    {
                        set<string> s = set_follow (grammar[i]->value);
                        follows.insert(s.begin(), s.end());
                    }
                    else if (k == rhs[j].size()-1)
                    {
                        continue;
                    }
                    else if (rhs[j][k+1]->is_terminal)
                    {
                        follows.insert(rhs[j][k+1]->value);
                    }
                    else
                    {
                        set<string> s = first (rhs[j][k+1]);
                        auto a = s.erase("\\L");
                        follows.insert(s.begin(), s.end());
                        if (a !=  0)
                        {
                            set<string> s = set_follow (grammar[i]->value);
                            follows.insert(s.begin(), s.end());
                        }
                    }
                }
            }
        }
        rhs.clear();
    }
    lock_value = lock_value == value ? "" : lock_value;
    follows.insert("$");
    follow_list[value] = follows;
    return follows;
}

