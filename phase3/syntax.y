%{
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <map>
#include <cstring>
#include <vector>
#include <unistd.h>

using namespace std;

extern int yylex();
extern FILE *yyin;

extern int lineCounter;
void yyerror(const char * s);

string outfileName;

ofstream fout("outputfile.j");

void setCodeSteps(vector<int> *list, int num);
void printCode(void);
string initiateLabel();
void castGeneralOperations(int from , int to, string op);
void castGeneralRelations(string op,char * nTrue, char * nFalse);
bool checkId(string id);
void setAllowedVariables(string name, int type);
vector<int> * getNextStep (vector<int> *list1, vector<int>* list2);


typedef enum {INT_T, FLOAT_T, BOOL_T, VOID_T, ERROR_T} type_enum;

map<string,string> operators_map = {
	/* arithmetic operations */
	{"+", "add"},
	{"-", "sub"},
	{"/", "div"},
	{"*", "mul"},
	{"|", "or"},
	{"&", "and"},
	{"%", "rem"},
	/* relational op */
	{"==", "if_icmpeq"},
	{"<=", "if_icmple"},
	{">=", "if_icmpge"},
	{"!=", "if_icmpne"},
	{">",  "if_icmpgt"},
	{"<",  "if_icmplt"}
};
map<string, pair<int,type_enum> > symbolTable;
vector<string> codeSteps;

int globalVarNum = 1;
int labelCounter = 0;

%}

%code requires {
	#include <vector>
	using namespace std;
}

%start METHOD_BODY

%union{

	int intValue;
	int boolValue;
	int type;
	float floatValue;
	char * idValue;
	char * aopValue;
	int expr_type;

	struct {
		vector<int> *trueList, *falseList;
	} bexpr_type;

	struct {
		vector<int> *nextList;
	} stmt_type;
}

%token <intValue> INT
%token <floatValue> FLOAT
%token <boolValue> BOOL
%token <idValue> IDENTIFIER

%left <aopValue> ARITH_OP
%left <aopValue> RELA_OP
%left <aopValue> BOOL_OP

%token INT_WORD
%token FLOAT_WORD
%token BOOLEAN_WORD
%token IF_WORD
%token ELSE_WORD
%token WHILE_WORD
%token FOR_WORD
%token SYSTEM_OUT

%nonassoc ';'
%left '='
%nonassoc '('
%nonassoc ')'
%nonassoc '{'
%nonassoc '}'

%type <type> PRIMITIVE_TYPE
%type <expr_type> EXPRESSION
%type <bexpr_type> SIMPLE_EXPRESSION
%type <stmt_type> STATEMENT
%type <stmt_type> STATEMENT_LIST
%type <stmt_type> IF
%type <stmt_type> WHILE
%type <stmt_type> FOR
%type <intValue> LABEL
%type <intValue> GOTO


%%
METHOD_BODY:
	{
		codeSteps.push_back((".source " + outfileName));
		codeSteps.push_back((".class public test\n.super java/lang/Object\n")); //code for defining class
		codeSteps.push_back((".method public <init>()V"));
		codeSteps.push_back(("aload_0"));
		codeSteps.push_back(("invokenonvirtual java/lang/Object/<init>()V"));
		codeSteps.push_back(("return"));
		codeSteps.push_back((".end method\n"));
		codeSteps.push_back((".method public static main([Ljava/lang/String;)V"));
		codeSteps.push_back((".limit locals 1024\n.limit stack 2048"));
		/* generate temporal vars for syso*/
		setAllowedVariables("print_int",INT_T);
		setAllowedVariables("print_float",FLOAT_T);
	}
	STATEMENT_LIST
	LABEL
	{
		setCodeSteps($2.nextList,$3);
		codeSteps.push_back(("return"));
		codeSteps.push_back((".end method"));
	}
	;
STATEMENT_LIST:
	STATEMENT
	|
	STATEMENT LABEL STATEMENT_LIST
	{
		setCodeSteps($1.nextList,$2);
		$$.nextList = $3.nextList;
	}
	;
LABEL:
{
	$$ = labelCounter;
	codeSteps.push_back((initiateLabel() + ":"));
}
;
STATEMENT:
	DECLARATION {$$.nextList = new vector<int>();}
	|IF {$$.nextList = $1.nextList;}
	|WHILE 	{$$.nextList = $1.nextList;}
	|FOR {$$.nextList = $1.nextList;}
	| ASSIGNMENT {$$.nextList = new vector<int>();}
	| SYSTEMOUT {$$.nextList = new vector<int>();}
	;
DECLARATION:
	PRIMITIVE_TYPE IDENTIFIER ';' /* implement multi-variable DECLARATION */
	{
		string str($2);
		if($1 == INT_T)
		{
			setAllowedVariables(str,INT_T);
		}else if ($1 == FLOAT_T)
		{
			setAllowedVariables(str,FLOAT_T);
		}
	}
	;
