#pragma once

#include "tokenTokenizer.cpp"
#include <iostream>


class ExpressionTree{
  public:
    ExpressionTree *left,*right;
    Token operation;
    ExpressionTree(Token t=Token(),ExpressionTree *newLeft=NULL,ExpressionTree *newRight=NULL){
        operation=t;
        left=newLeft;
        right=newRight;
    }
    void show(ostream &out) {
        if (left!=NULL) {
          left->show(out);
          out << "<--";
        }
        out << "[" <<operation.value<<"]"; 
        if (right!=NULL) {
          out <<
          "-->";
          right->show(out);
        }
    }
};