/* Implementation of PLSL Interpreter
 * parseInt.cpp
 * Programming Assignment 3
 * Spring 2022
 */

#include <vector>
#include <string>
#include "parseInt.h"
#include <sstream>
#include "val.h"
//#include "lex.cpp"
using namespace std;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; // Container of temporary locations of Value objects for results of expressions, variables values and constants
queue<Value> *ValQue;			 // declare a pointer variable to a queue of Value objects

bool ParseIfStmt(istream &in, int &line);
bool parseElse(istream &in, int &line);
bool ParseAssignStmt(istream &, int &line);
bool ParseWriteLnStmt(istream &in, int &line);
bool ParseStmt(istream &in, int &line);
bool parseThen(istream &in, int &line);
bool IdentList(istream &in, int &line, vector<string> &IdList);

namespace Parser
{
	bool pushed_back = false;
	LexItem pushed_token;

	static LexItem GetNextToken(istream &in, int &line)
	{
		if (pushed_back)
		{
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem &t)
	{
		if (pushed_back)
		{
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

// WriteStmt:= wi, ExpreList
bool WriteLnStmt(istream &in, int &line)
{
	LexItem t;
	// cout << "in WriteStmt" << endl;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex)
	{
		ParseError(line, "Missing expression after WriteLn");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	// Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;

	return ex;
} // End of WriteLnStmt

// Expr:= Term {(+|-) Term}
bool Expr(istream &in, int &line, Value &retVal)
{
	Value val1, val2;
	// cout << "in Expr" << endl;
	bool t1 = Term(in, line, val1);
	LexItem tok;

	if (!t1)
	{
		return false;
	}
	retVal = val1;

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	// Evaluate: evaluate the expression for addition or subtraction
	while (tok == PLUS || tok == MINUS)
	{
		t1 = Term(in, line, val2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == PLUS)
		{
			retVal = retVal + val2;
			if (retVal.IsErr())
			{
				ParseError(line, "Illegal addition operation.");
				// cout << "(" << tok.GetLexeme() << ")" << endl;
				return false;
			}
		}
		else if (tok == MINUS)
		{
			retVal = retVal - val2;
			if (retVal.IsErr())
			{
				ParseError(line, "Illegal subtraction operation.");
				return false;
			}
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
} // end of Expr
bool Prog(istream &in, int &line)
{
	bool f1, f2;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == PROGRAM)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT)
		{

			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() == SEMICOL)
			{
				f1 = DeclBlock(in, line);

				if (f1)
				{
					f2 = ProgBody(in, line);
					if (!f2)
					{
						ParseError(line, "Incorrect Program Body.");
						return false;
					}

					return true; // Successful Parsing is completed
				}
				else
				{
					ParseError(line, "Incorrect Declaration Section.");
					return false;
				}
			}
			else
			{
				// Parser::PushBackToken(tok);
				ParseError(line - 1, "Missing Semicolon.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if (tok.GetToken() == DONE && tok.GetLinenum() <= 1)
	{
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}
bool ProgBody(istream &in, int &line)
{
	bool status;

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == BEGIN)
	{

		status = Stmt(in, line);

		while (status)
		{
			tok = Parser::GetNextToken(in, line);
			if (tok != SEMICOL)
			{
				line--;
				ParseError(line, "Missing semicolon in Statement.");
				return false;
			}

			status = Stmt(in, line);
		}

		tok = Parser::GetNextToken(in, line);
		if (tok == END)
		{
			return true;
		}
		else
		{
			ParseError(line, "Syntactic error in Program Body.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Program Body.");
		return false;
	}
} // End of ProgBody function
bool DeclBlock(istream &in, int &line)
{
	bool status = false;
	LexItem tok;
	// cout << "in Decl" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	if (t == VAR)
	{
		status = DeclStmt(in, line);

		while (status)
		{
			tok = Parser::GetNextToken(in, line);
			if (tok != SEMICOL)
			{
				line--;
				ParseError(line, "Missing semicolon in Declaration Statement.");
				return false;
			}
			status = DeclStmt(in, line);
		}

		tok = Parser::GetNextToken(in, line);
		if (tok == BEGIN)
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else
		{
			ParseError(line, "Syntactic error in Declaration Block.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Declaration Block.");
		return false;
	}

} // end of DeclBlock function
bool DeclStmt(istream &in, int &line)
{
	LexItem t;
	vector<string> Idlist;
	bool status = IdentList(in, line, Idlist);

	if (!status)
	{
		ParseError(line, "Incorrect variable in Declaration Statement.");
		return status;
	}

	t = Parser::GetNextToken(in, line);
	if (t == COLON)
	{
		t = Parser::GetNextToken(in, line);
		if (t == INTEGER)
		{
			for (string i : Idlist)
			{
				defVar[i] = false;
				TempsResults[i] = Value(0);
				TempsResults[i].SetType(VINT);
			}
			return true;
		}
		else if (t == REAL)
		{
			for (string i : Idlist)
			{
				defVar[i] = false;
				TempsResults[i] = Value(0.0f);
				TempsResults[i].SetType(VREAL);

			}
			return true;
		}
		else if (t == STRING)
		{
			for (string i : Idlist)
			{
				defVar[i] = false;
				TempsResults[i] = Value("");
				TempsResults[i].SetType(VSTRING);
			} 
			return true;
		}
		else
		{
			ParseError(line, "Incorrect Declaration Type.");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(t);
		return false;
	}

} // End of DeclStmt
bool ExprList(istream &in, int &line)
{
	bool status = false;
	Value result;
	status = Expr(in, line, result);
	ValQue->emplace(result);
	if (!status)
	{
		ParseError(line, "Missing Expression");
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA)
	{

		status = ExprList(in, line);
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}
bool LogicExpr(istream &in, int &line, Value &result)
{
	Value result2;
	bool t1 = Expr(in, line, result);
	LexItem tok;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if (tok == GTHAN || tok == EQUAL || tok == LTHAN)
	{
		t1 = Expr(in, line, result2);
		if (tok.GetToken() == GTHAN)
		{
			result = result > result2;
		}
		else if (tok.GetToken() == LTHAN)
		{
			result = result < result2;
		}
		else
		{
			result = result == result2;
		}
		if (!t1)
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
		if (result.IsErr())
		{
			ParseError(line, "Invalid operands for logic expression");
			return false;
		}
		return true;
	}
	Parser::PushBackToken(tok);
	return true;
}
bool Term(istream &in, int &line, Value &result)
{

	Value result2;
	bool t1 = SFactor(in, line, result);
	LexItem tok;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == MULT || tok == DIV)
	{
		t1 = SFactor(in, line, result2);
		if (tok.GetToken() == MULT)
		{
			result = result * result2;
		}
		else
		{
			result = result / result2;
		}
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	if (result.IsErr())
	{
		ParseError(line, "Unable to perform operation");
		return false;
	}
	Parser::PushBackToken(tok);
	return true;
}
bool Factor(istream &in, int &line, int sign, Value &result)
{
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == IDENT)
	{
		string lexeme = tok.GetLexeme();
		if (defVar.find(lexeme) == defVar.end())
		{
			ParseError(line, "Using Undeclared Variable");
			return false;
		}
		if(!defVar[tok.GetLexeme()])
		{
			ParseError(line, "Using Undefined Variable");
			return false;
		}
		result = TempsResults[tok.GetLexeme()];
		if(sign == -1)
		{
			result = result * Value(-1);
		}
		return true;
	}

	else if (tok == ICONST)
	{
		istringstream ss(tok.GetLexeme());
		int value;
		ss >> value;
		result = Value(value);
		if(sign == -1)
		{
			result = result * Value(-1);
		}
		return true;
	}
	else if (tok == SCONST)
	{
		result = Value(tok.GetLexeme());
		return true;
	}
	else if (tok == RCONST)
	{
		istringstream ss(tok.GetLexeme());
		float value;
		ss >> value;
		result = Value(value);
		if(sign == -1)
		{
			result = result * Value(-1);
		}
		return true;
	}
	else if (tok == LPAREN)
	{
		bool ex = Expr(in, line, result);
		if (!ex)
		{
			ParseError(line, "Missing expression after (");
			return false;
		}
		if (Parser::GetNextToken(in, line) == RPAREN)
			return ex;
		else
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}
bool SFactor(istream &in, int &line, Value &result)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	bool isnum = false;
	int sign = 0;
	if (t == MINUS)
	{
		sign = -1;
		isnum = true;
	}
	else if (t == PLUS)
	{
		sign = 1;
		isnum = true;
	}
	else
		Parser::PushBackToken(t);

	status = Factor(in, line, sign, result);
	if(isnum && !(result.IsInt() || result.IsReal()))
	{
		ParseError(line,"Illegal Operand Type for Sign Operator");
		return false;
	}
	return status;
}
bool Var(istream &in, int &line, LexItem &idtok)
{
	// called only from the AssignStmt function
	string identstr;

	idtok = Parser::GetNextToken(in, line);

	if (idtok.GetToken() == IDENT)
	{
		identstr = idtok.GetLexeme();

		if (defVar.find(identstr) == defVar.end())
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
		return true;
	}
	else if (idtok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << idtok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
} // End of Var
bool AssignStmt(istream &in, int &line)
{

	bool varstatus = false, status = false;
	LexItem t;
	Value result;
	string lex;

	varstatus = Var(in, line, t);
	lex = t.GetLexeme();

	if (varstatus)
	{
		t = Parser::GetNextToken(in, line);

		if (t == ASSOP)
		{
			status = Expr(in, line, result);
			if (!status)
			{
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
		}
		else if (t.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else
		{
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else
	{
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	if (result.IsErr()|| (result.GetType() != TempsResults[lex].GetType() && !((result.GetType() == VREAL && TempsResults[lex].GetType() == VINT)||(result.GetType() == VINT && TempsResults[lex].GetType() == VREAL))))
	{
		ParseError(line, "Error mismatch types");
		return false;
	}
	if(TempsResults[lex].IsReal())
	{
		TempsResults[lex] = result * Value(1.00f);
	}
	else if(TempsResults[lex].IsInt() && result.IsReal())
	{
		TempsResults[lex] = Value((int)result.GetReal());
	}
	else
	{
		TempsResults[lex] = result;
	}
	defVar[lex] = true;
	return true;
}
bool ParseAssignStmt(istream &in, int &line)
{

	bool varstatus = false, status = false;
	LexItem t;
	Value result;

	varstatus = Var(in, line, t);

	if (varstatus)
	{
		t = Parser::GetNextToken(in, line);

		if (t == ASSOP)
		{
			status = Expr(in, line, result);
			if (!status)
			{
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
		}
		else if (t.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else
		{
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else
	{
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;
}
bool IdentList(istream &in, int &line, vector<string> &IdList)
{
	bool status = false;
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == IDENT)
	{
		// set IDENT lexeme to the type tok value
		identstr = tok.GetLexeme();
		if (defVar.find(identstr) == defVar.end())
		{
			IdList.push_back(identstr);
		}
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}

	tok = Parser::GetNextToken(in, line);

	if (tok == COMMA)
	{
		status = IdentList(in, line, IdList);
	}
	else if (tok == COLON)
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return status;

} // End of IdentList
bool ParseWriteLnStmt(istream &in, int &line)
{
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex)
	{
		ParseError(line, "Missing expression after WriteLn");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	return ex;
} // End of WriteLnStmt
bool ParseStmt(istream &in, int &line)
{
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken())
	{

	case WRITELN:
		status = ParseWriteLnStmt(in, line);

		break;

	case IF:
		status = ParseIfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = ParseAssignStmt(in, line);

		break;
	default:
		Parser::PushBackToken(t);
		return false;
	}

	return status;
} // End of Stmt
bool Stmt(istream &in, int &line)
{
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken())
	{

	case WRITELN:
		status = WriteLnStmt(in, line);

		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);

		break;
	default:
		Parser::PushBackToken(t);
		return false;
	}

	return status;
} // End of Stmt
bool ParseIfStmt(istream &in, int &line)
{
	bool ex = false;
	LexItem t;
	Value result;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	ex = LogicExpr(in, line, result);
	if (!ex)
	{
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
	if (result.IsErr() || !result.IsBool())
	{
		ParseError(line, "Illegal logic expression.");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	if (!parseThen(in, line))
	{
		return false;
	}
	if (!parseElse(in, line))
	{
		return false;
	}
	return true;

} // End of IfStmt function
bool parseElse(istream &in, int &line)
{
	bool status;
	LexItem t;
	t = Parser::GetNextToken(in, line);
	if (t == ELSE)
	{
		status = ParseStmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		// cout << "in IFStmt status of Stmt true" << endl;
		return true;
	}

	Parser::PushBackToken(t);
	return true;
}
bool parseThen(istream &in, int &line)
{
	bool status;
	LexItem t;

	t = Parser::GetNextToken(in, line);
	if (t != THEN)
	{
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}
	status = ParseStmt(in, line);
	if (!status)
	{
		ParseError(line, "Missing Statement for If-Stmt Then-Part");
		return false;
	}
	return true;
}
bool IfStmt(istream &in, int &line)
{
	bool ex = false, status;
	LexItem t;
	Value result;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	ex = LogicExpr(in, line, result);
	if (!ex)
	{
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
	if (result.IsErr() || !result.IsBool())
	{
		ParseError(line, "Illegal logic expression.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	if (result.IsErr() || !result.IsBool())
	{
		ParseError(line, "Semantic error in if-stmt logic");
		return false;
	}
	if (result.GetBool())
	{
		t = Parser::GetNextToken(in, line);
		if (t != THEN)
		{
			ParseError(line, "If-Stmt Syntax Error");
			return false;
		}
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}
		if (!parseElse(in, line))
		{
			return false;
		}
		return true;
	}
	else
	{
		if (!parseThen(in, line))
		{
			return false;
		}
		t = Parser::GetNextToken(in, line);
		if (t == ELSE)
		{
			status = Stmt(in, line);
			if (!status)
			{
				ParseError(line, "Missing Statement for If-Stmt Else-Part");
				return false;
			}
			// cout << "in IFStmt status of Stmt true" << endl;
			return true;
		}
		Parser::PushBackToken(t);
		return true;
	}
} // End of IfStmt function