PRIMITIVE_TYPE:
	INT_WORD {$$ = INT_T;}
	| FLOAT_WORD {$$ = FLOAT_T;}
	|BOOLEAN_WORD {$$ = BOOL_T;}
	;
GOTO:
{
	$$ = codeSteps.size();
	codeSteps.push_back(("goto "));
}
;
IF:
	IF_WORD '(' SIMPLE_EXPRESSION ')' '{'
	LABEL
	STATEMENT_LIST
	GOTO
	'}'
	ELSE_WORD '{'
	LABEL
	STATEMENT_LIST
	'}'
	{
		setCodeSteps($3.trueList,$6);
		setCodeSteps($3.falseList,$12);
		$$.nextList = getNextStep($7.nextList, $13.nextList);
		$$.nextList->push_back($8);
	}
	;
WHILE:
	LABEL
	WHILE_WORD '(' SIMPLE_EXPRESSION ')' '{'
	LABEL
	STATEMENT_LIST
	'}'
	{
		string s = "Label" + to_string($1);
		codeSteps.push_back(("goto " + s));

		setCodeSteps($8.nextList,$1);
		setCodeSteps($4.trueList,$7);
		$$.nextList = $4.falseList;
	}
	;
FOR:
	FOR_WORD '(' ASSIGNMENT LABEL SIMPLE_EXPRESSION ';' LABEL ASSIGNMENT_')'
	'{'
	GOTO
	LABEL
	STATEMENT_LIST
	GOTO
	'}'
	{
		setCodeSteps($5.trueList,$12);
		vector<int> * v = new vector<int> ();
		v->push_back($11);
		setCodeSteps(v,$4);
		v = new vector<int>();
		v->push_back($14);
		setCodeSteps(v,$7);
		setCodeSteps($13.nextList,$7);
		$$.nextList = $5.falseList;
	}
	;
ASSIGNMENT:
	IDENTIFIER '=' EXPRESSION ';'
	{
		string str($1);
		/* after EXPRESSION finishes, its result should be on top of stack.
		we just store the top of stack to the identifier*/
		if(checkId(str))
		{
			if($3 == symbolTable[str].second)
			{
				if($3 == INT_T)
				{
					codeSteps.push_back(("istore " + to_string(symbolTable[str].first)));
				}else if ($3 == FLOAT_T)
				{
					codeSteps.push_back(("fstore " + to_string(symbolTable[str].first)));
				}
			}
			else
			{
				string err = "identifier: "+str+"and EXPRESSION have different type";
				yyerror(err.c_str());
			}
		}else{
			string err = "identifier: "+str+" isn't declared in this scope";
			yyerror(err.c_str());
		}
	}
	;
ASSIGNMENT_:
IDENTIFIER '=' EXPRESSION
{
	string str($1);
	/* after EXPRESSION finishes, its result should be on top of stack.
	we just store the top of stack to the identifier*/
	if(checkId(str))
	{
		if($3 == symbolTable[str].second)
		{
			if($3 == INT_T)
			{
				codeSteps.push_back(("istore " + to_string(symbolTable[str].first)));
			}else if ($3 == FLOAT_T)
			{
				codeSteps.push_back(("fstore " + to_string(symbolTable[str].first)));
			}
			}
			else
			{
				string err = "identifier: "+str+"and EXPRESSION have different type";
				yyerror(err.c_str());
			}
		}else{
			string err = "identifier: "+str+" isn't declared in this scope";
			yyerror(err.c_str());
		}
	}
	;
EXPRESSION:
	FLOAT 	{$$ = FLOAT_T; codeSteps.push_back(("bipush "+to_string($1)));}
	| INT 	{$$ = INT_T;  codeSteps.push_back(("bipush "+to_string($1)));}
	| EXPRESSION ARITH_OP EXPRESSION	{castGeneralOperations($1, $3, string($2));}
	| IDENTIFIER {
		string str($1);
		if(checkId(str))
		{
			$$ = symbolTable[str].second;
			if(symbolTable[str].second == INT_T)
			{
				codeSteps.push_back(("iload " + to_string(symbolTable[str].first)));
			}else if (symbolTable[str].second == FLOAT_T)
			{
				codeSteps.push_back(("fload " + to_string(symbolTable[str].first)));
			}
		}
		else
		{
			string err = "identifier: "+str+" isn't declared in this scope";
			yyerror(err.c_str());
			$$ = ERROR_T;
		}
	}
	| '(' EXPRESSION ')' {$$ = $2;}
	;
