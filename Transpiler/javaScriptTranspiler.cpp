#include <iostream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

int main(){
	Parser p;
	string expression = "for(int i;i<3;i=i+1)";
		/*"int x=(5+4);"
		"int y=(3+2);";*/
	p.scan(expression);
	summary();
	return 0;
}