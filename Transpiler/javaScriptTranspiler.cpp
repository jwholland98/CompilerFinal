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
	string expression ="int main(){"
							"int i=0+3+i;"
							"if(i<3){"
								"int y=10;"
							"}else{"
								"int b=32;"
							"}"
						"}";
	p.scan(expression);
=======
	p.scan(ss.str());
>>>>>>> 5a2bcb352e9119f6514d5b0be5cba0a8ff17ffd8
	summary();
	return 0;
}