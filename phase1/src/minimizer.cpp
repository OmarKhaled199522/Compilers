#include "minimizer.h"
minimizer::minimizer(vector< vector< pair<int,string> > > transition_table, vector< pair<int, string> > acceptance_states )
{
    this->transition_table = transition_table;
    this->acceptance_states = acceptance_states;


}
void minimizer::start_minimizer()
{
    extract_inputs();
    initialize_groups();
    do_partitioning();
}
void minimizer::extract_inputs()
{
    for(int i = 0; i<transition_table.size(); i++)
    {
        map<string, int> mapp;
        for(int j = 0; j<transition_table[i].size(); j++)
        {
            inputs.insert(transition_table[i][j].second);
            transition_table[i][j].first++; //to make it one-based states
            mapp[transition_table[i][j].second] = transition_table[i][j].first;
        }
        table_maps.push_back(mapp);
    }
}
void minimizer::initialize_groups()
{
    int state_no, group_no;
    vector<int> non_accepting ;
    pair<int, string> state;//state no , what it accepts
    for(int i = 0 ; i< acceptance_states.size(); i++)
    {
        acceptance_states[i].first++;
        state_no = acceptance_states[i].first;
        state = acceptance_states[i];
        if(pattern_group[state.second] == 0 )//not mapped to a group
        {
            vector<int> g ;
            g.push_back(state_no);
            groups.push_back(g);
            state_group[state_no] = groups.size();
            pattern_group[state.second] = groups.size();
        }
        else
        {
            group_no = pattern_group[state.second];
            groups[group_no - 1].push_back(state_no);
            state_group[state_no] = group_no;
        }
    }

    for(int i = 1; i <= transition_table.size(); i++)
    {
        if(state_group[i] == 0)
        {
            non_accepting.push_back(i);
            state_group[i] = groups.size() + 1;
        }
    }
    groups.push_back(non_accepting);
    for(int i =0; i<transition_table.size(); i++)
    {
        minimization_state.push_back(false);
    }
}

void minimizer::do_partitioning()
{
    vector <int> temp ;
    vector< vector<int> > new_groups;
    int cnt = 0;
    while(true)
    {
        new_groups.clear();
        //that's to iterate through groups once again if they are all not minimized
        //in the last iteration so break when cnt = 2
        if(is_finally_minimized())
        {
            cnt++;
        }
        else
        {
            cnt  = 0;
        }
        if(cnt == 2)
        {
            build_final_automata();
            test_print();
            break;
        }
        //**************************************************************************
        minimization_state.clear();
        for(int i=0 ; i<groups.size(); i++)
        {
            //   int yy = groups.size();
            temp = groups[i];
            vector< vector<int> > group_destinations, group_cuts;
            for(int j = 0 ; j<temp.size(); j++)
            {
                group_destinations.push_back(get_destination(temp[j]));

                //debug
                vector<int> v = group_destinations.back();
                int aa = v[0];
                int bb = v[1];
                int cc = v[2];
                //*****
            }
            group_cuts = separate_states(group_destinations, temp);
            if(group_cuts.size() == 1)
            {
                int a = group_cuts.size();
                minimization_state.push_back(true);
            }
            else
            {
                int a = group_cuts.size();
                for(int j= 0 ; j<group_cuts.size(); j++)
                {
                    minimization_state.push_back(false);
                }
            }
            for(int j= 0 ; j<group_cuts.size(); j++)
            {
                new_groups.push_back(group_cuts[j]);
            }
        }
        groups = new_groups;
        update_states_groups();
    }

}
vector <int> minimizer::get_destination(int state_no)
{
    vector<int> dest;
    map <string, int> temp;

    for (set<string>::iterator it = inputs.begin(); it != inputs.end(); it++)
    {
        temp = table_maps[state_no-1];
        string input = *it;
        dest.push_back(state_group[temp[input]]);
        int t = temp[input];
        int tt = state_group[temp[input]];
    }
    return dest;
}
//separate a group into other groups depending on the destinations
vector< vector<int> > minimizer::separate_states(vector< vector<int> > states_destinations, vector<int> states)
{
    vector< vector<int> > new_groups ;//states that entered a group
    map <int, bool> visited;
    for(int i = 0; i < states_destinations.size(); i++ )
    {
        if(visited[states[i]])
            continue;
        vector<int> temp ;
        temp.push_back(states[i]);
        visited[states[i]] = true;
        for(int j = i+1 ; j< states_destinations.size() ; j++)
        {
            if(!visited[states[j]] && states_destinations[i] == states_destinations[j])
            {
                temp.push_back(states[j]);
                visited[states[j]] = true;
            }
        }

        new_groups.push_back(temp);
    }
    return new_groups;


}
bool minimizer::is_finally_minimized()
{
    for(int i = 0; i< minimization_state.size() ; i++)
    {
        if(!minimization_state[i])
            return false;
    }
    return true;
}

