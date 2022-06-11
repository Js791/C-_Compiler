# C++ Small Compiler

A Small Compiler along with a Interperter created in C++

This Compiler will be composed of Lexical Analyzer,Interperter, and Parser for a simple programming language similar to PASCAL.

Note:
A series of changes has been applied to this project satisfy grading software for this course project, certain special cases have been handled in parse.cpp due to instructor's requirements for parsing section of course project. Intially, this project started at writing code for the lexical analyzer, where a directed graph was used along with finite state automaton concept to determine a token state, tokenization was required to successfully create lexemes, where each lexeme was determined through state programming with the concept mentioned earlier. After the token's state was determined and validated, obtaining the semantics behind each token was neccessary. When each token's state was determined, analyzing the token's semantic was done by calling a serious of functions to simulatenously check for errors, if not errors are found the token's smenatics were identified and validated. Lastly, a interperter was implemented as a optional yet an attempt to fuse all the componets of the project. Where a similar idea was implemented from parse.cpp, yet errors would be checked at runtime.

-Lexer.cpp, parse.cpp, parseInt.cpp are files that are the lexical analyzer,syntax analyzer, and a interperter componets of this small project. <br>
-tester.cpp,prog2.cpp,prog3.cpp are tester driver programs, that test the various implementations of Lexer.cpp,parse.cpp,parseInt.cpp. <br>
-Lex.H is the header file for Lexer.cpp and for any other files that require to utilize the code written in Lex.h <br>
-parse.h is the header file for parse.cpp and for any other files that require to utilize the code written in parse.h <br>
-parseInt.h is the header file for parseInt.cpp and for any other files that require to utilize the code written in parseInt.h <br>
