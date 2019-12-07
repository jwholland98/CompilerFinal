#pragma once

#include <string>
#include <algorithm>
#include <regex>

using namespace std;

typedef enum{IF, ELSE, QUOTS, COUT, CIN, OSTREAM, ISTREAM, ENDL, USING, INCLUDE, FORLOOP, WHILELOOP, PLUSMINUS_OP, UNSIGNED_INT, UNSIGNED_REAL, DATATYPE, VARNAME, ADDITIVE_OP, RELATIONAL_OP,
             MULTIPLICATIVE_OP,UNARY_OP,OPEN_PAREN,CLOSE_PAREN, OPEN_BRACKET, CLOSE_BRACKET,EOL,EQUAL, SEMICOLON} TokenType;

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

    Token peekCopy(){
        int newPos = pos;
        smatch sm;
        string remainingCopy=line.substr(newPos);

        if (regex_match(remaining,sm,regex("(\\+|-).*")))
          return Token(PLUSMINUS_OP,sm[1]);
        if (regex_match(remaining,sm,regex("((<<)).*")))
          return Token(OSTREAM,sm[1]);
        if (regex_match(remaining,sm,regex("((>>)).*")))
          return Token(ISTREAM,sm[1]);
        if (regex_match(remaining,sm,regex("((<=)|(=>)|(==)|(<>)|<|>).*")))
          return Token(RELATIONAL_OP,sm[1]);
        if (regex_match(remaining,sm,regex("(=).*")))
          return Token(EQUAL,sm[1]);
        if (regex_match(remainingCopy,sm,regex("(if).*")))
          return Token(IF,sm[1]);
        if (regex_match(remainingCopy,sm,regex("(else).*")))
          return Token(ELSE,sm[1]);
        if (regex_match(remaining,sm,regex("(;).*")))
          return Token(SEMICOLON,sm[1]);
        if (regex_match(remaining,sm,regex("(\\|\\|).*")))
          return Token(ADDITIVE_OP,sm[1]);
        if (regex_match(remaining,sm,regex("(#include).*")))
          return Token(INCLUDE,sm[1]);
        if (regex_match(remaining,sm,regex("(usingnamespace).*")))
          return Token(USING,sm[1]);
        if(regex_match(remaining,sm,regex("(cin).*")))
          return Token(CIN,sm[1]);
        if (regex_match(remaining,sm,regex("(cout).*")))
          return Token(COUT,sm[1]);
        if (regex_match(remaining,sm,regex("(endl).*")))
          return Token(ENDL,sm[1]);
        if (regex_match(remaining,sm,regex("(\\*|/|%|&&).*")))
          return Token(MULTIPLICATIVE_OP,sm[1]);
        if (regex_match(remaining,sm,regex("(for).*")))
          return Token(FORLOOP,sm[1]);
        if (regex_match(remaining,sm,regex("(while).*")))
          return Token(WHILELOOP,sm[1]);
        if (regex_match(remaining,sm,regex("((bool)|(char)[(16_t)(32_t)]?|(int)|(long)|(signed)|(unsigned)|(float)|(double)|(auto)).*")))
          return Token(DATATYPE,sm[1]);
        if (regex_match(remaining,sm,regex("(not).*")))
          return Token(UNARY_OP,sm[1]);
        if (regex_match(remaining,sm,regex("(\\().*")))
          return Token(OPEN_PAREN,sm[1]);
        if (regex_match(remaining,sm,regex("(\\)).*")))
          return Token(CLOSE_PAREN,sm[1]);
        if (regex_match(remaining,sm,regex("(\").*")))
          return Token(QUOTS,sm[1]);
        if (regex_match(remaining,sm,regex("(\\{).*")))
          return Token(OPEN_BRACKET,sm[1]);
        if (regex_match(remaining,sm,regex("(\\}).*")))
          return Token(CLOSE_BRACKET,sm[1]);
        if (regex_match(remainingCopy,sm,regex("([_$[a-zA-z]+[_$\\w]*).*")))
          return Token(VARNAME,sm[1]);
        if (regex_match(remainingCopy,sm,regex("([0-9]+).*")))
          return Token(UNSIGNED_INT,sm[1]);
        if (regex_match(remaining,sm,regex("([0-9]*/.[0-9]+).*")))
          return Token(UNSIGNED_REAL,sm[1]);
        return Token();
  }
  
  /* Creates a copy of the current remaining string and assigns to a token, which is then incremented to next type
     If passed in type matches the copied and incremented type, returns true.*/
  bool isPeeked(Token t){
        Token tCopy;
        tCopy=peekCopy();
        if(t.type == tCopy.type)
          return true;
        else return false;
  }

  Token peek() {
      smatch sm;
      string remaining=line.substr(pos);
      cout << remaining << endl;

      if (regex_match(remaining,sm,regex("(\\+|-).*"))) 
        return Token(PLUSMINUS_OP,sm[1]);
      if (regex_match(remaining,sm,regex("((<<)).*"))) 
        return Token(OSTREAM,sm[1]);
      if (regex_match(remaining,sm,regex("((>>)).*"))) 
        return Token(ISTREAM,sm[1]);
      if (regex_match(remaining,sm,regex("((<=)|(=>)|(==)|(<>)|<|>).*"))) 
        return Token(RELATIONAL_OP,sm[1]);
      if (regex_match(remaining,sm,regex("(=).*"))) 
        return Token(EQUAL,sm[1]);
      if (regex_match(remaining,sm,regex("(if).*")))
        return Token(IF,sm[1]);
      if (regex_match(remaining,sm,regex("(else).*")))
        return Token(ELSE,sm[1]);
      if (regex_match(remaining,sm,regex("(;).*"))) 
        return Token(SEMICOLON,sm[1]);
      if (regex_match(remaining,sm,regex("(\\|\\|).*"))) 
        return Token(ADDITIVE_OP,sm[1]);
      if (regex_match(remaining,sm,regex("(#include).*"))) 
        return Token(INCLUDE,sm[1]);
      if (regex_match(remaining,sm,regex("(usingnamespace).*"))) 
        return Token(USING,sm[1]);
      if (regex_match(remaining,sm,regex("(cout).*"))) 
        return Token(COUT,sm[1]);
      if (regex_match(remaining,sm,regex("(endl).*"))) 
        return Token(ENDL,sm[1]);
      if (regex_match(remaining,sm,regex("(\\*|/|%|&&).*"))) 
        return Token(MULTIPLICATIVE_OP,sm[1]);
      if (regex_match(remaining,sm,regex("(for).*"))) 
        return Token(FORLOOP,sm[1]);
      if (regex_match(remaining,sm,regex("(while).*"))) 
        return Token(WHILELOOP,sm[1]);
      if (regex_match(remaining,sm,regex("((bool)|(char)[(16_t)(32_t)]?|(int)|(long)|(signed)|(unsigned)|(float)|(double)|(auto)).*"))) 
        return Token(DATATYPE,sm[1]);
      if (regex_match(remaining,sm,regex("(not).*"))) 
        return Token(UNARY_OP,sm[1]);
      if (regex_match(remaining,sm,regex("(\\().*"))) 
        return Token(OPEN_PAREN,sm[1]);
      if (regex_match(remaining,sm,regex("(\\)).*"))) 
        return Token(CLOSE_PAREN,sm[1]);
      if (regex_match(remaining,sm,regex("(\").*"))) 
        return Token(QUOTS,sm[1]);
      if (regex_match(remaining,sm,regex("(\\{).*"))) 
        return Token(OPEN_BRACKET,sm[1]);
      if (regex_match(remaining,sm,regex("(\\}).*"))) 
        return Token(CLOSE_BRACKET,sm[1]);
      if (regex_match(remaining,sm,regex("([_$[a-zA-z]+[_$\\w]*).*")))
        return Token(VARNAME,sm[1]);
      if (regex_match(remaining,sm,regex("([0-9]+).*")))
        return Token(UNSIGNED_INT,sm[1]);
      if (regex_match(remaining,sm,regex("([0-9]*/.[0-9]+).*"))) 
        return Token(UNSIGNED_REAL,sm[1]);
      return Token();
    }
    Token next(){
        Token t;
        t=peek();
        pos+=t.value.size();
        return t;
    }
};
