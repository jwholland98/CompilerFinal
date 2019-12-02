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
						"}"
						
						"int foo(){"
								"int r=2;"
						"}"

						"int test(int x){"
								"x=5;"
						"}";
	p.scan(expression);
	summary();
	return 0;
}