#pragma once

#include "tokenTokenizer.cpp"
#include "expressionTree.cpp"
#include <vector>
#include <string>

using namespace std;

class Parser{
    Tokenizer tokenizer;
    vector<string> error;
    public:

    /*while(declaration(subtree)){
        vars.push_back(subtree)
    }*/
    
    bool declaration(ExpressionTree &tree){
        Token next=tokenizer.next();
        if(next.type==DATATYPE){
            ExpressionTree *subtree=new ExpressionTree;
            next=tokenizer.next();
            if(next.type==VARNAME){
                subtree=new ExpressionTree(next,NULL,NULL);
                if(init_decl(*subtree)){
                    tree=*subtree;
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
        if(next.type==EQUAL){
            ExpressionTree *subtree=new ExpressionTree();
            if(expression(*subtree)){
                tree.right=subtree;
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
        else error.push_back("Syntax error expected a primary expression");
        return false;
    }

    ExpressionTree scan(string s){
        ExpressionTree tree; // Empty Tree Really
        tokenizer.start(s);
        if (declaration(tree)){
            return tree;
        } 
        else {
            for(auto i:error)
			    cerr << i << endl;
        }
        return ExpressionTree();
    }
};