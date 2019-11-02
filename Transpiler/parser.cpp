#pragma once

#include "tokenTokenizer.cpp"
#include "expressionTree.cpp"
#include <string>

using namespace std;

class Parser{
    Tokenizer tokenizer;
    string error;
    public:
    //here is where the recursion happens

    bool declaration(ExpressionTree &tree){
        cout << "hi";
        return true;
    }

    ExpressionTree scan(string s){
        ExpressionTree tree; // Empty Tree Really
        tokenizer.start(s);
        if (declaration(tree)) {
            return tree;
        } 
        else {
			cerr << error << ":" << tokenizer.getPosition() << endl;
        }
        return ExpressionTree();
    }
};