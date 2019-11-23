#pragma once

#include "tokenTokenizer.h"
#include "expressionTree.h"
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

struct StateTree{
	ExpressionTree tree;
	string statement = "";
};


vector<StateTree> SymbolTable;

void summary(){
	for(auto i:SymbolTable){
		if(i.statement==""){
			i.tree.show(cout);
		}
		else
			cout << i.statement;
	}
}

class Parser{
    Tokenizer tokenizer;
    vector<string> error;
    public:

	bool start(){
		Token next=tokenizer.peek();
		if (next.type==INCLUDE){
			while(include()){
				StateTree s;
				s.statement = "\n";
				SymbolTable.push_back(s);
			}
		}
		next=tokenizer.peek();
		if(next.type==USING)
			next=tokenizer.next();
			next=tokenizer.next();
			if(next.type==VARNAME){
				next=tokenizer.next();
				if(next.type==SEMICOLON){
					next=tokenizer.peek();
				}else error.push_back("expected ;");
			}else error.push_back("expected name of namespace");
		//next=tokenizer.next();
		cout << next.value << endl;
		while(fun()){
			StateTree s;
			s.statement = "\n";
			SymbolTable.push_back(s);
		}
		return true;
	}

	bool include(){
		Token next=tokenizer.next();
		if (next.type==INCLUDE){
			next=tokenizer.next();
			if(next.value=="<"){
				next=tokenizer.next();
				if(next.type==VARNAME){
					next=tokenizer.next();
					if(next.value==">"){
						return true;
					}else error.push_back("expected >");
				}
			}else error.push_back("expected <");
		}
		return false;
	}


	bool fun(){
		Token next=tokenizer.peek();
		cout << next.value << endl;
		if(next.type==DATATYPE){
			next=tokenizer.next();
			next=tokenizer.next();
			cout << next.value << endl;
			if(next.type==VARNAME) {
				if(next.value=="main"){
					next=tokenizer.next();
					if(next.type==OPEN_PAREN){//can add params into main later
						next=tokenizer.next();
						if(next.type==CLOSE_PAREN){
							next=tokenizer.next();
							if(next.type==OPEN_BRACKET){
								while(statement()){}
								next=tokenizer.next();
								if(next.type==CLOSE_BRACKET){
									return true;
								}else error.push_back("expected }");
							}else error.push_back("expected {");
						}else error.push_back("expected )");
					}else error.push_back("expected (");
				}
				else {
					next=tokenizer.next();
					cout << next.value << endl;
					if (next.type==OPEN_PAREN) {
						next = tokenizer.peek();
						cout << next.value << endl << endl;
						if (next.type==DATATYPE) {
							next = tokenizer.next();
						}
						else if (next.type==CLOSE_PAREN) {
							next=tokenizer.next();
							if(next.type==OPEN_BRACKET){
								while(statement()) {
									cout << next.value << endl;
								}
								next=tokenizer.next();
								if(next.type==CLOSE_BRACKET){
									return true;
								} else error.push_back("expected }");
							} else error.push_back("expected {");
						}
					}
				}
			}else error.push_back("expected variable name");
		}
		return false;
	}

	bool statement(){//might need flag defaulted to false to know if coming from for loop
		Token next=tokenizer.peek();
		cout << endl;
		cout << "In statement. 'next' is: " << next.type << endl;
		cout << endl;
		if(next.type==DATATYPE){
			while(declaration()){
				StateTree s;
				s.statement = "\n";
				SymbolTable.push_back(s);
			}
			return true;
		}
		else if(next.type==FORLOOP){
			if(forloop())
				return true;
			else{
				error.push_back("Invalid syntax in for loop");
				return false;
			}
		}
		else if(next.type==WHILELOOP){
			if(whileloop())
				return true;
			else{
				error.push_back("Invalid syntax in for loop");
				return false;
			}
		}
		else if(next.type==IF){
			if(ifelse())
				return true;
			else{
				error.push_back("Invalid syntax for IF or ELSE statment");
			}
		}
		else if(next.type==VARNAME){
			next=tokenizer.next();
			//checks if var is declared already
			for(auto i:SymbolTable){
				if(i.statement==""){
					if(i.tree.operation.value==next.value){
						ExpressionTree subtree = i.tree;
						if(init_decl(subtree)){
							StateTree s;
							s.statement=subtree.operation.value + '=' + subtree.right->treeToString();
							SymbolTable.push_back(s);
							return true;
						}
					}
				}
			}
			error.push_back("undeclared variable used");
			return false;
		}
		else if(next.type==COUT){
			if(output())
				return true;
			else{
				error.push_back("Invalid syntax in cout");
				return false;
			}
		}
		else{
			error.push_back("Expected DataType, variable name, or for loop");
			return false;
		}
	}

