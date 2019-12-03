#include <iostream>
#include <fstream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

void codeGen(){
	ofstream out("output.txt");
	for(auto i:SymbolTable){
		if(i.statement==""){
			string line = "let " + i.tree.operation.value + " = " + i.tree.right->treeToString() + ";";
			out << line;
		}
		else
			out << i.statement;
	}
}


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
	codeGen();
	return 0;
}