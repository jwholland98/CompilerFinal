#include <iostream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

int main(){
	Parser p;
	string expression = 
						//"#include <iostream>"
						//"using namespace std;"
						"int main(){"
							"int i=0;"
							"cout << \"hi\" << i << endl << \"hi\";"
							/*"for(int i=0;i<3;i=i+1){"
								"int c=0;"
							"}"
							"if(i<3){"
								"int y=0;"
							"}else{"
								"int b=0;"
							"}"*/
						"}"/*
						
						"int test(){"
								"int x=5;"
						"}"*/;
	p.scan(expression);
	summary();
	return 0;
}