SIMPLE_EXPRESSION:
	BOOL
	{
		if($1)
		{
			/* bool is 'true' */
			$$.trueList = new vector<int> ();
			$$.trueList->push_back(codeSteps.size());
			$$.falseList = new vector<int>();
			codeSteps.push_back(("goto "));
			}else
			{
				$$.trueList = new vector<int> ();
				$$.falseList= new vector<int>();
				$$.falseList->push_back(codeSteps.size());
				codeSteps.push_back(("goto "));
			}
		}
		|SIMPLE_EXPRESSION
		BOOL_OP
		LABEL
		SIMPLE_EXPRESSION
		{
			if(!strcmp($2, "&&"))
			{
				setCodeSteps($1.trueList, $3);
				$$.trueList = $4.trueList;
				$$.falseList = getNextStep($1.falseList,$4.falseList);
			}
			else if (!strcmp($2,"||"))
			{
				setCodeSteps($1.falseList,$3);
				$$.trueList = getNextStep($1.trueList, $4.trueList);
				$$.falseList = $4.falseList;
			}
		}
		| EXPRESSION RELA_OP EXPRESSION
		{
			string op ($2);
			$$.trueList = new vector<int>();
			$$.trueList ->push_back (codeSteps.size());
			$$.falseList = new vector<int>();
			$$.falseList->push_back(codeSteps.size()+1);
			codeSteps.push_back((operators_map[op]+ " "));
			codeSteps.push_back(("goto "));
		}
		/*|EXPRESSION RELA_OP BOOL 	// to be considered */
		;
SYSTEMOUT:
	SYSTEM_OUT '(' EXPRESSION ')' ';'
	{
		if($3 == INT_T)
		{
			/* EXPRESSION is at top of stack now */
			/* save it at the predefined temp syso var */
			codeSteps.push_back(("istore " + to_string(symbolTable["1syso_int_var"].first)));
			/* call syso */
			codeSteps.push_back(("getstatic      java/lang/System/out Ljava/io/PrintStream;"));
			/*insert param*/
			codeSteps.push_back(("iload " + to_string(symbolTable["1syso_int_var"].first )));
			/*invoke syso*/
			codeSteps.push_back(("invokevirtual java/io/PrintStream/println(I)V"));

		}else if ($3 == FLOAT_T)
		{
			codeSteps.push_back(("fstore " + to_string(symbolTable["1syso_float_var"].first)));
			/* call syso */
			codeSteps.push_back(("getstatic      java/lang/System/out Ljava/io/PrintStream;"));
			/*insert param*/
			codeSteps.push_back(("fload " + to_string(symbolTable["1syso_float_var"].first )));
			/*invoke syso*/
			codeSteps.push_back(("invokevirtual java/io/PrintStream/println(F)V"));
		}
	}
	;
%%

main (int argv, char * argc[])
{
	FILE *myfile;
	if(argv > 1)
	{
		myfile = fopen(argc[1], "r");
		outfileName = string(argc[1]);
	}
	else
	{
		printf("Please enter code file!\n");
		return -1;
	}
	if (!myfile) {
		printf("can't open code file!\n");
		return -1;

	}
	yyin = myfile;
	yyparse();
	printCode();
}

void yyerror(const char * s)
{
	printf("ERROR : line %d: %s\n",lineCounter, s);
}

void castGeneralOperations(int from , int to, string op)
{
	if(from == to)
	{
		if(from == INT_T)
		{
			codeSteps.push_back(("i" + operators_map[op]));
		}else if (from == FLOAT_T)
		{
			codeSteps.push_back(("f" + operators_map[op]));
		}
	}
	else
	{
		yyerror("castGeneral not implemented yet");
	}
}

bool checkId(string op)
{
	return (symbolTable.find(op) != symbolTable.end());
}

string initiateLabel()
{
	return "Label" + to_string(labelCounter++);
}

void setAllowedVariables(string name, int type)
{
	if(checkId(name))
	{
		string err = "variable: "+name+" declared before";
		yyerror(err.c_str());
	} else
	{
		if(type == INT_T)
		{
			codeSteps.push_back(("iconst_0\nistore " + to_string(globalVarNum)));
		}
		else if ( type == FLOAT_T)
		{
			codeSteps.push_back(("fconst_0\nfstore " + to_string(globalVarNum)));
		}
		symbolTable[name] = make_pair(globalVarNum++,(type_enum)type);
	}
}

void printCode(void)
{
	for ( int i = 0 ; i < codeSteps.size() ; i++)
	{
		fout<<codeSteps[i]<<endl;
	}
}

vector<int> * getNextStep(vector<int> *list1, vector<int> *list2)
{
	if(list1 && list2){
		vector<int> *list = new vector<int> (*list1);
		list->insert(list->end(), list2->begin(),list2->end());
		return list;
	} else if(list1)
	{
		return list1;
	} else if (list2)
	{
		return list2;
	} else
	{
		return new vector<int>();
	}
}

void setCodeSteps(vector<int> *lists, int ind)
{
	if(lists)
	for(int i =0 ; i < lists->size() ; i++)
	{
		codeSteps[(*lists)[i]] = codeSteps[(*lists)[i]] + "Label"+to_string(ind);
	}
}
