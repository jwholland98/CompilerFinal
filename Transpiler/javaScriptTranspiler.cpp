#include <iostream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

int main(){
	Parser p;
	string expression = 
						"#include <iostream>"
						"using namespace std;"
						"int main(){"
							"cout << i;"
							"while(i>3){"
								"int x=5+4;"
								"int y=(3+2);"
								"int a=12;"
							"}"
						"}"/*
						
						"int test(){"
								"int x=5;"
						"}"*/;
	p.scan(expression);
	summary();
	return 0;
}