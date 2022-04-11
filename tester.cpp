#include <iostream>
#include <string>
#include "lex.h"
#include <cstring>
#include <fstream>

using namespace std;

int main(int argc,char ** argv)
{
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
            print_II == true;
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
                cout << "ONLY ONE FILE NAME ALLOWED" << endl;
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
        cout << "NO SPECIFIED INPUT FILE NAME FOUND" << endl;
        return EXIT_FAILURE;
    }

    fstream file = fstream(fname);
    if(!file.is_open())
    {
        cout << "CANNOT OPEN THE FILE " << fname << endl;
        return EXIT_FAILURE;
    }
}