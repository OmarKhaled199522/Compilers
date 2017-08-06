#include "LeftAdjust.h"
#include <bits/stdc++.h>

using namespace std;

const string LeftAdjust::epislon = "\\L";

LeftAdjust::LeftAdjust()
{
    //ctor
}

LeftAdjust::~LeftAdjust()
{
    //dtor
}

string LeftAdjust::get_first_sym(string sym_option_string, int &first_sym_end){

    string first_sym_string = "";
    int i = 0;

    for(i; i < sym_option_string.size() && sym_option_string[i] != ' '; i++){
        first_sym_string += sym_option_string[i];
    }

    first_sym_end = i + 1;
    return first_sym_string;
}

void LeftAdjust::remove_immediate_recursion(vector <vector <string> > &input_grammer,
                                            vector <string> &non_terms, int &symbol_row){

    int i = symbol_row, first_sym_end, alphaS_num, betaS_num;
    int symbol_options = input_grammer[i].size();
    vector <string> alphaS;
    vector <string> betaS;

    for(int j = 0; j < symbol_options; j++){

        string first_sym = get_first_sym(input_grammer[i][j], first_sym_end);
        if(first_sym.compare(non_terms[i]) == 0){

            alphaS.push_back(input_grammer[i][j].substr(first_sym_end));

        } else betaS.push_back(input_grammer[i][j]);
    }

    alphaS_num = alphaS.size();
    if(alphaS_num != 0){        // need left recursion

        string new_symbol = non_terms[i] + "?";
        vector <string> new_transition, new_row;
        betaS_num = betaS.size();

        for(int j = 0; j < betaS_num; j++){
            new_transition.push_back(betaS[j] + " " + new_symbol);
        }

        input_grammer[i] = new_transition;

        for(int j = 0; j < alphaS_num; j++){
            new_row.push_back(alphaS[j] + " " + new_symbol);
        }

        new_row.push_back(epislon);
        added_symbols.push_back(new_row);
        non_terms.push_back(new_symbol);
    }
}

void LeftAdjust::replace_transitions(vector <vector <string> > &input_grammer,
                                     vector <string> &non_terms, int org_row, int rep_row){

    int i = org_row;
    int first_sym_end, size_org_row = input_grammer[i].size();
    vector <string> replaced_symbols;
    vector <string> new_row;

    for(int j = 0; j < size_org_row; j++){

        string first_sym = get_first_sym(input_grammer[i][j], first_sym_end);
        if(first_sym.compare(non_terms[rep_row]) == 0){

            string follow_syms = input_grammer[i][j].substr(first_sym_end);
            replaced_symbols = input_grammer[rep_row];

            for(int k = 0; k < input_grammer[rep_row].size(); k++){
                new_row.push_back(input_grammer[rep_row][k] + " " + follow_syms);
            }

        } else new_row.push_back(input_grammer[i][j]);
    }

    input_grammer[i] = new_row;
}

void LeftAdjust::adjust_left_recursion(vector <vector <string> > &input_grammer, vector <string> &non_terms){

    int non_term_num = input_grammer.size();

    for(int i = 0; i < non_term_num; i++){
        for(int j = 0; j < i; j++){

            replace_transitions(input_grammer, non_terms, i, j);
        }

        remove_immediate_recursion(input_grammer, non_terms, i);
    }

    for(int i = 0; i < added_symbols.size(); i++){
        input_grammer.push_back(added_symbols[i]);
    }
}

vector <string> LeftAdjust::get_string_tokens(string whole_string){

    istringstream is_stream(whole_string);
    vector <string> tokens{istream_iterator<string>{is_stream},
                      istream_iterator<string>{}};

    return tokens;
}

vector <vector <int>> LeftAdjust::get_similar_start(vector <vector <string> > &input_grammer,
                                    vector <string> &non_terms, int cur_row){

    int i = cur_row, options_num = input_grammer[cur_row].size();
    vector <string> first_string_tokens, second_string_tokens;
    vector <vector <int> > similar_start;
    int st_toks_siz, nd_toks_siz;

    for(int j = 0; j < options_num; j++){

        vector <int> sub_similar_start;

        for(int k = 0; k < options_num; k++){

            if(k <= j){
                sub_similar_start.push_back(0); // dummy value
                continue;
            }
            int min_similar = 0;
            first_string_tokens = get_string_tokens(input_grammer[i][j]);
            second_string_tokens = get_string_tokens(input_grammer[i][k]);
            st_toks_siz = first_string_tokens.size();
            nd_toks_siz = second_string_tokens.size();

            for(int h = 0; h < st_toks_siz && h < nd_toks_siz; h++){

                //cout << first_string_tokens[h] << " " << second_string_tokens[h] << "\n";
                if(first_string_tokens[h].compare(second_string_tokens[h]) == 0)
                    min_similar++;
                else break;
            }

            sub_similar_start.push_back(min_similar);
        }
        similar_start.push_back(sub_similar_start);
    }
    return similar_start;
}

