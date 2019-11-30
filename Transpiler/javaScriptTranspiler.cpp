#include <iostream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

int main(){
	Parser p;
	string expression ="int main(){"
							"int i=0;"
							"if(i<3){"
								"int y=0;"
							"}else{"
								"int b=0;"
							"}"
						"}";
	p.scan(expression);
	summary();
	return 0;
}