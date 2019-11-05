#include <iostream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

int main(){
	Parser p;
	ExpressionTree t=p.scan("int x=(2+3);");
	t.show(cout);
	return 0;
}