string LeftAdjust::group_elements(vector <vector <string> > &input_grammer, vector <string> &non_terms,
                                const vector <int> &optimized_group,  int mini_similar, int cur_row){

    int i = cur_row, first_elemet = optimized_group[0];
    vector <string> tokens = get_string_tokens(input_grammer[cur_row][first_elemet]);
    string optimized_string = "", new_symbol = "", distinct_string;
    vector <string> new_row;

    for(int j = 0; j < mini_similar; j++){

        optimized_string += tokens[j];
        optimized_string += " ";
    }

    new_symbol += non_terms[cur_row];
    for(int j = 0; j < added_symbols.size() + 1; j++) new_symbol += "?";
    non_terms.push_back(new_symbol);
    optimized_string += new_symbol;

    for(int j = 0; j < optimized_group.size(); j++){

        tokens = get_string_tokens(input_grammer[cur_row][optimized_group[j]]);
        distinct_string = "";

        for(int k = mini_similar; k < tokens.size(); k++){
            distinct_string += tokens[k];
            if(k + 1 != tokens.size()) distinct_string += " ";
        }

        if(distinct_string.size() == 0)
            distinct_string = "\\L";

        new_row.push_back(distinct_string);
    }

    added_symbols.push_back(new_row);
    return optimized_string;
}

bool LeftAdjust::factor_row(vector <vector <string> > &input_grammer, vector <string> &non_terms, int cur_row){

    int i = cur_row, options_num = input_grammer[cur_row].size();
    bool is_factored = false;
    vector <bool> used_option(options_num, false);
    vector <int> optimized_group;
    vector <string> new_row;
    int mini_similar;
    string collected_start;

    vector <vector <int> > similar_start = get_similar_start(input_grammer, non_terms, cur_row);

    /*for(int j = 0; j < similar_start.size(); j++){
        for(int k = 0; k < similar_start[j].size(); k++){
            cout << similar_start[j][k] << " ";
        }
        cout << "\n";
    }*/

    for(int j = 0; j < options_num; j++){

        if(used_option[j]) continue;
        optimized_group.clear();
        optimized_group.push_back(j);
        mini_similar = INT_MAX;

        for(int k = j + 1; k < options_num; k++){

           // cout << !used_option[k] << " " << similar_start[j][k] << "\n";
            if(!used_option[k] && similar_start[j][k] != 0){
                optimized_group.push_back(k);
                used_option[k] = true;
                mini_similar = min(mini_similar, similar_start[j][k]);
            }
        }

        if(optimized_group.size() > 1){

            is_factored = true;
            collected_start = group_elements(input_grammer, non_terms, optimized_group, mini_similar, i);
            //cout << collected_start << "\n";
            new_row.push_back(collected_start);

        } else new_row.push_back(input_grammer[i][j]);
    }

    input_grammer[i] = new_row;
    return is_factored;
}

void LeftAdjust::adjust_left_factoring(vector <vector <string> > &input_grammer, vector <string> &non_terms){

    int non_term_num = input_grammer.size();
    bool is_factored;

    for(int i = 0; i < input_grammer.size(); i++){

        is_factored = true;
        added_symbols.clear();

        while(is_factored){
            is_factored = factor_row(input_grammer, non_terms, i);

            if(is_factored)
                for(int j = 0; j < added_symbols.size(); j++)
                    input_grammer.push_back(added_symbols[j]);
        }
    }
}

void LeftAdjust::get_left_recursion(vector <vector <string> > &input_grammer,
                             vector <string> &non_terms){

    adjust_left_recursion(input_grammer, non_terms);
}

void LeftAdjust::get_left_factoring(vector <vector <string> > &input_grammer,
                             vector <string> &non_terms){

    adjust_left_factoring(input_grammer, non_terms);
}
