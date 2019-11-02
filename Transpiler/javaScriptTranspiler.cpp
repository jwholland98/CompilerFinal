#include <iostream>
#include "tokenTokenizer.cpp"
#include "expressionTree.cpp"
#include "parser.cpp"

using namespace std;

int main(){
	Parser p;
	ExpressionTree t=p.scan("int x=(2+3);");
	t.show(cout);
	return 0;
}