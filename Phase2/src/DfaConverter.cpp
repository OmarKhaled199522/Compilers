#include <bits/stdc++.h>
#include "DfaConverter.h"

using namespace std;

const string DfaConverter::epislon = "\\L";

DfaConverter::DfaConverter()
{
    new_state_num = 0;
}

DfaConverter::~DfaConverter()
{
    //dtor
}

void DfaConverter::get_next_states_dfs(int ith_state, int global_state_num, set <int> & episoln_states,
                                set <string> & possible_inputs,
                                bool first_state, vector <bool> & visited_states,
                                const vector< vector< pair< int, string > > > & nfa_states){

    visited_states[ith_state] = true;

    if(first_state){
        episoln_states.insert(ith_state);
    }

    int branches_num = nfa_states[ith_state].size();

    for(int j = 0; j < branches_num; j++){

        int next_state = nfa_states[ith_state][j].first;
        string input = nfa_states[ith_state][j].second;

        if(input.compare(epislon) == 0){

            if(visited_states[next_state]) continue;
            get_next_states_dfs(next_state, global_state_num, episoln_states, possible_inputs,
                              first_state, visited_states, nfa_states);

        } else {

            if(!first_state){

                possible_inputs.insert(input);
                set <int> next_group = dfa_states_map[make_pair(global_state_num, input)];
                set <int> near_states = connected_states_map[next_state];
                next_group.insert(near_states.begin(), near_states.end());
                dfa_states_map[make_pair(global_state_num, input)] = next_group;

                /*for(auto hy: next_group){

                    cout << hy << " ";
                }*/
            }
        }
    }
}

void DfaConverter::check_final_states(int global_state_number, set <int> cur_states){

    int priority = 2e9; // unreachable low priority
    int num_of_state = -1;

    for(auto ith_state: cur_states){

        if(final_states_map.find(ith_state) != final_states_map.end()){


            int cur_priority = final_states_map[ith_state].second;
            if(cur_priority < priority){

                priority = cur_priority;
                num_of_state = ith_state;
            }
        }
    }

    if(priority != 2e9){

        final_destinations_states.push_back(make_pair(global_state_number, final_states_map[num_of_state].first));
    }

}

void DfaConverter::get_dfa_states(queue <set <int> > & queue_states, const set <string> & possible_inputs,
                                  int global_state_num, vector < vector < pair <int, string> > > & dfa_new_states){

    for(auto ith_state: possible_inputs){

        set <int> cur_state = dfa_states_map[make_pair(global_state_num , ith_state)];
        int state_number;

        /*
        cout << "cur state \n";

        for(auto tt : cur_state){

            cout << tt << " ";
        }

        cout << "\n";
        */

        if(state_num_map.find(cur_state) == state_num_map.end()){

            state_number = ++new_state_num;
            queue_states.push(cur_state);
            check_final_states(state_number, cur_state);

        } else {

            state_number = state_num_map[cur_state];
        }

        state_num_map[cur_state] = state_number;
        pair <int , string> next_state = {state_number, ith_state};
        dfa_new_states[global_state_num].push_back(next_state);
    }
}

void DfaConverter::set_dfa(const vector< vector< pair< int, string > > > & nfa_states){

    int num_nfa_states = nfa_states.size();
    queue <set <int> > states_queue;
    set <int>  dummy_empty_ints;
    vector< vector< pair< int, string > > > dfa_new_states;

    for(int i = 0; i < num_nfa_states; i++){

        set <int> start_set;
        vector <bool> visited (num_nfa_states, false);
        set <string>  dummy_empty_strings;

        get_next_states_dfs(i, 0, start_set, dummy_empty_strings, true, visited, nfa_states);

        if(i == 0){

            check_final_states(0, start_set);
            states_queue.push(start_set);
            state_num_map[start_set] = 0;
        }

        connected_states_map[i] = start_set;

        /*

        for(auto i : start_set){

            cout << i << " ";
        }

        cout << "\n";

        */
    }

    /*for(auto i : start_set){

      //  cout << i << " ";
    }*/

    while(!states_queue.empty()){

        set <int> cur_states = states_queue.front();
        states_queue.pop();
        set <string> possible_inputs;
        vector <bool> visited_states (num_nfa_states, false);

        for(auto ith_state: cur_states){

            if(!visited_states[ith_state]){
                get_next_states_dfs(ith_state, state_num_map[cur_states], dummy_empty_ints,
                                    possible_inputs, false, visited_states, nfa_states);
            }
        }

        vector < pair <int , string> > first_vec;
        dfa_new_states.push_back(first_vec);
        get_dfa_states(states_queue, possible_inputs, state_num_map[cur_states], dfa_new_states);
    }

    ofstream dfa_output;
    dfa_output.open ("dfa_output.txt");

    for(int i = 0; i < dfa_new_states.size(); i++){
        for(int j = 0; j < dfa_new_states[i].size(); j++){

            pair <int, string> test = dfa_new_states[i][j];
            dfa_output << "from state " << i << " to state " << test.first << " using input " << test.second << "\n";
        }
    }

    for(int i = 0; i < final_destinations_states.size(); i++){

        pair <int, string> test = final_destinations_states[i];
        dfa_output << "final state " << test.first << " for " << test.second << "\n";
    }



    states_after_dfa = dfa_new_states;
}
