#include "lex.h"
#include <string>
#include <iostream>
#include <map>

using namespace std;

enum lex_state {IDENT_state,LETTER_state,ICONST_state,INVALIDCONST_state,RCONST_state,START_state,SCONST_state,DOT_state,COL_state,COM_state,PAREN_state};

LexItem getNextToken (istream& in, int& linenumber)
{
    string cur_lex = "";
    char cur_char;
    lex_state state = START_state;
    while(true)
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
                return LexItem(GTHAN,"/",linenumber);
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
                if(cur_lex == "program")
                {
                    return LexItem(PROGRAM,cur_lex,linenumber);
                }
                else if (cur_lex == "begin")
                {
                    return LexItem(BEGIN,cur_lex,linenumber);
                }
                else if (cur_lex == "end")
                {
                    return LexItem(END,cur_lex,linenumber);
                }
                else if (cur_lex == "writeLn")
                {
                    return LexItem(WRITELN,cur_lex,linenumber);
                }
                else if (cur_lex == "if")
                {
                    return LexItem(IF,cur_lex,linenumber);
                }
                else if (cur_lex == "else")
                {
                    return LexItem(ELSE,cur_lex,linenumber);
                }
                else if (cur_lex == "integer")
                {
                    return LexItem(INTEGER,cur_lex,linenumber);
                }
                
                else if (cur_lex == "real")
                {
                    return LexItem(REAL,cur_lex,linenumber);
                }
                else if (cur_lex == "string")
                {
                    return LexItem(STRING,cur_lex,linenumber);
                }
                else if (cur_lex == "for")
                {
                    return LexItem(FOR,cur_lex,linenumber);
                }
                else if (cur_lex == "to")
                {
                    return LexItem(TO,cur_lex,linenumber);
                }
                else if (cur_lex == "downto")
                {
                    return LexItem(DOWNTO,cur_lex,linenumber);
                }
                else if (cur_lex == "var")
                {
                    return LexItem(VAR,cur_lex,linenumber);
                }

                else
                {
                   return LexItem(IDENT,cur_lex,linenumber);
                }
                
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
             else
            {
                return LexItem(RCONST,cur_lex,linenumber);
            }
            break;

            case SCONST_state:
            do
            {
                in >> cur_char;
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
                return LexItem(DOT,cur_lex,linenumber);
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
                char prev = ';';
                do
                {
                prev = cur_char;
                in >> cur_char;
                cur_lex+=cur_char;
                }
                while(cur_char != ')' && cur_char != '*');

                cur_lex = "";
                state = START_state;
            }
           
            break;

            case PAREN_state:
            cur_char = in.peek();
            if(cur_char == '*')
            {
                in >> cur_char;
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
}