#pragma once

#include <string>
#include <algorithm>
#include <regex>


typedef enum{DATATYPE, VARNAME, ADDITIVE_OP, RELATIONAL_OP,MULTIPLICATIVE_OP,UNARY_OP,OPEN_PAREN,CLOSE_PAREN,EOL } TokenType;

class Token{
    public:
    TokenType type;
    string value;
    Token(TokenType newType=EOL,string newValue="") {
		type=newType;
		value=newValue;  
    }
};

class Tokenizer{
    string line;
    int pos;
    public:
    int getPosition(){
        return pos;
    }
    void start(string newLine){
        line=newLine;
        line.erase(remove(line.begin(), line.end(), ' '), line.end());//removes whitespace
        pos=0;
    }
    Token peek() {
        smatch sm;
        string remaining=line.substr(pos);
        if (regex_match(remaining,sm,regex("(\\+|-).*"))) 
          return Token(ADDITIVE_OP,sm[1]);
        if (regex_match(remaining,sm,regex("((<=)|(=>)|(<>)|<|=|>).*"))) 
          return Token(RELATIONAL_OP,sm[1]);
        if (regex_match(remaining,sm,regex("(or).*"))) 
          return Token(ADDITIVE_OP,sm[1]);
        if (regex_match(remaining,sm,regex("(\\*|/|div|mod|and).*"))) 
          return Token(MULTIPLICATIVE_OP,sm[1]);
        /*if (regex_match(remaining,sm,regex("(not).*"))) 
          return Token(UNARY_OP,sm[1]);*/
        if (regex_match(remaining,sm,regex("(\\().*"))) 
          return Token(OPEN_PAREN,sm[1]);
        if (regex_match(remaining,sm,regex("(\\)).*"))) 
          return Token(CLOSE_PAREN,sm[1]);
        return Token();
    }
    Token next(){
        Token t;
        t=peek();
        pos+=t.value.size();
        return t;
    }
};