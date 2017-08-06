#include "scanner_gammar.h"

scanner_gammar::scanner_gammar()
{
    //ctor
}

scanner_gammar::~scanner_gammar()
{
    //dtor
}


string scanner_gammar::get_nonTerminal(string *line)
{
    string &_line = *line;
    string s ="";
    regex rx ("^[[:s:]]*#[[:s:]]*(.+)[[:s:]]*[^\']=(.+)$");
    smatch m;
    if (regex_search(_line, m, rx))
    {
        s = m[1];
    }
    _line = m[2];
    return s;
}

vector<string> scanner_gammar::split (string str , char spliter)
{
    vector<string> v;
    unsigned i=0;
    while (i < str.size())
    {
        string s = "";
        while(str[i] != spliter && i < str.size())
        {
           s += str[i];
           i++;
        }
        i++;
        if(!s.empty())
        {
            regex rx("^[[:s:]]*(.+)[[:s:]]*$");
            smatch m;
            if (regex_search(s, m, rx))
            {
                s = m[1];
            }
            v.push_back(s);
        }
    }
    return v;
}

vector<string> scanner_gammar::read_file(string path)
{
    ifstream file(path.c_str());
    if(!file.is_open())
    {
        file.close();
        return {};
    }
    vector<string> lines;
    string line;
    while(getline(file, line))
    {
		lines.push_back(line);
    }
    file.close();
    return lines;
}

int scanner_gammar::find_element (vector<element*> grammar, element elem)
{
    int i=0;
    for (vector<element*>::iterator it = grammar.begin() ; it != grammar.end(); ++it)
    {
        if((*it)->value == elem.value)
        {
            return i;
        }
        i++;
    }
    return -1;
}

vector<element*> scanner_gammar::convert (vector<vector<string> > rhss, vector<string> nonTerminal)
{
    vector<element*> grammar;
    for(unsigned i=0; i < nonTerminal.size(); i++)
    {
        element *elem = new element();
        elem->is_terminal = 0;
        elem->value = nonTerminal[i];
        grammar.push_back(elem);
    }
    for(unsigned i=0; i < rhss.size(); i++)
    {
        vector<vector<element*> > rhs;
        for(unsigned j = 0 ; j < rhss[i].size(); j++)
        {
            vector<element*> rhs_elem;
            vector<string> elems = split (rhss[i][j] , ' ');
            for(unsigned k=0; k < elems.size(); k++)
            {
                element *elem = new element();
                elem->is_terminal = 0;
                elem->value = elems[k];
                int index = find_element(grammar, *elem);
                if(index == -1)
                {
                    elem->is_terminal = 1;
                    if (elems[k] != "\\L")
                        elem->value = elems[k].substr(1,elems[k].size() - 2);
                    rhs_elem.push_back(elem);
                }
                else
                    rhs_elem.push_back(grammar[index]);
            }
            rhs.push_back(rhs_elem);
        }
        grammar[i]->right_side_elements = rhs;
    }
    return grammar;
}

vector<string> scanner_gammar::get_grammar(vector<string> file, vector<vector<string> >* rhss)
{
    vector<vector<string> > _rhss;
    vector<string> grammar;
    for(unsigned i = 0 ; i < file.size() ; i++)
    {
        string line = file[i];
        while (i+1 < file.size() && file[i+1][0] != '#')
        {
            line += file[++i];
        }
        grammar.push_back(get_nonTerminal(&line));
        _rhss.push_back(split (line , '|'));
    }
    *rhss = _rhss;
    return grammar;
}

void scanner_gammar::print_grammar(vector<element*> grammar)
{
    for(unsigned i = 0; i < grammar.size(); i++)
    {
        element *e = grammar[i];
        cout << e->is_terminal << " : " << e->value << " = ";
        for(unsigned j = 0 ; j < grammar[i]->right_side_elements.size(); j++)
        {
            if (j != 0)cout << " | ";
            for(unsigned k = 0; k < grammar[i]->right_side_elements[j].size(); k++)
            {
                cout << grammar[i]->right_side_elements[j][k]->is_terminal <<
                 ":" << grammar[i]->right_side_elements[j][k]->value << " ";
            }
        }
        cout << endl;
    }
}
