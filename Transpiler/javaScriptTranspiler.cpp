#include <iostream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

int main(){
	Parser p;
	string expression = "#include <iostream>"
						"using namespace std;"//this has to be included for some reason; address later
						"int main(){"
							"int z = 0;"
							"int x = 1;"
							"if(x){"
								"int y = 2;"
								"if(z){"
									"int mynum = 17"
								"}"
							"}"
						"}";
	p.scan(expression);
	summary();
	return 0;
}