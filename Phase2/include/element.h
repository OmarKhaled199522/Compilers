#ifndef ELEMENT_H
#define ELEMENT_H
#include "bits/stdc++.h"
using namespace std;
class element
{
    public:
        element();
        virtual ~element();
        string value;
        bool is_terminal;
        vector< vector<element*> > right_side_elements ;

    protected:


    private:
};

#endif // ELEMENT_H
