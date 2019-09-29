#include <iostream>
#include <vector>
#include <string>
#include <regex>

using namespace std;

typedef enum{PLUSMINUS_OP, RELATIONAL_OP,ADDITIVE_OP,MULTIPLICATIVE_OP,UNARY_OP,UNSIGNED_INT,UNSIGNED_REAL,OPEN_PAREN,CLOSE_PAREN,EOL } TokenType;

class Token{
    public:
    TokenType type;
    string value;
    Token(TokenType newType=EOL,string newValue="") {
		type=newType;
		value=newValue;  
    }
};

class Tokenizer{
    string line;
    int pos;
    public:
    int getPosition(){
        return pos;
    }
    void start(string newLine){
        line=newLine;
        line.erase(remove(line.begin(), line.end(), ' '), line.end());//removes whitespace
        pos=0;
    }
    Token peek() {
        smatch sm;
        string remaining=line.substr(pos);
        if (regex_match(remaining,sm,regex("(\\+|-).*"))) 
          return Token(PLUSMINUS_OP,sm[1]);
        if (regex_match(remaining,sm,regex("((<=)|(=>)|(<>)|<|=|>).*"))) 
          return Token(RELATIONAL_OP,sm[1]);
        if (regex_match(remaining,sm,regex("(or).*"))) 
          return Token(ADDITIVE_OP,sm[1]);
        if (regex_match(remaining,sm,regex("(\\*|/|div|mod|and).*"))) 
          return Token(MULTIPLICATIVE_OP,sm[1]);
        if (regex_match(remaining,sm,regex("(not).*"))) 
          return Token(UNARY_OP,sm[1]);
        if (regex_match(remaining,sm,regex("([0-9]+).*"))) 
          return Token(UNSIGNED_INT,sm[1]);
        if (regex_match(remaining,sm,regex("([0-9]*/.[0-9]+).*"))) 
          return Token(UNSIGNED_REAL,sm[1]);
        if (regex_match(remaining,sm,regex("(\\().*"))) 
          return Token(OPEN_PAREN,sm[1]);
        if (regex_match(remaining,sm,regex("(\\)).*"))) 
          return Token(CLOSE_PAREN,sm[1]);
        return Token();
    }
    Token next(){
        Token t;
        t=peek();
        pos+=t.value.size();
        return t;
    }
};

class ExpressionTree{
  public:
    ExpressionTree *left,*right;
    Token operation;
    ExpressionTree(Token t=Token(),ExpressionTree *newLeft=NULL,ExpressionTree *newRight=NULL){
        operation=t;
        left=newLeft;
        right=newRight;
    }
    void showRPN(ostream &out){
        if (left!=NULL) left->showRPN(out);
        if (right!=NULL) right->showRPN(out);
        out << operation.value;
    }
    void show(ostream &out) {
        if (left!=NULL) {
			left->show(out);
			out << "<--";
		}
        out << "[" <<operation.value<<"]"; 
        if (right!=NULL) {
			out <<
			"-->";
			right->show(out);
		}
    }
};

class Parser{
    Tokenizer tokenizer;
    string error;
    public:
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
		if(next.type==EOL || next.type==CLOSE_PAREN)
			return true;
		else error="Expected relative expression";
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
		if(flag || next.type==EOL || next.type==CLOSE_PAREN)
			return true;
		else error="Expected additive expression";
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
		if(flag || next.type==EOL || next.type==CLOSE_PAREN){
			return true;
		}
		else error="Expected multiplicative expression";
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
			else error="Expected unary expression";
		}
		else if(primaryExpression(tree))
			return true;
		else error="Expected a unary expression";
        return false;
    }
    bool primaryExpression(ExpressionTree &tree){
        Token next=tokenizer.next();
        if(next.type==OPEN_PAREN){
            ExpressionTree subtree;
            if(expression(subtree)){
				next=tokenizer.next();
				if(next.type!=CLOSE_PAREN) error="Syntax error missing )";
				else {
					tree=subtree;
					return true;
				}
            } 
            else error="Expected expression after (";
        }
        else if(next.type==UNSIGNED_INT){
            tree=ExpressionTree(next); // Just a leaf with an Unsigned Int
            return true;
        }
        else if(next.type==UNSIGNED_REAL){
            tree=ExpressionTree(next); // Just a leaf with an Unsigned Real
            return true;
        }
        else error="Syntax error expected a primary expression";
        return false;
    }
    ExpressionTree scan(string s){
        ExpressionTree tree; // Empty Tree Really
        tokenizer.start(s);
        if (expression(tree)) {
            return tree;
        } 
        else {
			cerr << error << ":" << tokenizer.getPosition() << endl;
        }
        return ExpressionTree();
    }
};

