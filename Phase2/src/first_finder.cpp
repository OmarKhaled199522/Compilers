#include "first_finder.h"

first_finder::first_finder(vector<element*> non_terminals)
{
    this->non_terminals = non_terminals;
    calculate_first();
    _follow_finder = new follow_finder(non_teminals_first, non_terminals);
    complete_table();
}

first_finder::~first_finder()
{
    //dtor
}
void first_finder:: calculate_first()
{
    set<string> temp;
    for(unsigned i = 0;i< non_terminals.size();i++)
    {
        non_teminals_first[non_terminals[i]] = temp;
    }
    for(unsigned i = 0;i< non_terminals.size();i++)
    {
        if(!finished[non_terminals[i]->value])
        {
            calculate_element_first(non_terminals[i]);
            //visited.clear();

        }
        set<string> s2 = non_teminals_first[non_terminals[i]];
//        cout <<non_terminals[i]-> value <<" :  ";
//        for (set<string>::iterator it = s2.begin(); it != s2.end(); it++)
//        {
//            cout << *it << " ";
//        }
//        cout<<endl;

    }
}
set<string> first_finder::calculate_element_first(element *elem)
{
   // visited[elem->value] = true;
    if(elem->is_terminal)
    {
        set<string> temp;
        temp.insert(""+elem->value);
        return temp;
    }
    for(unsigned i = 0;i<elem->right_side_elements.size();i++)
    {

        bool flag = false;
        for(unsigned j = 0;j<elem->right_side_elements[i].size();j++)
        {

//            if(visited[elem->right_side_elements[i][j]->value])
//            {
//                break;
//            }
            //getting the first of a right hand side element
            set<string> tmp ;
            if(!finished[elem->right_side_elements[i][j]->value])
            {
                tmp = calculate_element_first(elem->right_side_elements[i][j]);
            }
            else{
                tmp = non_teminals_first[elem->right_side_elements[i][j]];
            }
            //***************concatenating the production*********************
            vector<element*> v = elem->right_side_elements[i];
            string production = "";
            for(unsigned k = 0;k<v.size();k++)
            {
                production =production +  v[k]->value+" ";
            }
            //*****************building predictive table**********************
            for (set<string>::iterator it = tmp.begin(); it != tmp.end(); it++)
            {
               // predictive_table[elem->value][*it].push_back(production);
                unsigned x = 0;
                for(unsigned k = 0;k<v.size();k++)
                {
                    predictive_table[elem->value][*it].push_back(v[k]->value);
                    x += v[k]->value.size() + 1;
                }
                if (max_lenght < x)
                    max_lenght = x;
                //cout << elem->value << " \"for\"  "<<*it<< "  \"choose production\"  "<<predictive_table[elem->value][*it][0] <<endl;
                //cout << predictive_table[elem->value][*it].size()<<endl;
            }
            //*****************************************************************
            bool contains_eps = tmp.count("\\L");
            if(!contains_eps)
            {
                flag = true;
            }
            //*****************************************************************


            if( !(j == elem->right_side_elements[i].size()-1 &&  !flag ) )
            {
                tmp.erase("\\L");
            }
            non_teminals_first[elem] = get_sets_union(non_teminals_first[elem] , tmp);
            if(!contains_eps)
            {
                break;
            }
        }
    }
    finished[elem->value] = true;
    return non_teminals_first[elem];
}



set<string> first_finder::get_sets_union(set<string> s1 , set<string> s2)
{
    for (set<string>::iterator it = s2.begin(); it != s2.end(); it++)
    {
        s1.insert(*it + "");
    }
    return s1;
}

void first_finder::complete_table()
{
    vector<string> follows;

    for(unsigned i=0;i<non_terminals.size();i++)
    {
        string value = non_terminals[i]->value;
        follows = _follow_finder->follow(value);
        //cout << "follow of "<<value << "  :  ";
        if(non_teminals_first[non_terminals[i]].count("\\L"))
        {
            for(unsigned j = 0;j< follows.size();j++)
            {
                if(predictive_table[value][follows[j]].size() > 0)
                    continue;
                predictive_table[value][follows[j]].push_back("\\L");
                //cout<<follows[j]<<" ";
            }
        }
        else{
            for(unsigned j = 0;j< follows.size();j++)
            {
                if(predictive_table[value][follows[j]].size() > 0)
                    continue;
                predictive_table[value][follows[j]].push_back("synch");

                //cout<<follows[j]<<" ";
            }
        }
        //cout<<endl;
    }
}
/*
void first_finder::print(map<string , map<string , vector<string> > > predictive_table)
{
    ofstream output("predictive table.txt");
    map<string , map<string , vector<string> > >::iterator it;
    for(it = predictive_table.begin() ; it != predictive_table.end(); ++it)
    {
        output << it->first << ":" << endl;
        map<string , vector<string> >::iterator it2;
        for(it2 = it->second.begin() ; it2 != it->second.end(); ++it2)
        {
            if(it2->first == "\\L")
                continue;
            output << "\t" << it2->first << ":\t" ;
            vector<string>::iterator it3;
            for(it3 = it2->second.begin() ; it3 != it2->second.end(); ++it3)
                output << *it3 << " ";
            output << endl;
        }
    }
    output.close();
}*/

void first_finder::print(map<string , map<string , vector<string> > > predictive_table)
{
    ofstream output("predictive table.txt");
    map<string , map<string , vector<string> > >::iterator row_it;
    map<string , vector<string> >::iterator column_it;
    set<string> terminal;
    set<string>::iterator terminal_it;
    const char separator = ' ';
    //cout << max_lenght << endl;
    const int width = max_lenght;
    for(row_it = predictive_table.begin() ; row_it != predictive_table.end(); ++row_it)
    {
        for(column_it = row_it->second.begin() ; column_it != row_it->second.end(); ++column_it)
        {
            if(column_it->first == "\\L")
                continue;
            terminal.insert(column_it->first);
        }
    }
    output << left << setw(width) << setfill(separator) << ' ';
    unsigned i = 1;
    for(terminal_it = terminal.begin() ; terminal_it != terminal.end(); ++terminal_it)
    {
        output << left << setw(width) << setfill(separator) << *terminal_it;
        i++;
    }
    output << endl;
    for(i ; i > 0; i--)
    {
        output << left << setw(width) << setfill('_') << "_";
    }
    output << endl;
    for(row_it = predictive_table.begin() ; row_it != predictive_table.end(); ++row_it)
    {
        output  << left << setw(width) << setfill(separator) << row_it->first;
        terminal_it = terminal.begin();
        //cout << "----" << endl;
        for(column_it = row_it->second.begin();
            column_it != row_it->second.end();
            ++column_it)
        {
            if(column_it->first == "\\L")continue;
            for(;terminal_it != terminal.end(); ++terminal_it)
            {
                //cout << column_it->first  << " " << *terminal_it << endl;
                if(column_it->first != *terminal_it)
                    output << left << setw(width) << setfill(separator) << ' ';
                else
                {
                    ++terminal_it;
                    break;
                }
            }
            string s = "";
            for(unsigned i =0; i < column_it->second.size(); i++)
            {
                s += column_it->second[i] + " ";
            }
            output << left << setw(width) << setfill(separator) << s;
        }
        output << endl;
    }
    output.close();
}

