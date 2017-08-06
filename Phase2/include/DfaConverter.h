#ifndef DFACONVERTER_H
#define DFACONVERTER_H
#include <bits/stdc++.h>

class DfaConverter
{
    public:
        DfaConverter();
        virtual ~DfaConverter();
        void set_dfa(const std::vector< std::vector< std::pair< int, std::string> > > &);
        void set_final_states(std:: map < int, std::pair <std::string, int > > &);
        std::map <int, std::pair <std::string, int> > final_states_map;
        std::vector< std::vector< std::pair< int, std::string > > > states_after_dfa;
        std::vector <std::pair <int, std::string> > final_destinations_states;

    protected:

    private:
        void get_next_states_dfs(int, int, std::set <int> &, std::set <std::string> &, bool, std::vector <bool> &,
                                    const std::vector< std::vector< std::pair < int, std::string > > > &);

        void get_dfa_states(std::queue <std::set <int> > & , const std::set <std::string> & ,
                                  int , std::vector < std::vector < std::pair <int, std::string> > > &);

        void check_final_states(int, std::set <int>);
        //void set_connected_states (const std::vector< std::vector< std::pair < int, std::string > > > &);

        int new_state_num;
        static const std::string epislon;
        std::map < std::set <int> , int> state_num_map;
        std::map < std::pair < int, std::string > , std::set <int> > dfa_states_map;
        std::map < int, std::set <int> > connected_states_map;
};

#endif // DFACONVERTER_H