vector<string> testExpressions;
void loadTest(){
	//Some expressions to try that should work
	testExpressions.push_back("1+2");
	testExpressions.push_back("(1+2)*3");
	testExpressions.push_back("((1+2)*3)");
	
	//Some that should error out in some informative way
	
	testExpressions.push_back("1+");
	testExpressions.push_back("(1+2*3");
}

int main(){
	Parser p;
	loadTest();
	for (unsigned i=0;i<testExpressions.size();i++) {
		ExpressionTree t=p.scan(testExpressions[i]);
		t.showRPN(cout);
		cout << endl;
	}
	ExpressionTree t=p.scan("(5*2)<>(2+4)");
	t.show(cout);	
	return 0;
}

/*bool additiveExpression(ExpressionTree &tree){//if this doesn't work, could try adding condition in mult if it sees add-op; also need to check for 
        ExpressionTree *subtree=new ExpressionTree();
        ExpressionTree *subtree2=new ExpressionTree();		
	    ExpressionTree *left=NULL;
	    Token last;
	    Token next=tokenizer.peek();
	    bool flag=false, flag2=false;
	    if(next.type==UNSIGNED_INT || next.type==UNSIGNED_REAL){//check for additive stuff before calling multiplicative
			last=tokenizer.next();
			next=tokenizer.next();
			if(next.type==PLUSMINUS_OP){
				//consume both and set up subtree
				//op as node, additive stuff on left, multiplicative on right; possibly switched
				flag=true;
				left=new ExpressionTree(last, NULL, NULL);
				subtree2=new ExpressionTree(next, left, NULL);
				left=NULL;
				last=EOL;
				next=tokenizer.peek();
			}
		}
		while(multiplicativeExpression(*subtree)){
			next=tokenizer.peek();
			if(!(next.type==ADDITIVE_OP || next.type==PLUSMINUS_OP)){
				if(last.type==EOL){//for case where no add op is in the overall expression; not sure if this works for all test cases
					tree=*subtree;
				}
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
			flag2=true;
		}
		if(flag){//heres error
			subtree2->right=subtree;
			tree=*subtree2;
			flag2=true;
		}
		if(flag2 || next.type==EOL || next.type==CLOSE_PAREN)
			return true;
		else error="Expected additive expression";
		return false;
    }*/

/*Token peek(){
        smatch sm;
        string remaining=line.substr(pos);
        if (regex_match(remaining,sm,regex("((<=)|(=>)|(<>)|<|=|>).*"))) 
			return Token(RELATIONAL_OP,sm[1]);
		//cout << "got here 2" << endl;
		if (regex_match(remaining,sm,regex("([-+]|(not)).*"))){ //needs testing when finished
			string past;
			if(pos!=0)past=line.substr(pos-1);
			else past=remaining;
			if(regex_match(past,sm, regex("([^0-9]).*"))){
				cout << sm[1] << endl;
				return Token(UNARY_OP,sm[1]);
			}
		}
		//cout << "got here 3" << endl;
        if(regex_match(remaining,sm,regex("([-+]|(or)).*"))) 
			return Token(ADDITIVE_OP,sm[1]);
		//cout << "got here 4" << endl;*/
        //if(regex_match(remaining,sm,regex("([*/]|(div)|(mod)|(and)).*"))) 
			/*return Token(MULTIPLICATIVE_OP,sm[1]);
		//cout << "got here 5" << endl;
        if(regex_match(remaining,sm,regex("([0-9]+\\.[0-9]+).*"))) 
			return Token(UNSIGNED_REAL,sm[1]);
		//cout << "got here 6" << endl;
        if(regex_match(remaining,sm,regex("([0-9]+).*")))
			return Token(UNSIGNED_INT,sm[1]);
		//cout << "got here 7" << endl;
        if(regex_match(remaining,sm,regex("(\\().*"))) 
			return Token(OPEN_PAREN,sm[1]);
		//cout << "got here 8" << endl;
        if(regex_match(remaining,sm,regex("(\\)).*"))) 
			return Token(CLOSE_PAREN,sm[1]);
		//cout << "got here 9" << endl;
        return Token();
    }*/


/* Language Specification
expression: 
   expression relational-op additive-expression 
   additive-expression  

relational-op: one of 
   <  <=  =  <>  =>  > 

additive-expression: 
   additive-expression additive-op multiplicative-expression 
   multiplicative-expression  

additive-op: one of 
   +  -  or 

multiplicative-expression: 
   multiplicative-expression multiplicative-op unary-expression 
   unary-expression  

multiplicative-op: one of 
   *  /  div  mod  and  // in 

unary-expression: 
   unary-op unary-expression  
   primary-expression  

unary-op:  one of 
   +  -  not 

primary-expression:  
   // variable  
   unsigned-integer  
   unsigned-real  
   // string  
   // nil  
   // funcid ( expression-list )  
   // [ element-list ]  
   ( expression ) */
   
   //Old peek function
   
   
