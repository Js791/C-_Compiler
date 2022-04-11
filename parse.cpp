/* Definitions and some functions implementations
 * parse.cpp to be completed
 * Programming Assignment 2
 * Spring 2022
 */

#include "parse.h"

map<string, bool> defVar;
map<string, Token> SymTable;

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

bool ProgBody(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != BEGIN)
    {
        ParseError(t.GetLinenum(), "Missing begin statment");
        ;
        return false;
    }
    while (Stmt(in, line))
    {
      
        t = Parser::GetNextToken(in, line);
        if (t.GetToken() != SEMICOL)
        {
            ParseError(line-1, "Missing semicolon in Statement.");
            return false;
        }
    }

    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != END)
    {
        ParseError(t.GetLinenum(), "Syntactic error in Program Body");
        return false;
    }
    return true;
}

bool DeclStmt(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != IDENT)
    {
        Parser::PushBackToken(t);
        return false;
    }
    if (defVar.find(t.GetLexeme()) == defVar.end())
    {
        defVar[t.GetLexeme()] = false;
    }
    else
    {
        ParseError(t.GetLinenum(), "Variable name already defined");
        Parser::PushBackToken(t);
        return false;
    }

    while (true)
    {
        t = Parser::GetNextToken(in, line);
        if (t.GetToken() != COMMA)
        {
            Parser::PushBackToken(t);
            break;
        }
        t = Parser::GetNextToken(in, line);
        if (t.GetToken() != IDENT)
        {
            Parser::PushBackToken(t);
            ParseError(t.GetLinenum(), "Expected identifier after comma");
            return false;
        }
        if (defVar.find(t.GetLexeme()) == defVar.end())
        {
            defVar[t.GetLexeme()] = true;
        }
        else
        {
            ParseError(t.GetLinenum(), "Variable name already defined");
            Parser::PushBackToken(t);
            return false;
        }
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != COLON)
    {
        Parser::PushBackToken(t);
        ParseError(t.GetLinenum(), "Missing colon after identifier list");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != INTEGER && t.GetToken() != REAL && t.GetToken() != STRING)
    {
        Parser::PushBackToken(t);
        ParseError(t.GetLinenum(), "Incorrect Declaration Type.");
        return false;
    }
    return true;
}

bool DeclBlock(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != VAR)
    {
        Parser::PushBackToken(t);
        return false;
    }
    while (DeclStmt(in, line))
    {
        t = Parser::GetNextToken(in, line);
        if (t.GetToken() != SEMICOL)
        {
            ParseError(t.GetLinenum(), "Expected semicolon after declaration");
            return false;
        }
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != BEGIN)
    {
        Parser::PushBackToken(t);
        ParseError(t.GetLinenum(), "Syntactic error in Declaration Block.");
        return false;
    }
    Parser::PushBackToken(t);
    return true;
}

bool Prog(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != PROGRAM)
    {
         ParseError(t.GetLinenum(), "Missing PROGRAM.");;
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != IDENT)
    {

        ParseError(t.GetLinenum(), "Missing Program Name.");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != SEMICOL)
    {
        ParseError(t.GetLinenum(), "Expected semicolon after program name");
        return false;
    }
    if (!DeclBlock(in, line))
    {
        t = Parser::GetNextToken(in, line);
        ParseError(t.GetLinenum(), "Incorrect Declaration Section.");
        return false;
    }
    if (!ProgBody(in, line))
    {
        ParseError(t.GetLinenum(),"Incorrect Program Body.");
        return false;
    }
    return true;
}

