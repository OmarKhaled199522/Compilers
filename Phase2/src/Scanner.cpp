#include "Scanner.h"

Scanner::Scanner()
{
    scan_log.open ("log.txt");
    priority = 0;
    line_number = 0;
}

Scanner::~Scanner()
{

}

/**read**/

vector<string> Scanner::read_rules(string path)
{
    ifstream file(path.c_str());
    vector<string> lines;
    string line;
    while(getline(file, line))
    {
		lines.push_back(line);
    }
    file.close();
    return lines;
}

/**check input**/

void Scanner::parse (vector<string> lines)
{
    enum pattern
    {
        Definition,
        Expression,
        Keyword,
        Punctuation
    };
    regex rules [] = {regex ("^[[:s:]]*([[:alnum:]]+)[[:s:]]*=[[:s:]]*(.+)$"),
                      regex ("^[[:s:]]*([[:alnum:]]+)[[:s:]]*:[[:s:]]*(.+)$"),
                      regex ("^[[:s:]]*\\{(.+)\\}[[:s:]]*$"),
                      //regex ("^\\s*\\[(.+)\\]\\s*$")
                      regex ("^[[:s:]]*\\[(.+)\\][[:s:]]*$")
                     };
    string line;
    smatch match;
    for (unsigned i=0 ; i < lines.size() ; i++)
    {
        line = lines[i];
        line_number = i + 1;
        if (regex_search(line, match, rules[Definition]))
        {
            parse_definition (match[1], match[2]);
        }
        else if (regex_search(line, match, rules[Expression]))
        {
            parse_expression (match[1], match[2]);
        }
        else if (regex_search(line, match, rules[Keyword]))
        {
            parse_keyword (match[1].str());
        }
        else if (regex_search(line, match, rules[Punctuation]))
        {
            parse_punctuation (match[1].str());
        }
        else
        {
            scan_log << "can't read line " << line_number << " : " << line <<endl;
        }
    }
}

/**parse rules**/

void Scanner::parse_punctuation (string punctuations)
{
    replace_all(punctuations, " ", "");
    vector<char> symbols = str_to_vector(punctuations);
    for (vector<char>::const_iterator i = symbols.begin(); i != symbols.end(); ++i)
    {
        nfa.set_punctuations(*i, priority++);
    }
}

void Scanner::parse_keyword (string keyword)
{
    regex r ("(\\w+)\\s*(.*)");
    smatch m;
    while (regex_search(keyword, m, r))
    {
        vector<char> key = str_to_vector(m[1].str());
        nfa.set_key_word(key, priority++);
        if (m.size() == 1) break;
        keyword = m[2].str();
    }

}

void Scanner::parse_definition (string definition_id, string definition)
{
    nfa.set_difinition (definition_id, build_expression_graph (definition));
}

void Scanner::parse_expression (string expression_id, string expression)
{
     nfa.add_expresion (expression_id, build_expression_graph (expression), priority++);
}

/**Operation**/

bool Scanner::is_operation (char c)
{
    return is_single_operation (c) || is_double_operation (c);
}

bool Scanner::is_single_operation (char c)
{
    return c == '*' || c == '+' || c == '?';
}

bool Scanner::is_double_operation (char c)
{
    return c == '-'|| c == '|';
}

/**build expression**/

graph_t Scanner::build_expression_graph (string str)
{
    map<string, graph_t > graph_map;
    vector<string> postfix = convert_expression_to_postfix (str, &graph_map);
    stack <graph_t > operand_stack;
    unsigned i = 0;
    while(i < postfix.size())
    {
        string s = postfix[i++];
        if(is_single_operation (s[0]))
        {
            if (operand_stack.empty())
                false_rules();
            graph_t op_graph = operand_stack.top();
            operand_stack.pop();
            if (s[0] == '*')
            {
                operand_stack.push(nfa.closure(op_graph));
            }
            else if (s[0] == '+')
            {
                operand_stack.push(nfa.positive_closure(op_graph));
            }
            else if (s[0] == '?')
            {
                operand_stack.push(nfa.option_operator(op_graph));
            }
        }
        else if (is_double_operation (s[0]) || s[0] == '&')
        {

            if (operand_stack.size() < 2)
                false_rules();
            graph_t op1_graph = operand_stack.top();
            operand_stack.pop();
            graph_t op2_graph = operand_stack.top();
            operand_stack.pop();
            if (s[0] == '|')
            {
                operand_stack.push(nfa.OR (op2_graph, op1_graph));
            }
            else if (s[0] == '-')
            {
                operand_stack.push(multi_or (op2_graph, op1_graph));
            }
            else if (s[0] == '&')
            {
                operand_stack.push(nfa.AND (op2_graph, op1_graph));
            }

        }
        else
        {
            operand_stack.push(graph_map[s]);
        }
    }
    graph_map.clear();
    return operand_stack.top();
}