void minimizer::update_states_groups()
{
    for(int i = 0; i < groups.size(); i++)
    {
        for(int j = 0; j<groups[i].size(); j++)
        {
            state_group[groups[i][j]] = i + 1;
        }
    }
}
void minimizer::build_final_automata()
{
    int state_representative;
    vector<int> destination ;
    for(int i = 0; i < groups.size(); i++)
    {
        int j =0;
        map<string, int> space;
        final_automata.push_back(space);
        state_representative = groups[i][0];
        destination = get_destination(state_representative);
        for (set<string>::iterator it = inputs.begin(); it != inputs.end(); it++)
        {
            final_automata[i][*it] = destination[j];
            int a = destination[j];
            j++;
        }

    }
    extract_final_acceptance_states();
    get_start_state();


    //call the recognizer********************************************************
    patterns_recognizer *p= new patterns_recognizer(final_acceptance_states, final_automata, inputs,final_start_state);
    p->start_recognizing();
    //***************************************************************************
}
void minimizer::extract_final_acceptance_states()
{
    int state;
    bool flag = false;
    for(int i = 0 ; i < groups.size(); i++ )
    {
        for(int j = 0; !flag && j < groups[i].size(); j++)
        {
            state = groups[i][j];
            for(int k = 0 ; !flag && k< acceptance_states.size(); k++)
            {
                if(acceptance_states[k].first == state)
                {
                    //final_acceptance_states.push_back(make_pair(i+1 ,acceptance_states[k].second ));
                    final_acceptance_states[i+1] = acceptance_states[k].second;
                    // cout<<i+1;
                    flag = true;


                }
            }
        }
        flag = false;
    }
}
void minimizer::get_start_state()
{
    int state;
    bool flag = false;
    for(int i = 0 ; i < groups.size(); i++ )
    {
        for(int j = 0; !flag && j < groups[i].size(); j++)
        {
            state = groups[i][j];
            if(state == 1)
            {
                final_start_state = i + 1;
                flag = true;
            }
        }
    }
}


void minimizer::test_print()
{
    freopen ("minimized_dfa.txt","w",stdout);
    for(int i = 0 ; i< groups.size(); i++)
    {
        for(int j = 0; j<groups[i].size(); j++)
        {
            cout << groups[i][j]<<" ";
        }

        cout<< "            ";
    }
    cout<<endl;

    cout << "states   ";

    for (set<string>::iterator it = inputs.begin(); it != inputs.end(); it++)
    {
        cout << *it<<"   ";
    }
    cout << endl;
    for(int i = 0 ; i<final_automata.size(); i++)
    {
        if(i+1 <= 9)
            cout<<"0";
        cout << i+1<<"       ";
        for (set<string>::iterator it = inputs.begin(); it != inputs.end(); it++)
        {
            if(final_automata[i][*it] < 10)
                cout<<"0";
            cout << final_automata[i][*it] << "  ";
        }
        cout<<endl;
    }
    cout<<endl;
    cout<<endl;
    cout<<endl;
    cout << "Start State : "<< final_start_state<<endl;
    map<int, string>::const_iterator itr;

    for(itr = final_acceptance_states.begin(); itr != final_acceptance_states.end(); ++itr)
    {
        cout << "State  " << (*itr).first << "  Accepts " << (*itr).second<<endl;
    }
    fclose (stdout);
}

