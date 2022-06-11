#include <iostream>
#include <string>
#include "lex.h"
#include <cstring>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

int main(int argc,char ** argv)
{
    int linenum = 1;
    bool verbose = false;
    bool print_I = false;
    bool print_S = false;
    bool print_R = false;
    bool print_II = false;

    char* fname = nullptr;
    for(int i = 1;i<argc;i++)
    {
        if(strcmp(argv[i],"-v") == 0)
        {
            verbose = true;
        }
        else if (strcmp(argv[i],"-iconst") == 0)
        {
            print_I = true;
        }
        else if (strcmp(argv[i],"-sconst") == 0)
        {
            print_S = true;
        }
        else if(strcmp(argv[i],"-rconst") == 0)
        {
            print_R = true;
        }
        else if(strcmp(argv[i],"-ident") == 0)
        {
            print_II = true;
        }
        else if(argv[i][0] == '-')
        {
            cout << "UNRECOGNIZED FLAG " << argv[i] << endl;
            return EXIT_FAILURE;
        }
        else
        {
            if(fname != nullptr)
            {
                cout << "ONLY ONE FILE NAME ALLOWED." << endl;
                return EXIT_FAILURE;
            }
            else
            {
                fname = argv[i];
            }
        }
        
    }
    if(fname == nullptr)
    {
        cout << "NO SPECIFIED INPUT FILE NAME." << endl;
        return EXIT_FAILURE;
    }
    fstream file = fstream(fname);
    file.open(fname, ios::in);
    file.unsetf(ios_base::skipws);
    if(!file.is_open())
    {
        cout << "CANNOT OPEN the File " << fname << endl;
        return EXIT_FAILURE;
    }
    vector<LexItem> tkns;
    LexItem curr_tkn = getNextToken(file,linenum);
    while(curr_tkn.GetToken() != ERR && curr_tkn.GetToken() != DONE)
    {
        tkns.push_back(curr_tkn);
        if(verbose)
        {
            cout << curr_tkn << endl;
        }
        curr_tkn = getNextToken(file,linenum);
    }
    
    if(curr_tkn.GetToken() == ERR)
    {
        if(curr_tkn.GetLexeme()[0] == '(' && curr_tkn.GetLexeme()[1] == '*')
        {    
            cout << endl;
            cout << "Missing a comment end delimiters '*)' at line " << (linenum-1) << endl;
        }
        else
        {
            cout <<"Error in line " << curr_tkn.GetLinenum() << " (" << curr_tkn.GetLexeme() << ")" << endl;
            return EXIT_FAILURE;
        }
       
    }
 
        if(linenum<1) cout << "Lines: " << 0 << endl;
        else
        {
            cout << "Lines: " << (linenum-1) << endl;
        }
    
        if(tkns.size() !=0) cout << "Tokens: " << tkns.size() << endl;
    
    if(print_S)
    {
        vector<string>sconts;
        for(LexItem t:tkns)
        {
            if(t.GetToken() == SCONST)
            {
                sconts.push_back(t.GetLexeme());
            }
        }
        sort(sconts.begin(),sconts.end());
        cout << "STRINGS:" << endl;
        for(string tk:sconts)
        {
            cout << tk << endl;
        }
    }
    if(print_I)
    {
        vector<int>sconts;
        for(LexItem t:tkns)
        {
            if(t.GetToken() == ICONST)
            {
                sconts.push_back(stoi(t.GetLexeme()));
            }
        }
        sort(sconts.begin(),sconts.end());
        cout << "INTEGERS:" << endl;
        for(int i =0;i<sconts.size();i++)
        {
            if(i==0 || sconts[i] != sconts[i-1])
            {
               cout << sconts[i] << endl;
            }
            
        }
        
    }
    if(print_R)
    {
        vector<float>sconts;
        for(LexItem t:tkns)
        {
            if(t.GetToken() == RCONST)
            {
                sconts.push_back(stof(t.GetLexeme()));
            }
        }
        sort(sconts.begin(),sconts.end());
        cout << "REALS:" << endl;
        for(int i =0;i<sconts.size();i++)
        {
            if(i==0 || sconts[i] != sconts[i-1])
            {
               cout << sconts[i] << endl;
            }
            
        }
    }
    if(print_II)
    {
        vector<string>sconts;
        for(LexItem t:tkns)
        {
            if(t.GetToken() == IDENT)
            {
                string tkn = t.GetLexeme();
                for(int i = 0;i<tkn.size();i++)
                {
                    tkn[i] = toupper(tkn[i]);
                }    
                sconts.push_back(tkn);
            }
        }
        sort(sconts.begin(),sconts.end());
        cout << "IDENTIFIERS:" << endl;
        string rtn = "";
        for(int i = 0;i<sconts.size();i++)
        {
            if(i==0 || sconts[i].compare(sconts[i-1]))
            {
                rtn+=sconts[i] + ", " ;
            }
        }
        if(rtn.size() > 0)
        {
            rtn = rtn.substr(0,rtn.size()-2);
        }
        cout << rtn << endl;
    }
}
