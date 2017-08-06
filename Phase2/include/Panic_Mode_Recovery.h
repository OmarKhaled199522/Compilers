#ifndef PANIC_MODE_RECOVERY_H
#define PANIC_MODE_RECOVERY_H

#include "next_token.h"
#include "bits/stdc++.h"

class Panic_Mode_Recovery
{
    public:
        Panic_Mode_Recovery(next_token *next , string start_state , map<string , map<string , vector<string> > > predictive_table);
        void Panic_Mode_Checker() ;
        virtual ~Panic_Mode_Recovery();

    protected:
    private:
        void print_stack() ;
        next_token *next ;
        string start_state ;
        map<string , map<string , vector<string> > > predictive_table ;
        vector<string> stack_string ;
        ofstream output;
        const char separator = ' ';
        const int width = 100;
};

#endif // PANIC_MODE_RECOVERY_H
