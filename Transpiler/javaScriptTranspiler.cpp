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
							"cout << \"hi\";"
							"for(int i=0;i<4;i=i+1){"
								"int z=5+4;"
								"int y=(3+2);"
								"int a=12;"
							//	"i++"
							"}"
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