void Scanner::false_rules()
{
    scan_log << "Error: can't scan grammar file line " << line_number << " invalid" << endl;
    exit(0);
}

graph_t Scanner::build_word (string str)
{
    graph_t graph = nfa.get_definition(str);
    if (graph.empty () && (str == "\\L" || str.size() == 1))
    {
        graph = nfa.constituent_subexpressions(str);
    }
    else if (graph.empty ())
    {
        graph =  nfa.multi_and (str_to_vector (str));
    }
    return graph;
}

graph_t Scanner::multi_or (graph_t a, graph_t b)
{
    string ca = a[0][0].second;
    string cb = b[0][0].second;
    return  nfa.multi_or(range_chars (ca[0], cb[0]));
}

/**postfix**/

int Scanner::operation_priority(char a)
{
    int temp;
    if (a == '-')
        temp = 1;
    else if (a == '*' || a == '+' || a == '?')
        temp = 2;
    else  if (a == '&')
        temp = 3;
    else  if (a == '|')
        temp = 4;
    return temp;
}

void Scanner::set_operand (string* operand,map<string, graph_t >* graph_map, vector<string>* output)
{
    map<string, graph_t > &mappings = *graph_map;
    string &_operand = *operand;
    vector<string>& _output = *output;
    string index = _operand;
    if(_operand != "\\L")
        replace_all(_operand, "\\", "");
    mappings[index] = build_word (_operand);
    _output.push_back(index);
    _operand = "";
}

string Scanner::check_infix (string infix)
{
    string operand = "";
    string output = "";
    for (unsigned i = 0; i < infix.length(); i++) {
        if (is_double_operation (infix[i]) || infix[i] == '&' ||
            infix[i] == ')' || infix[i] == '(' || is_single_operation (infix[i])){
            graph_t graph = nfa.get_definition(operand);
            if (graph.empty () && !(operand == "\\L" || operand.size() == 1))
            {
                operand = anding(operand);
            }
            output += operand;
            output += infix[i];
            operand = "";
        } else {
            operand += infix[i];
            if (infix[i] == '\\')
                operand += infix[++i];
        }
    }
    output += operand;
    return output;
}

string Scanner::anding (string str)
{
    string s = "";
    for (unsigned i=0 ; i < str.size(); i++)
    {
        s += str[i];
        if (i!=str.size()-1 && str[i] != '\\')s += "&";
    }
    return s;
}

vector<string> Scanner::convert_expression_to_postfix (string infix , map<string, graph_t >* graph_map)
{
    map<string, graph_t > &mappings = *graph_map;
    replace_all(infix, "&", "\\&");
    infix = rebuildexp (infix);
    infix = check_infix (infix);
    stack<char> operator_stack;
    string operand = "";
    vector<string> output;
    for (unsigned i = 0; i < infix.length(); i++) {
        if (is_double_operation (infix[i])|| infix[i] == '&'
        						 || is_single_operation (infix[i])){
            if (operand.size() != 0)
                set_operand(&operand, &mappings, &output);
            while (!operator_stack.empty() && operation_priority(operator_stack.top())
            			 <= operation_priority(infix[i]) && operator_stack.top() != '(') {
                output.push_back(char_to_string(operator_stack.top()));
                operator_stack.pop();
            }
            operator_stack.push(infix[i]);
        } else if (infix[i] == '(') {
            if (operand.size() != 0)
                set_operand(&operand, &mappings, &output);
            operator_stack.push(infix[i]);
        } else if (infix[i] == ')') {
            if (operand.size() != 0)
                set_operand(&operand, &mappings, &output);
            while (operator_stack.top() != '(') {
                output.push_back(char_to_string(operator_stack.top()));
                operator_stack.pop();
            }
            operator_stack.pop();
        } else {
            operand += infix[i];
            if (infix[i] == '\\')
                operand += infix[++i];
        }
    }
    if (operand.size() != 0)
    {
        set_operand(&operand, &mappings, &output);
    }
    while (!operator_stack.empty()) {
        output.push_back(char_to_string(operator_stack.top()));
        operator_stack.pop();
    }
    return output;
}

