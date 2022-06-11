#include "lex.h"
#include <string>
#include <iostream>
#include <map>
#include <cstring>
#include <fstream>

using namespace std;

enum lex_state {IDENT_state,LETTER_state,ICONST_state,INVALIDCONST_state,RCONST_state,START_state,SCONST_state,DOT_state,COL_state,COM_state,PAREN_state};

LexItem getNextToken (istream& in, int& linenumber)
{
    string cur_lex = "";
    char cur_char;
    lex_state state = START_state;
    while(!in.eof() && in.peek() != std::ifstream::traits_type::eof())
    {
        switch (state)
        {
            case START_state:
            in >> cur_char;
            if(isalpha(cur_char))
            {
                cur_lex += cur_char;
                state = LETTER_state;
            }
            else if(isdigit(cur_char))
            {
                cur_lex+=cur_char;
                state = ICONST_state;
            }
            else if (cur_char == '_')
            {
                cur_lex += cur_char;
                state = IDENT_state;
            }
            else if (cur_char == '.')
            {
                cur_lex+=cur_char;
                state = DOT_state;
            }
            else if (cur_char == ':')
            {
                cur_lex+=cur_char;
                state = COL_state;
            }
            else if(cur_char == '(')
            {
                cur_lex+=cur_char;
                state = PAREN_state;
            }
            else if (cur_char == '\'')
            {
                cur_lex+=cur_char;
                state = SCONST_state;
            }
             else if (cur_char == '*')
            {
                return LexItem(MULT,"*",linenumber);
            }
            else if(cur_char == '+')
            {
                return LexItem(PLUS,"+",linenumber);
            }
            else if (cur_char == '-')
            {
                return LexItem(MINUS,"-",linenumber);
            }
            else if (cur_char == '/')
            {
                return LexItem(DIV,"/",linenumber);
            }
            else if (cur_char == ')')
            {
                return LexItem(RPAREN,")",linenumber);
            }
            else if (cur_char == ';')
            {
                return LexItem(SEMICOL,";",linenumber);
            }
            else if (cur_char == ',')
            {
                return LexItem(COMMA,",",linenumber);
            }
            else if (cur_char == '=')
            {
                return LexItem(EQUAL,"=",linenumber);
            }
            else if (cur_char == '<')
            {
                return LexItem(LTHAN,"<",linenumber);
            }
            else if (cur_char == '>')
            {
                return LexItem(GTHAN,">",linenumber);
            }
            else if (cur_char == '\n')
            {
                linenumber++;
            }
            else if (isspace(cur_char))
            {
                //space
            }
            else
            {
                
                return LexItem(ERR,cur_lex+cur_char,linenumber);
            }
            break;

            case IDENT_state:
            cur_char = in.peek();
            if(isalnum(cur_char) || cur_char == '_')
            {
                in >> cur_char;
                cur_lex+=cur_char;
            }
            else
            {
                return LexItem(IDENT,cur_lex,linenumber);
            }

            break;
            
            case LETTER_state:
            cur_char = in.peek();
            if(isalpha(cur_char))
            {
                in >> cur_char;
                cur_lex+=cur_char;
            }

            else if(isdigit(cur_char) || cur_char == '_')
            {
                in >> cur_char;
                cur_lex+=cur_char;
                state = IDENT_state;
            }
            
            else
            {
               return id_or_kw(cur_lex,linenumber);
            }
            break;

            case ICONST_state:
            cur_char = in.peek();
            if(isdigit(cur_char))
            {
                in >> cur_char;
                cur_lex+=cur_char;
            }

            else if(cur_char == '.')
            {
                in >> cur_char;
                cur_lex+=cur_char;
                state = INVALIDCONST_state;
            }
            else
            {
                return LexItem(ICONST,cur_lex,linenumber);
            }
            break;

            case INVALIDCONST_state:
            cur_char = in.peek();
            if(isdigit(cur_char))
            {
                in >> cur_char;
                cur_lex+=cur_char;
                state = RCONST_state;
            }

            else
            {
                return LexItem(ERR,cur_lex,linenumber);
            }
            break;

            case RCONST_state:
            cur_char = in.peek();
            if(isdigit(cur_char))
            {
                in >> cur_char;
                cur_lex+=cur_char;
            }
            else if(cur_char == '.')
            {
                cur_lex += cur_char;
                return LexItem(ERR,cur_lex,linenumber);
            }    
              
            else
            {
                return LexItem(RCONST,cur_lex,linenumber);
            }
            break;

            case SCONST_state:
            do
            {
                in >> cur_char;
                if(cur_char == '\n') return LexItem(ERR,cur_lex,linenumber);
                cur_lex+=cur_char;
            }
            while(cur_char != '\'');
            return LexItem(SCONST,cur_lex,linenumber);
            break;

            case DOT_state:
            cur_char = in.peek();
            if(isdigit(cur_char))
            {
                in >> cur_char;
                cur_lex+=cur_char;
                state = RCONST_state;
            }

            else
            {
                return LexItem(ERR,cur_lex,linenumber);
            }
            break;

            case COL_state:
            cur_char = in.peek();
            if(cur_char == '=')
            {
                in >> cur_char;
                cur_lex+=cur_char;
                return LexItem(ASSOP,cur_lex,linenumber);
            }
            else
            {
                 return LexItem(COLON,cur_lex,linenumber);
            }
            break;

            case COM_state:
            {
                char prev;
                do
                {
                prev = cur_char;
                in >> cur_char;
                if(cur_char == '\n') linenumber++;
                cur_lex+=cur_char;
                }
                while(cur_char != ')' && prev != '*' && !in.eof() && in.peek() != std::ifstream::traits_type::eof());
                
                if(in.eof() || in.peek() == std::ifstream::traits_type::eof()) return LexItem(ERR,cur_lex,linenumber);
                cur_lex = "";
                state = START_state;
            }
           
            break;

            case PAREN_state:
            cur_char = in.peek();
            if(cur_char == '*')
            {
                in >> cur_char;
                cur_lex+=cur_char;
                cur_char = ' ';
                state = COM_state;
            }
            else
            {
                return LexItem(LPAREN,cur_lex,linenumber);
            }
            break;
        }

    }
    return LexItem(DONE,"",linenumber);
    
}
LexItem id_or_kw(const string& lexeme, int linenumber)
{   
                const string &cur_lex = lexeme;
                if(strcasecmp(cur_lex.c_str(),"PROGRAM")==0)
                {
                    return LexItem(PROGRAM,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"begin")==0)
                {
                    return LexItem(BEGIN,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"END")==0)
                {
                    return LexItem(END,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"WRITELN")==0)
                {
                    return LexItem(WRITELN,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"IF")==0)
                {
                    return LexItem(IF,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"ELSE")==0)
                {
                    return LexItem(ELSE,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"integer")==0)
                {
                    return LexItem(INTEGER,cur_lex,linenumber);
                }
                
                else if (strcasecmp(cur_lex.c_str(),"real")==0)
                {
                    return LexItem(REAL,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"string")==0)
                {
                    return LexItem(STRING,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"for")==0)
                {
                    return LexItem(FOR,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"to")==0)
                {
                    return LexItem(TO,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"DOWNTO")==0)
                {
                    return LexItem(DOWNTO,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"VAR")==0)
                {
                    return LexItem(VAR,cur_lex,linenumber);
                }
                else if (strcasecmp(cur_lex.c_str(),"do")==0)
                {
                    return LexItem(DO,cur_lex,linenumber);
                }
                else
                {
                   return LexItem(IDENT,cur_lex,linenumber);
                }
}

