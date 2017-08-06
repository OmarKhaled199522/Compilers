#include "next_token.h"

next_token::next_token(vector<string> tokens)
{
    this->tokens = tokens;
}

next_token::~next_token()
{
    //dtor
}
string next_token::get_next_token()
{
    if (cnt >= tokens.size())
        return "$";
    while(tokens[cnt] == "" || tokens[cnt] == " ")
        cnt++;
    return tokens[cnt++];
}