bool Factor(istream &in, int &line, int sign)
{
    LexItem t = Parser::GetNextToken(in, line);
    switch (t.GetToken())
    {
    case IDENT:
     Parser::PushBackToken(t);
     return Var(in,line);
    case ICONST:
    case RCONST:
    case SCONST:
        return true;
    case LPAREN:
        if (Expr(in, line))
        {
            t = Parser::GetNextToken(in, line);
            if (t.GetToken() != RPAREN)
            {
                ParseError(t.GetLinenum(), "Expected closing parenthesis.");
                return false;
            }
            return true;
        }
        else
        {
            ParseError(t.GetLinenum(), "Expect expression in parenthesis.");
            return false;
        }
    default:
        Parser::PushBackToken(t);
        return false;
    }
}
bool ForStmt(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != FOR)
    {
        Parser::PushBackToken(t);
        return false;
    }
    if (!Var(in, line))
    {
        ParseError(t.GetLinenum(), "Expected variable declaration in for-loop");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != ASSOP)
    {
        ParseError(t.GetLinenum(), "Expected variable assignemnt in for-loop");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != ICONST)
    {
        ParseError(t.GetLinenum(), "Expected value variable assignemnt in for-loop");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != TO && t.GetToken() != DOWNTO)
    {
        ParseError(t.GetLinenum(), "Expected to or down to");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != ICONST)
    {
        ParseError(t.GetLinenum(), "Expected finish value in for-loop");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != DO)
    {
        ParseError(t.GetLinenum(), "Expected DO");
        return false;
    }
    return Stmt(in, line);
}
bool Var(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != IDENT)
    {
        Parser::PushBackToken(t);
        return false;
    }
    map<string,bool>::iterator var_entry = defVar.find(t.GetLexeme());
    if (var_entry == defVar.end())
    {
         ParseError(t.GetLinenum(), "Undeclared Variable");
         return false;
    }
    /*if(var_entry->second == false)
    {
         ParseError(t.GetLinenum(), "Undefined Variable");
         return false;
    }
    */
    return true;
}
/*bool LVar(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != IDENT)
    {
        Parser::PushBackToken(t);
        return false;
    }
    if (defVar.find(t.GetLexeme()) == defVar.end())
    {
         ParseError(t.GetLinenum(), "Undeclared Variable");
         return false;
    }
    defVar[t.GetLexeme()] = true;
    return true;
}
*/
bool AssignStmt(istream &in, int &line)
{
    if (!Var(in, line))
    {
         ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
         return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != ASSOP)
    {
        ParseError(t.GetLinenum(), "Expected assignment operator");
        return false;
    }
    if (!Expr(in, line))
    {
        ParseError(t.GetLinenum(), "Missing Expression in Assignment Statment");
        return false;
    }
    else
    {
        return true;
    }
}
bool SFactor(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == PLUS)
    {
        return Factor(in, line, 1);
    }
    else if (t.GetToken() == MINUS)
    {
        return Factor(in, line, -1);
    }
    else
    {
        Parser::PushBackToken(t);
        return Factor(in, line, 1);
    }
}
bool Term(istream &in, int &line)
{
    if (SFactor(in, line))
    {
        while (true)
        {
            LexItem t = Parser::GetNextToken(in, line);
            if (t.GetToken() != MULT && t.GetToken() != DIV)
            {
                Parser::PushBackToken(t);
                return true;
            }
            if (!SFactor(in, line))
            {
                ParseError(t.GetLinenum(), "Expected second factor in term expression");
                return false;
            }
        }
    }
    return false;
}
bool Expr(istream &in, int &line)
{

    if (Term(in, line))
    {
        while (true)
        {
            LexItem t = Parser::GetNextToken(in, line);
            if(t.GetToken() == ERR)
            {
                ParseError(line, "Unrecognized Input Pattern\n(" + t.GetLexeme() + ")");
                return false;
            }
            if (t.GetToken() != PLUS && t.GetToken() != MINUS)
            {
                Parser::PushBackToken(t);
                return true;
            }
            if (!Term(in, line))
            {
                ParseError(t.GetLinenum(), "Expected secondary second term in expression");
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    return false;
}
bool LogicExpr(istream &in, int &line)
{
    if (Expr(in, line))
    {
        LexItem t = Parser::GetNextToken(in, line);
        if (t.GetToken() != EQUAL && t.GetToken() != LTHAN && t.GetToken() != GTHAN)
        {
            ParseError(t.GetLinenum(), "Expected comparison operator in logic expression");
            return false;
        }
        if (!Expr(in, line))
        {
            ParseError(t.GetLinenum(), "logic expression should contain two expressions");
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}
bool IfStmt(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != IF)
    {
        Parser::PushBackToken(t);
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != LPAREN)
    {
        ParseError(t.GetLinenum(), "Expected left paren for if stmt");
        return false;
    }
    if (!LogicExpr(in, line))
    {
        ParseError(t.GetLinenum(), "Expected logical condition");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != RPAREN)
    {
        ParseError(t.GetLinenum(), "Expected right paren for if stmt");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != THEN)
    {
        ParseError(t.GetLinenum(), "Expected then for if stmt");
        return false;
    }
    if (!Stmt(in, line))
    {
        ParseError(t.GetLinenum(), "Missing Statement for If-Stmt Then-Part");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != ELSE)
    {
        Parser::PushBackToken(t);
        return true;
    }
    if (!Stmt(in, line))
    {
        ParseError(t.GetLinenum(), "else stmt missing body");
        return false;
    }
    else
    {
        return true;
    }
}

// Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
// Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt
bool Stmt(istream &in, int &line)
{
    bool status;
    // cout << "in ContrlStmt" << endl;
    LexItem t = Parser::GetNextToken(in, line);

    switch (t.GetToken())
    {

    case WRITELN:
        status = WriteLnStmt(in, line);
        // cout << "After WriteStmet status: " << (status? true:false) <<endl;
        break;

    case IF:
        Parser::PushBackToken(t);
        status = IfStmt(in, line);
        break;

    case IDENT:
        Parser::PushBackToken(t);
        status = AssignStmt(in, line);

        break;

    case FOR:
        Parser::PushBackToken(t);
        status = ForStmt(in, line);

        break;

    default:
         Parser::PushBackToken(t);
         return false;
    }

    return status;
} // End of Stmt

// WriteStmt:= wi, ExpreList
bool WriteLnStmt(istream &in, int &line)
{
    LexItem t;
    // cout << "in WriteStmt" << endl;

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
    // Evaluate: print out the list of expressions values

    return ex;
}

// ExprList:= Expr {,Expr}
bool ExprList(istream &in, int &line)
{
    bool status = false;
    // cout << "in ExprList and before calling Expr" << endl;
    status = Expr(in, line);
    if (!status)
    {
        ParseError(line, "Missing Expression");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA)
    {
        // cout << "before calling ExprList" << endl;
        status = ExprList(in, line);
        // cout << "after calling ExprList" << endl;
    }
    else if (tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern\n(" + tok.GetLexeme() + ")");
        return false;
    }
    else
    {
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}
