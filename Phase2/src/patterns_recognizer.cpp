#include "patterns_recognizer.h"
#include <iostream>
#include <fstream>
patterns_recognizer::patterns_recognizer(map<int,string> final_acceptance_states,
        vector< map<string, int> > final_automata,
        set<string> inputs,
        int final_start_state)
{
    this->final_acceptance_states = final_acceptance_states;
    this->final_automata = final_automata;
    this->final_start_state = final_start_state;
    this->inputs = inputs;

}
void patterns_recognizer::start_recognizing()
{
    read_file();
    parse_file();
    print_symbol_table();
}
void patterns_recognizer::read_file()
{
    ifstream file("code.txt");
    string str;
    while (std::getline(file, str))
    {
       //cout << str<<endl;
        input_file_lines.push_back(str);
    }
    //cout<<endl<<endl;

}
void patterns_recognizer::parse_file()
{
    for(int i = 0; i < input_file_lines.size(); i++)
    {
        parse_line(input_file_lines[i]);
    }
    ofstream myfile;
    myfile.open ("tokens.txt");
   // freopen ("tokens.txt","w",stdout);
    for(int i = 0;i<output_file_lines.size();i++)
    {
        myfile <<output_file_lines[i]<< endl ;
    }
   // fclose (stdout);


}
void patterns_recognizer::parse_line(string line)
{
    vector<char> connected_string;
    for(int i = 0; i<line.length(); i++)
    {
        while(line[i] == ' ' || line[i] == '\t')
        {
            if(connected_string.size()!= 0)
            {
                std::string s = string(connected_string.begin(), connected_string.end());
                parse_connected_string(s);
                connected_string.clear();
            }
            i++;

        }
        connected_string.push_back(line[i]);
        if( i == line.length()-1 && connected_string.size()!= 0)
        {
            std::string s = string(connected_string.begin(), connected_string.end());
            parse_connected_string(s);
        }
    }

}

void patterns_recognizer::parse_connected_string(string connected_string)
{
    int current_state = final_start_state;
    map <string,int> mapp;
    int previous_state = -1;
    string last_accepted_lexeme = "";
    string last_accepted_token = "";
    int last_pattern_index =0,last_error_index=-1;
    string tmp;
    vector<char> temp;
    for(int i = 0;i< connected_string.length();i++)
    {
        //if the input is not part of the alphabet
        if(!inputs.count(tmp+connected_string[i]) )
        {
            //cout << connected_string <<endl;
            output_file_lines.push_back(last_accepted_token);
            output_lexemes.push_back(last_accepted_lexeme);
            last_accepted_token="";
            current_state = final_start_state;
            temp.clear();
            last_accepted_lexeme = "";
            last_pattern_index = 0;
            output_file_lines.push_back("Sorry, it isn't from the alphabet.");
            output_lexemes.push_back(" ");
            continue;
        }
        //******************************************
        temp.push_back(connected_string[i]);
        if(current_state != 0)
            mapp = final_automata[current_state-1];
          else{
               // mapp = final_automata[final_start_state-1];
               // temp.clear();
            }
        current_state = mapp[tmp+connected_string[i]];
        int p = current_state;
        bool flag = false;
        if(final_acceptance_states[current_state] != "")
        {
            last_accepted_lexeme = string(temp.begin(),temp.end());
            last_accepted_token = final_acceptance_states[current_state];
            last_pattern_index = i;
        }
        while (true)
        {
            mapp = final_automata[ final_start_state -1];

            p = mapp[tmp+connected_string[i]];
            if(p != 0)
                mapp = final_automata[ p -1];
            else{
               // mapp = final_automata[final_start_state-1];
               // temp.clear();
            }
            if(last_accepted_lexeme == "" &&( i+1 == connected_string.length() ||
                                              (i+1 <connected_string.length() &&
                                                mapp[tmp+connected_string[i+1]] == 0)  ))
            {
                output_file_lines.push_back("error");
                output_lexemes.push_back(" ");
                i++;
                flag = true;
                last_pattern_index = i-1;
                temp.clear();
            }
            else{
                   // temp.clear();
                 break;
            }

        }
        if(flag)
        {
            i--;
            continue;
        }


        //can't proceed
        if(last_accepted_lexeme != "" && (current_state == 0 || i == connected_string.length() -1))
        {
            output_file_lines.push_back(last_accepted_token);
            output_lexemes.push_back(last_accepted_lexeme);
            last_accepted_token="";
            current_state = final_start_state;
            temp.clear();
            last_accepted_lexeme = "";
            i = last_pattern_index ;
            last_pattern_index = 0;
        }

    }
}
void patterns_recognizer::print_symbol_table()
{
    //freopen ("symbol table.txt","w",stdout);
    ofstream myfile;
    myfile.open ("symbol table.txt");
    for(int i = 0;i<output_file_lines.size();i++)
    {
        if(output_lexemes[i] != " " && output_lexemes[i] != "" )
        {
            myfile <<output_file_lines[i]<< "\t\t";
            myfile <<output_lexemes[i] << endl;
        }

    }
    //fclose (stdout);

}
