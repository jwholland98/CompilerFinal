#pragma once

#include "tokenTokenizer.h"
#include <iostream>

class ExpressionTree {
  public:
    ExpressionTree *left,*right;
    Token operation;
    ExpressionTree(Token t=Token(),ExpressionTree *newLeft=NULL,ExpressionTree *newRight=NULL) {
        operation=t;
        left=newLeft;
        right=newRight;
    }
    string treeToString(string s="") {
        if (left!=NULL) {
          s+=left->treeToString(s);
        }
        s += operation.value;
        if (right!=NULL) {
          s=right->treeToString(s); // Possible error here. It only works with =, not +=
        }
        return s;
      }
};