	bool ifelse(){
		Token next=tokenizer.next();
		cout << "I'm in ifelse() and 'next' is: " << next.type << endl;
		if(next.type==IF){ // I'M RIGHT HERE++++++++++++++++++++++++++++++++++++++++++++++++++
			cout << "current type: " << next.type << endl;
			next=tokenizer.next();
			cout << "new type: " << next.type << endl;
			if(next.type==OPEN_PAREN){
				ExpressionTree *subtree= new ExpressionTree;
				if(expression(*subtree)){
					next=tokenizer.next();
					if(next.type==CLOSE_PAREN){ // Could add option not requiring brackets for single line expressions here
						next=tokenizer.next();
						if(next.type==OPEN_BRACKET){ // Could add option for continuing with empty statment here
							while(statement()){
								next=tokenizer.next();
							}
							if(next.type==CLOSE_BRACKET){
								next=tokenizer.peek();
								if(next.type==ELSE){
									next=tokenizer.next();
									next=tokenizer.next();
									if(next.type==OPEN_BRACKET){
										while(statement()){
											next=tokenizer.next();
										}
										if(next.type==CLOSE_BRACKET){
											return true;
										}else error.push_back("Expected } in 'if' statment");
									}else error.push_back("Expected { in 'if' statment");
								}else return true;
							}else error.push_back("Expected } in 'if' statement");
						}else error.push_back("Expected { in 'if' statement");
					}else error.push_back("Expected ) in 'if' statement");
				}else error.push_back("Expected expression in 'if' statement");
			}else error.push_back("Expected ( in 'if' statement");
		}else error.push_back("Expected 'if' keyword");
		return false;
	}
    
	bool forloop(){
		string forj;//string to store forloop
		StateTree s;
		Token next=tokenizer.next();
		if(next.type==FORLOOP){
			forj+=next.value;
			next=tokenizer.next();
			if(next.type==OPEN_PAREN){
				forj+=next.value;
				s.statement=forj;
				SymbolTable.push_back(s);
				if(declaration()){
					forj=";";
					ExpressionTree *subtree = new ExpressionTree;
					if(expression(*subtree)){//possibly too vague
						forj+=subtree->left->operation.value + subtree->operation.value + subtree->right->operation.value;
						next=tokenizer.next();
						if(next.type==SEMICOLON){
							forj+=";";
							s.statement=forj;
							SymbolTable.push_back(s);
							while(statement()){//doesnt allow i++
								next=tokenizer.next();
								if(next.type==CLOSE_PAREN){
									forj = next.value;
									next=tokenizer.next();
									if(next.type==OPEN_BRACKET){
										forj += next.value+'\n';
										s.statement=forj;
										SymbolTable.push_back(s);
										while(statement()){
											next=tokenizer.next();
										}
										if(next.type==CLOSE_BRACKET){
											forj =next.value + '\n';
											s.statement=forj;
											SymbolTable.push_back(s);
											return true;
										}else error.push_back("expected }");
									}else error.push_back("expected {");
								}else error.push_back("expected )");
							} //else error.push_back("expected expression");
						}else error.push_back("expected semicolon");
					}else error.push_back("expected expression");
				}else error.push_back("expected declaration");
			}else error.push_back("expected (");
		}else error.push_back("expected for keyword");
		return false;
	}

