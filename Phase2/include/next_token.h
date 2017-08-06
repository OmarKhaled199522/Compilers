#ifndef NEXT_TOKEN_H
#define NEXT_TOKEN_H
#include "bits/stdc++.h"

using namespace std;
class next_token
{
    public:
        next_token( vector<string> );
        vector<string> tokens;
        string get_next_token();
        virtual ~next_token();

    protected:

    private:
        int cnt = 0;
};

#endif // NEXT_TOKEN_H
