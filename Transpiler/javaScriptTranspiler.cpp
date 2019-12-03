#include <iostream>
#include <fstream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

int main(){
	ifstream in;
    stringstream ss;
    in.open("input.txt");
    while (!in.eof()) {
        string line;
        getline(in,line);
        ss << line;
    }
    in.close();
	Parser p;
<<<<<<< HEAD
	p.scan(ss.str());
=======
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
>>>>>>> andrew-new
	summary();
	return 0;
}