ostream& operator<<(ostream& out, const LexItem& tok)
{
    string lex = tok.GetLexeme();
    for(int i =0;i<lex.size();i++)
    {
        lex[i] = toupper(lex[i]);
    }
    
    switch(tok.GetToken())
    {
        case IDENT:
        out << "IDENT" << "(" << lex << ")";
        break;
        case ICONST:
        out << "ICONST" << "(" << lex << ")";
        break;
        case RCONST:
        out << "RCONST" << "(" << lex << ")";
        break;
        case SCONST:
        out << "SCONST" << "(" << tok.GetLexeme().substr(1,lex.size()-2) << ")";
        break;
        case SEMICOL:
        out << "SEMICOL";
        break;
        case EQUAL:
        out << "EQUAL";
        break;
        case PLUS:
        out << "PLUS";
        break;
        case MINUS:
        out << "MINUS";
        break;
        case MULT:
        out << "MULT";
        break;
        case DIV:
        out << "DIV";
        break;
        case ASSOP:
        out << "ASSOP";
        break;
        case LPAREN:
        out << "LPAREN";
        break;
        case RPAREN:
        out << "RPAREN";
        break;
        case GTHAN:
        out << "GTHAN";
        break;
        case LTHAN:
        out << "LTHAN";
        break;
        case COLON:
        out << "COLON";
        break;
        case COMMA:
        out << "COMMA";
        break;
        case PROGRAM:
        out << "PROGRAM";
        break;
        case END:
        out << "END";
        break;
        case BEGIN:
        out << "BEGIN";
        break;
        case WRITELN:
        out << "WRITELN";
        break;
        case IF:
        out << "IF";
        break;
        case ELSE:
        out << "ELSE";
        break;
        case INTEGER:
        out << "INTEGER";
        break;
        case REAL:
        out << "REAL";
        break;
        case STRING:
        out << "STRING";
        break;
        case FOR:
        out << "FOR";
        break;
        case TO:
        out << "TO";
        break;
        case DOWNTO:
        out << "DOWNTO";
        break;
        case VAR:
        out << "VAR";
        break;
        case DO:
        out << "DO";
        break;
        default:
         out << lex;
        break;
    }

    return out;
}
