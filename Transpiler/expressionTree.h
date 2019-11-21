#pragma once

#include "tokenTokenizer.h"
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
    void show(ostream &out=cout) {
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
    string treeToString(string s=""){
        if (left!=NULL) {
          s+=left->treeToString(s);
        }
        s += operation.value;
        if (right!=NULL) {
          s=right->treeToString(s);//possible error here as it only is working with =, not +=
        }
        return s;
      }
};