	bool whileloop(){
		string whilej;//string to store forloop
		StateTree s;
		Token next=tokenizer.next();
		if(next.type==WHILELOOP){
			whilej+=next.value;
			next=tokenizer.next();
			if(next.type==OPEN_PAREN){
				whilej+=next.value;
				ExpressionTree *subtree = new ExpressionTree;
				if(expression(*subtree)){//doesnt put in while
					whilej+=subtree->left->operation.value + subtree->operation.value + subtree->right->operation.value;
					next=tokenizer.next();
					if(next.type==CLOSE_PAREN){
						whilej+=next.value;
						next=tokenizer.next();
						if(next.type==OPEN_BRACKET){
							whilej += next.value+'\n';
							s.statement=whilej;
							SymbolTable.push_back(s);
							while(statement()){
								next=tokenizer.next();
							}
							if(next.type==CLOSE_BRACKET){
								whilej =next.value + '\n';
								s.statement=whilej;
								SymbolTable.push_back(s);
								return true;
							}else error.push_back("expected }");
						}else error.push_back("expected {");
					}else error.push_back("expected )");
				}else error.push_back("expected expression");
			}else error.push_back("expected (");
		}else error.push_back("expected while keyword");
		return false;
	}

	bool output(){
		return false;
	}

    bool declaration(){
		Token next=tokenizer.peek();//next few lines ensure close bracket in for loop isnt consumed
				if(next.type!=CLOSE_BRACKET)
					next=tokenizer.next();
        if(next.type==DATATYPE){
            ExpressionTree *subtree=new ExpressionTree;
            next=tokenizer.next();
            if(next.type==VARNAME){
                subtree=new ExpressionTree(next,NULL,NULL);
                if(init_decl(*subtree)){
					StateTree s;
					s.tree = *subtree;
					SymbolTable.push_back(s);
                    return true;
                }
            }
            else{
                error.push_back("expected varname");
                return false;
            }
        }
        error.push_back("expected data type");
        return false;
    }
    bool init_decl(ExpressionTree &tree){
        Token next=tokenizer.next();
        if(next.type==SEMICOLON){
            return true;
        }
        else if(next.type==EQUAL){
            ExpressionTree *subtree=new ExpressionTree();
            if(expression(*subtree)){
                tree.right=subtree;
				next=tokenizer.peek();//next few lines ensure close paren in for loop isnt consumed
				if(next.type!=CLOSE_PAREN)
					next=tokenizer.next();
                return true;
            }
            error.push_back("expected expression");
            return false;
        }
        error.push_back("expected equal or semicolon at end of expression");
        return false;
    }
    bool expression(ExpressionTree &tree){
		ExpressionTree *subtree=new ExpressionTree();	
	    ExpressionTree *left=NULL;
	    Token last;
	    Token next=tokenizer.peek();
		while(additiveExpression(*subtree)){
			next=tokenizer.peek();
			if(!(next.type==RELATIONAL_OP)){
				if(last.type==EOL)//for case where no rel op is in the overall expression
					tree=*subtree;
				break;
			}
			else{
				next=tokenizer.next();
				if(left!=NULL)
					left=new ExpressionTree(last,left,subtree);
				else 
					left=subtree;
				subtree=new ExpressionTree();
				last=next;
			}
		}
		if(left!=NULL)
			tree=ExpressionTree(last,left,subtree);
		if(next.type==EOL || next.type==CLOSE_PAREN || next.type==SEMICOLON)
			return true;
		else error.push_back("Expected relative expression");
		return false;
    }
    bool additiveExpression(ExpressionTree &tree){
        ExpressionTree *subtree=new ExpressionTree();	
	    ExpressionTree *left=NULL;
	    Token last;
	    bool flag = false;
	    Token next=tokenizer.peek();
		while(multiplicativeExpression(*subtree)){
			next=tokenizer.peek();
			if(!(next.type==ADDITIVE_OP || next.type==PLUSMINUS_OP)){
				if(last.type==EOL){//for case where no add op is in the overall expression
					tree=*subtree;
				}
				if(next.type==RELATIONAL_OP)
					flag=true;
				break;
			}
			else{
				next=tokenizer.next();
				if(left!=NULL)
					left=new ExpressionTree(last,left,subtree);
				else 
					left=subtree;
				subtree=new ExpressionTree();
				last=next;
			}
		}
		if(left!=NULL)
			tree=ExpressionTree(last,left,subtree);
		if(flag || next.type==EOL || next.type==CLOSE_PAREN || next.type==SEMICOLON)
			return true;
		else error.push_back("Expected additive expression");
		return false;
    }
    bool multiplicativeExpression(ExpressionTree &tree){
	    ExpressionTree *subtree=new ExpressionTree();		
	    ExpressionTree *left=NULL;
	    Token last;
	    Token next=tokenizer.peek();
	    bool flag=false;
		while(unaryExpression(*subtree)){
			next=tokenizer.peek();
			if(!(next.type==MULTIPLICATIVE_OP)){
				if(last.type==EOL)//for case where no mult op is in the overall expression
					tree=*subtree;
				if(next.type==ADDITIVE_OP || next.type==PLUSMINUS_OP || next.type==RELATIONAL_OP)
					flag=true;
				break;
			}
			else{
				next=tokenizer.next();
				if(left!=NULL)
					left=new ExpressionTree(last,left,subtree);
				else 
					left=subtree;
				subtree=new ExpressionTree();
				last=next;
			}
		}
		if(left!=NULL) {
			tree=ExpressionTree(last,left,subtree);
		}
		if(flag || next.type==EOL || next.type==CLOSE_PAREN || next.type==SEMICOLON){
			return true;
		}
		else error.push_back("Expected multiplicative expression");
		return false;
    }
    bool unaryExpression(ExpressionTree &tree){
		Token next=tokenizer.peek();
		if(next.type==UNARY_OP || next.type==PLUSMINUS_OP){
			ExpressionTree *subtree=new ExpressionTree;
			next=tokenizer.next();
			if(unaryExpression(*subtree)){
				tree=ExpressionTree(next, NULL, subtree);
				return true;
			}
			else error.push_back("Expected unary expression");
		}
		else if(primaryExpression(tree))
			return true;
		else error.push_back("Expected a unary expression");
        return false;
    }
    bool primaryExpression(ExpressionTree &tree){
        Token next=tokenizer.next();
        if(next.type==OPEN_PAREN){
            ExpressionTree subtree;
            if(expression(subtree)){
				next=tokenizer.next();
				if(next.type!=CLOSE_PAREN) error.push_back("Syntax error missing )");
				else {
					tree=subtree;
					return true;
				}
            } 
            else error.push_back("Expected expression after (");
        }
        else if(next.type==UNSIGNED_INT){
            tree=ExpressionTree(next); // Just a leaf with an Unsigned Int
            return true;
        }
        else if(next.type==UNSIGNED_REAL){
            tree=ExpressionTree(next); // Just a leaf with an Unsigned Real
            return true;
        }
		else if(next.type==VARNAME){
			tree=ExpressionTree(next);
			return true;
			//check if varname is in list of vars
			/*for(auto i:SymbolTable){
				if(!((*typeid(i).name())=='s')){

				}
			}*/
		}
        else error.push_back("Syntax error expected a primary expression");
        return false;
    }

