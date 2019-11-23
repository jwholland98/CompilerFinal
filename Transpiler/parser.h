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
	cout << endl << endl << "***SUMMARY***" << endl;
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
		cout << "***START***" << endl;
		StateTree s;
		Token next;//=tokenizer.peek();
		while(include(next)){
			s.statement = "\n";
			SymbolTable.push_back(s);
		}
		//next=tokenizer.peek();
		while(namespaces(next)) {
			s.statement = "\n";
			SymbolTable.push_back(s);
		}
		//next=tokenizer.next();
		//cout << next.value << endl;
		while(fun(next)){
			s.statement = "\n";
			SymbolTable.push_back(s);
		}
		return true;
	}

	bool include(Token &next){
		next=tokenizer.peek();
		if (next.type==INCLUDE) {
			cout << endl << endl << "***INCLUDE***" << endl;
			next=tokenizer.next();
			next=tokenizer.next();
			if(next.value=="<"){
				next=tokenizer.next();
				if(next.type==VARNAME){
					next=tokenizer.next();
					if(next.value==">"){
						//next=tokenizer.peek();
						return true;
					} else error.push_back("expected >");
				}
			} else error.push_back("expected <");
		}
		return false;
	}

	bool namespaces(Token &next) {
		next = tokenizer.peek();
		//cout << endl << endl<< next.value << endl << endl;
		if(next.type==USING) {
			cout << endl << endl << "***NAMESPACES***" << endl;
			next=tokenizer.next();
			next=tokenizer.next();
			if(next.type==VARNAME){
				next=tokenizer.next();
				if(next.type==SEMICOLON){
					//next=tokenizer.peek();
					return true;
				} else error.push_back("expected ;");
			} else error.push_back("expected name of namespace");
		}
		return false;
	}

	bool fun(Token &next){
		cout << endl << endl << "***FUNCTIONS***" << endl;
		next=tokenizer.peek();
		//cout << next.value << endl;
		if(next.type==DATATYPE){
			next=tokenizer.next();
			next=tokenizer.next(); 
			//cout << next.value << endl;
			if(next.type==VARNAME) {
				if(next.value=="main"){
					next=tokenizer.next();
					if(next.type==OPEN_PAREN){//can add params into main later
						next=tokenizer.next();
						if(next.type==CLOSE_PAREN){
							next=tokenizer.next();
							if(next.type==OPEN_BRACKET){
								next=tokenizer.peek();
								while(statement(next)){}
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
					//cout << next.value << endl;
					if (next.type==OPEN_PAREN) {
						next = tokenizer.peek();
						//cout << next.value << endl << endl;
						if (next.type==DATATYPE) {
							next = tokenizer.next();
						}
						else if (next.type==CLOSE_PAREN) {
							next=tokenizer.next();
							if(next.type==OPEN_BRACKET){
								while(statement(next)) {
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


	bool statement(Token &next){//might need flag defaulted to false to know if coming from for loop
		cout << endl << endl << "***STATEMENT***" << endl;
		if(next.type==DATATYPE){
			while(declaration(next)){
				StateTree s;
				s.statement = "\n";
				SymbolTable.push_back(s);
			}
			return true;
		}
		else if(next.type==FORLOOP){
			if(forloop(next))
				return true;
			else{
				error.push_back("Invalid syntax in for loop");
				return false;
			}
		}
		else if(next.type==WHILELOOP){
			if(whileloop(next))
				return true;
			else{
				error.push_back("Invalid syntax in for loop");
				return false;
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
		if(next.type==COUT){
			if(output(next))
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
    
	bool forloop(Token &next){
		string forj;//string to store forloop
		StateTree s;
		next=tokenizer.next();
		if(next.type==FORLOOP){
			forj+=next.value;
			next=tokenizer.next();
			if(next.type==OPEN_PAREN){
				forj+=next.value;
				s.statement=forj;
				SymbolTable.push_back(s);
				if(declaration(next)){
					forj=";";
					ExpressionTree *subtree = new ExpressionTree;
					if(expression(*subtree)){//possibly too vague
						forj+=subtree->left->operation.value + subtree->operation.value + subtree->right->operation.value;
						next=tokenizer.next();
						if(next.type==SEMICOLON){
							forj+=";";
							s.statement=forj;
							SymbolTable.push_back(s);
							while(statement(next)){//doesnt allow i++
								next=tokenizer.next();
								if(next.type==CLOSE_PAREN){
									forj = next.value;
									next=tokenizer.next();
									if(next.type==OPEN_BRACKET){
										forj += next.value+'\n';
										s.statement=forj;
										SymbolTable.push_back(s);
										next=tokenizer.peek();
										while(statement(next)){
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

	bool whileloop(Token &next){
		string whilej;//string to store forloop
		StateTree s;
		next=tokenizer.next();
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
							next=tokenizer.peek();
							while(statement(next)){
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

	bool output(Token &next){
		string outj;//string to store output
		StateTree s;
		int counter=0;
		cout << next.value << endl;
		if(next.type==COUT){
			next=tokenizer.next();
			outj="process.stdout.write(";
			next=tokenizer.next();
			//cout << next.value << endl;
			while(next.type==OSTREAM){
				s.statement=outj;
				SymbolTable.push_back(s);
				outj="";
				if(counter>=1)//makes sure a comma is put in for additional outputs beyond first
					outj+=",";
				counter++;
				next=tokenizer.peek();
				if(next.type==QUOTS){
					next=tokenizer.next();
					outj+="\"";
					next=tokenizer.next();//possible bug where it doesnt grab everything between quots
					outj+=next.value;
					next=tokenizer.next();
					if(next.type==QUOTS){
						outj+="\"";
						next=tokenizer.next();
					}
				}
				else if(next.type==statement(next))  //too vague for all cases
					next=tokenizer.next();
			}
			if(next.type==SEMICOLON){
				outj+=");\n";
				s.statement=outj;
				SymbolTable.push_back(s);
			}
		}else error.push_back("expected cout keyword");
		return false;
	}

    bool declaration(Token &next){
		  next=tokenizer.peek();
      if(next.type==DATATYPE){
			next=tokenizer.next();
            ExpressionTree *subtree=new ExpressionTree;
            next=tokenizer.peek();
            if(next.type==VARNAME){
				next=tokenizer.next();
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
		cout << endl << next.value << endl;
        if(next.type==SEMICOLON){
			return true;
        }
        else if(next.type==EQUAL){
            ExpressionTree *subtree=new ExpressionTree();
            if(expression(*subtree)){
                tree.right=subtree;
				next=tokenizer.peek();
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