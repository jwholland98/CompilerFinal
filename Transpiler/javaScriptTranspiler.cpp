#include <iostream>
#include <fstream>
#include "tokenTokenizer.h"
#include "expressionTree.h"
#include "parser.h"

using namespace std;

void codeGen() {
	ofstream out("output.html");
	out << "<!Doctype html>\n<html>\n\t<head></head>\n\t<body>\n\t\t<script>";
	for(auto i:SymbolTable) {
		if(i.statement=="") {
			string line = "let " + i.tree.operation.value + " = " + i.tree.right->treeToString() + ";";
			out << line;
		}
		else out << i.statement;
	}
	out << "\t\t</script>\n\t</body>\n</html>\n";
}


int main() {
	ifstream in("input.cpp");
    stringstream ss;
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