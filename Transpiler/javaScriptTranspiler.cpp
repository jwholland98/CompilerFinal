#include <iostream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

int main(){
	Parser p;
	string expression ="int main(){"
							"int i=0+3+i;"
							"if(i<3){"
								"int y=10;"
							"}else{"
								"int b=32;"
							"}"
						"}";
	p.scan(expression);
	summary();
	return 0;
}