    ExpressionTree scan(string s){
        ExpressionTree tree; // Empty Tree Really
        tokenizer.start(s);
        if (start()){
            return tree;
        } 
        else {
            for(auto i:error)
			    cerr << i << endl;
        }
        return ExpressionTree();
    }
};

/*
bool forloop(){
		string forj;//string to store forloop
		Token next=tokenizer.next();
		if(next.type==FORLOOP){
			next=tokenizer.next();
			if(next.type==OPEN_PAREN){
				next=tokenizer.next();
				if(next.type==DATATYPE){
					next=tokenizer.next();
					if(next.type==VARNAME){
						next=tokenizer.next();//doesn't currently allow for int i=0, only int i, might just call declaration instead
						if(next.type==SEMICOLON){
							next=tokenizer.peek();
							ExpressionTree *subtree = new ExpressionTree;
							if(expression(*subtree)){//possibly too vague
								next=tokenizer.next();
								if(next.type==SEMICOLON){
									next=tokenizer.next();
									ExpressionTree *subtree2 = new ExpressionTree;
									if(expression(*subtree2)){//possibly too vague;doesnt allow i++; breaks here; need to call init_decl i believe
										next=tokenizer.next();
										if(next.type==CLOSE_PAREN){
											next=tokenizer.next();
											if(next.type==OPEN_BRACKET){
												next=tokenizer.next();
												cout << "yay" << endl;
												return true;
												//call statement I believe in here
												//wait till after statement call to append }
											}else error.push_back("expected )");
										}else error.push_back("expected )");
									}else error.push_back("expected expression");
								}else error.push_back("expected semicolon");
							}else error.push_back("expected expression");
						}else error.push_back("expected semicolon");
					}else error.push_back("invalid varname");
				}else error.push_back("expected type declaration");
			}else error.push_back("expected (");
		}else error.push_back("expected for keyword");
		return false;
	}
*/