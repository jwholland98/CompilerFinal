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
	p.scan(ss.str());
	summary();
	return 0;
}