#include "Panic_Mode_Recovery.h"

Panic_Mode_Recovery::Panic_Mode_Recovery
(next_token *next, string start_state, map<string, map<string, vector<string> > > predictive_table)
{
    this->next = next ;
    this->start_state = start_state ;
    this->predictive_table = predictive_table ;
    output.open("panic_mode_output.txt");
}

Panic_Mode_Recovery::~Panic_Mode_Recovery()
{
    //dtor
    this->predictive_table.clear();
    this->start_state.clear() ;
    this->stack_string.clear() ;

}
// Table map<string , map<string , vector<string> > >
void Panic_Mode_Recovery::Panic_Mode_Checker()
{
    stack<string> stack_table ;

    stack_table.push("$") ;
    stack_table.push(this->start_state) ;

    ///stack_string.push_back("$") ;
    stack_string.push_back(this->start_state) ;

    //output <<left <<setw(width) << setfill(separator)<< "stack" << "output" << endl ;

    string tocken_input = next->get_next_token() ;


    /*while we not reach to the end of the sentence which end with "$*/
    while (!(stack_table.top() == "$" || tocken_input == "$"))
    {
        print_stack() ;

        string tos = stack_table.top() ; // top of stack

        //    cout <<left <<setw(width) << setfill(separator)<< tos;
        //cout <<left <<setw(width) << setfill(separator)<< tocken_input;
        if (tos == tocken_input)
        {
            stack_table.pop() ;
            stack_string.pop_back () ;

            tocken_input = next->get_next_token() ;
            output << "\n" ;
            continue ;
        }


        if (predictive_table.find(tos) != predictive_table.end()) // exist in map
        {
            if (predictive_table[tos].find(tocken_input) != predictive_table[tos].end())
            {
                stack_table.pop() ;
                stack_string.pop_back () ;

                vector<string> v = predictive_table[tos][tocken_input] ;

                if (v[0] == "\\L")
                {
                    ///output << "LAMBDA" ;
                }
                else if (v[0] == "synch")
                {
                    output << "synchronizing token" ;
                }
                else
                {
                    for (int i = 0 ; i < v.size() ; i++)
                    {
                        ///output << v[i] << " " ;

                        stack_table.push(v[v.size() -i-1]) ;
                        stack_string.push_back(v[v.size() -i-1]) ;
                    }
                }
            }
            else
            {
                output << "Error:(illegal "<<tos<<") -- discard "<<tocken_input ;

                tocken_input = next->get_next_token() ;
            }
        }
        else
        {
            output << "Error: missing "<<tos<< ",inserted"; // print error ;

            stack_table.pop() ;
            stack_string.pop_back () ;
        }
        output << "\n" ;
    }
    if (stack_table.top() == "$" && tocken_input == "$")
    {
        output << "$\t\t\tACCEPT" ;
    }
    else if ( stack_table.top() == "$")
    {
        output << "Error :: those are over sentences => " ;
        while ( tocken_input != "$")
        {
            output << tocken_input << " " ;
            tocken_input = next->get_next_token() ;
        }
        output << "\n" ;
        output << "$\t\t\tNot ACCEPT" ;
    }
    else
    {
        while (stack_table.top() != "$")
        {
            while (stack_table.top() != "$")
            {
                string tos = stack_table.top() ;
                print_stack() ;

                if (predictive_table.find(tos) != predictive_table.end()) // exist in map
                {
                    if (predictive_table[tos].find(tocken_input) != predictive_table[tos].end())
                    {


                        vector<string> v = predictive_table[tos][tocken_input] ;

                        if (v[0] == "\\L")
                        {
                            ///output << "LAMBDA\n" ;
                            output << endl;
                        }
                        else
                        {
                            break ;
                        }
                        stack_table.pop() ;
                        stack_string.pop_back () ;
                    }
                    else
                    {
                        output << "Error:(illegal "<<tos<<") -- discard "<<tocken_input ;
                        break ;
                        tocken_input = next->get_next_token() ;
                    }
                }
                else
                {
                    output << "Error: missing "<<tos<< ",inserted\n"; // print error ;
                    stack_table.pop() ;
                    stack_string.pop_back () ;
                }
            }

            if (stack_table.top() != "$")
            {
                output << "Error :: those are recommended => " ;

                string tos = stack_table.top() ;

                map<string, vector<string> > m = predictive_table[tos] ;

                map<string, vector<string> >::iterator column_it;
                int i = 0 ;
                for(column_it = m.begin() ; column_it != m.end(); ++column_it)
                {
                    string recommended = column_it->first ;
                    vector<string> vec = column_it->second ;
                    if (vec[0] == "synch") continue ;
                    if (i) output << " , " ;
                    output << recommended  ;
                    i++ ;
                }
                output << endl;
                stack_table.pop() ;
                stack_string.pop_back () ;
            }
            else
            {
                output << "ACCEPT \n" ;
            }
        }
    }
    output.close();
}
void Panic_Mode_Recovery::print_stack()
{
    string printed_str = "" ;
    for (int i = 0 ; i < stack_string.size() ; i++)
        printed_str+= stack_string[i] + " " ;
    output <<left <<setw(width) << setfill(separator)<< printed_str ;
}


