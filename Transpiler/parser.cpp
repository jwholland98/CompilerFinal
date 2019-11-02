#pragma once

#include "tokenTokenizer.cpp"
#include "expressionTree.cpp"
#include <string>



class Parser{
    Tokenizer tokenizer;
    string error;
    public:
    //here is where the recursion happens
    ExpressionTree scan(string s){
        ExpressionTree tree; // Empty Tree Really
        tokenizer.start(s);
        /*if (expression(tree)) {
            return tree;
        } 
        else {
			cerr << error << ":" << tokenizer.getPosition() << endl;
        }*/
        return ExpressionTree();
    }
};