/**helper**/

vector<char> Scanner::range_chars (char first, char last)
{
    vector<char> v;
    for (int j = first ; j <= last ; j++)
    {
        v.push_back(j);
    }
    return v;
}

int Scanner::skip_space (string str, int i)
{
    i++;
    while (str[i] == ' ' || str[i] == '\t')
    {
        i++;
    }
    return i;
}

string Scanner::rebuildexp (string str)
{
    string newstr = "";
    unsigned i = -1;
    i = skip_space (str, i);
    while(i < str.size())
    {
        if (is_double_operation (str[i]) || str[i] == '(' || newstr.size() == 0)
        {
            newstr += str[i];
            i = skip_space (str, i);
        }
        else if (is_single_operation (str[i]))
        {
            unsigned j = i;
            newstr += str[i];
            i = skip_space (str, i);
            if (is_double_operation (str[i]) || is_single_operation (str[i])
            												|| str[i] == ')')
            {
                continue;
            }
            else if (i > j+1 && i < str.size())
            {
                i = j + 1;
                newstr += "&";
                i = skip_space (str, i);
            }
            else if (i < str.size())
            {
                i = j + 1;
            }
        }
        else
        {
            unsigned j = i;
            newstr += str[i];
            i = skip_space (str, i);
            if (is_double_operation (str[i]) || is_single_operation (str[i])
            									|| str[i] == ')')
            {
                continue;
            }
            else if (i > j+1 && i < str.size())
            {
                i = j + 1;
                newstr += "&";
                i = skip_space (str, i);
            }
            else if (i < str.size())
            {
                i = j + 1;
            }
        }
    }
    return newstr;
}

void Scanner::replace_all(string& str, const string& from, const string& to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

vector<char> Scanner::str_to_vector (string str)
{
    vector<char> v;
    for (unsigned i = 0 ; i < str.size(); i++)
    {
        if (str[i] == '\\' && i < str.size() - 1)
            i++;
        v.push_back(str[i]);
    }
    return v;
}

string Scanner::char_to_string (char c)
{
    string s = "";
    return s+c;
}

/**unused**/
vector<string> Scanner::split (string str , char spliter)
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
        v.push_back(s);
    }
    return v;
}

/*get nfa graph */
graph_t Scanner::get_nfa_graph()
{
    return nfa.get_nfa_graph();
}
/*return acceptance state*/
map<int , pair<string, int> > Scanner::get_accept_state_map()
{
    return nfa.get_accept_state_map();
}

/**test**/
void Scanner::print_nfa()
{
    ofstream nfa_output;
    nfa_output.open ("nfa_output.txt");
    nfa_output << "-------- NFA Graph --------" << endl;
    graph_t graph = get_nfa_graph();
    for (unsigned i = 0 ; i < graph.size() ; i++)
    {
        int boolean = 0 ;
        for (unsigned j = 0 ; j < graph[i].size() ; j++)
        {
            boolean = 1 ;
            nfa_output << "from " << i <<  " -> " <<  graph[i][j].first << "  with value = " << graph[i][j].second << endl;
        }
        if (!boolean) nfa_output << "from " << i << " no edges" << endl;
    }
    nfa_output << "-------- NFA Accept State --------" << endl;
    map<int , pair<string, int> > m = get_accept_state_map();

    map<int , pair<string, int> >::iterator i ;

    for (i = m.begin() ; i != m.end() ; i++)
    {
        nfa_output << "state index : " << (*i).first <<"\t\t token : "<<(*i).second.first << "\t\t priority :" << (*i).second.second << endl ;
    }
    nfa_output.close();
}

