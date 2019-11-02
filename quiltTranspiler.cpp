#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;

typedef enum { QLIT,ID,KEYWORD,EQUALS,COMMA,OPEN_PAREN,CLOSE_PAREN,EOL } TokenType;

class Token {
    public:
    TokenType type;
    string value;
    Token(TokenType newType=EOL,string newValue="") {
      type=newType;
      value=newValue;  
    }
    void output(ostream &out) { out << type << ' ' << value << endl; }
};

class Tokenizer{
    string line;
    unsigned pos;
    unsigned lineNumber;
    unsigned linePos;
    public:
    int getPosition() { return linePos; }
    int getLineNumber() { return lineNumber; }
    void start(string newLine) {
        line=newLine;
        pos=0;
        lineNumber=1;
        linePos=1;
    }
    Token peek() {
        smatch sm;
        string remaining=line.substr(pos);
        while (remaining[0]==' ' || remaining[0]=='\t') {
          if (remaining[0]=='\t') {
			  lineNumber++;
			  linePos=0;
		  }
		  pos++;
		  linePos++;
          remaining=line.substr(pos);
		}
		//cerr << remaining << endl;
        if (regex_match(remaining,sm,regex("(a|b).*"))) 
          return Token(QLIT,sm[1]);
        if (regex_match(remaining,sm,regex("((let)|(fun)|(turn)|(sew)|(val)|(in)|(end)).*")))
          return Token(KEYWORD,sm[1]);
        if (regex_match(remaining,sm,regex("([a-z]+).*"))) 
          return Token(ID,sm[1]);
        if (regex_match(remaining,sm,regex("(=).*"))) 
          return Token(EQUALS,sm[1]);
        if (regex_match(remaining,sm,regex("(\\,).*"))) 
          return Token(COMMA,sm[1]);
        if (regex_match(remaining,sm,regex("(\\().*"))) 
          return Token(OPEN_PAREN,sm[1]);
        if (regex_match(remaining,sm,regex("(\\)).*"))) 
          return Token(CLOSE_PAREN,sm[1]);
        return Token();
    }
    void output(ostream &out) {
		cout << line << endl;
        unsigned l=0;
        for (unsigned j=0;j<lineNumber-1;j++) 
          while (l<line.size()) {
              if (line[l]=='\t') break;
              l++;
          }
        l+=linePos+(lineNumber-1)*1;  
		cout << setw(l) << '^'<< endl;
	}
    Token next() {
        Token t;
        t=peek();
        pos+=t.value.size();
        linePos+=t.value.size();
        return t;
    }
};

class ExpressionTree{
    public:
    ExpressionTree *left,*right;
    vector<ExpressionTree *> parameters;
    Token operation;
    ExpressionTree(Token t=Token(),ExpressionTree *newLeft=NULL,ExpressionTree *newRight=NULL){
        operation=t;
        left=newLeft;
        right=newRight;
    }
    void setArguments(vector<ExpressionTree *> newParameters){
		parameters=newParameters;
	}
    void showRPN(ostream &out) {
        if (left!=NULL) left->showRPN(out);
        if (right!=NULL) right->showRPN(out);
        out << operation.value;
    }
    void show(ostream &out) {
        if (left!=NULL) {
			left->show(out);
			out << "<=";
		}
        out << "[" <<operation.value<<"]"; 
        if (right!=NULL) {
			out <<
			"=>";
			right->show(out);
		}
		if (parameters.size()>0) {
			out << '(' ;
			for (unsigned int i=0;i<parameters.size();i++)
				parameters[i]->show(out);
            out << ')' <<endl;
		}
    }
};

class SymbolEntry {
	public:
	vector<string> parameters;
	ExpressionTree *expression;	
};

typedef map <string, SymbolEntry > SymbolTable;
SymbolTable symbolTable;
typedef SymbolTable::iterator SymbolIterator;
ExpressionTree *inStatement;

void summary(ostream &out) {
    out << endl<< "Syntax Expression Tree Summary" <<endl;
    out << setfill('-') << setw(20) << ' ' <<endl;
	for (SymbolIterator i=symbolTable.begin();i!=symbolTable.end();i++) {
		out << "Symbol: "<< i->first;
        if (i->second.parameters.size()>0) {
		  out << " Fun" << '(';
		  for (unsigned int j=0;j< i->second.parameters.size();j++) {
			if (j>0) cout << ", ";
			out << i->second.parameters[j];
		  }
		  out << ')' << endl;
        } else 
          out << " Val" << endl;
		i->second.expression->show(out);
		out << endl;
	}
    out << "In ";
	inStatement->show(out);
    out << endl;
    out << setfill('-') << setw(20) << ' ' <<endl;
}

class Parser{
    Tokenizer tokenizer;
    vector<string> errors;
    public:
    bool expression(ExpressionTree &tree) {
        Token t=tokenizer.next();
        if (t.type==QLIT) {
            if (t.value=="a") 
                tree=ExpressionTree(t);
            if (t.value=="b") 
                tree=ExpressionTree(t);
            return true;
        }
        if (t.type==ID) {
            Token node=t;
            t=tokenizer.peek();
            if (t.type==OPEN_PAREN) {
				/* function call */
				vector <ExpressionTree *> parameters;
				t=tokenizer.next();
				while (true) {
					ExpressionTree *subtree=new ExpressionTree();
					if (expression(*subtree)){
						parameters.push_back(subtree);
						t=tokenizer.next();
						if (t.type==CLOSE_PAREN) {
							//node.output(cout);
							tree=ExpressionTree(node);
							tree.setArguments(parameters);
							return true;
						} 
						if (t.type!=COMMA) {
							errors.push_back("Expected ) or , in function call");
							return false;
						}
					}
				}
			} else  {
			  tree=ExpressionTree(node);
              return true;
		    }
        }
        if (t.type==KEYWORD && t.value=="turn") {
			Token node=t;
            t=tokenizer.next();
            if (t.type==OPEN_PAREN) {
                ExpressionTree *subtree=new ExpressionTree();
                if (expression(*subtree)) {
                    t=tokenizer.next();
                    if (t.type==CLOSE_PAREN){
                        tree=ExpressionTree(node,NULL,subtree);
                        return true;
                    }
                    errors.push_back("Expected )");
                    return false;
                }
                errors.push_back("Expected Expression");
                return false;
            }
            errors.push_back("Expected (");
            return false;
        }
        if (t.type==KEYWORD && t.value=="sew") {
			Token node=t;
            t=tokenizer.next();
            if (t.type==OPEN_PAREN) {
                ExpressionTree *first=new ExpressionTree();
                if (expression(*first)) {
					//cout << "Checking left of sew " << endl;
					//first->show(cout);
                    t=tokenizer.next();
                    if (t.type==COMMA){
                      ExpressionTree *second=new ExpressionTree();
                      if (expression(*second)) {
                        t=tokenizer.next();
                        if (t.type==CLOSE_PAREN){
                            tree=ExpressionTree(node,first,second);
                            return true;
                        }
                        errors.push_back("Expected )");
                        return false;
                      }
                      errors.push_back("Expected expression");
                      return false;
                    }
                    errors.push_back("Expected ,");
                    return false;
                }
                errors.push_back("Expected expression");
                return false;
            }
            errors.push_back("Expected (");
            return false;
        }
        errors.push_back("Expected a, b, id, turn, or sew");
        return false;
    }
    bool val() {
        Token t=tokenizer.next();
        if (t.type==KEYWORD && t.value=="val") {
            Token id=tokenizer.next();
            if (id.type==ID) {
                //string valName=t.value;
                t=tokenizer.next();
                if (t.type==EQUALS) {
                    ExpressionTree *subtree=new ExpressionTree();
                    if (expression(*subtree)) {
                        SymbolEntry se;
                        se.expression=subtree;
                        //cout << "Adding " << id.value << " to symbol table " << endl;
                        symbolTable[id.value]=se;
                        return true;
                    }
                    errors.push_back("Expected expression");
                    return false;
                }
                errors.push_back("Expected = ");
                return false;
            }
            errors.push_back("Expected an identifier");
            return false; 
        }
        errors.push_back("Expected val expression");
        return false;
    }
    bool fun() {
		/* function definition */
        Token t=tokenizer.next();
        if (t.type==KEYWORD && t.value=="fun") {
            Token funid=tokenizer.next();
            if (funid.type==ID) {
                t=tokenizer.next();
                if (t.type==OPEN_PAREN) {
   				    SymbolEntry se;
   				    Token var;
					do {
                      var=tokenizer.next();
                      if (var.type==ID)
						se.parameters.push_back(var.value);
                      var=tokenizer.next();  
                      if (!(var.type==COMMA || var.type==CLOSE_PAREN)) {
                            errors.push_back("Expected a , or ) here");
                            return false;
                      }
                    } while (var.type==COMMA);
                    if (var.type==CLOSE_PAREN) {
                        t=tokenizer.next();
                        if (t.type==EQUALS) {
                            ExpressionTree *subtree=new ExpressionTree();
                            if (expression(*subtree)) {
                                se.expression=subtree;
                               // cout << "Adding " << funid.value << " to symbol table " << endl;
                                symbolTable[funid.value]=se;
                                return true;
                            }
                            errors.push_back("Expected expression");
                            return false;
                        }
                        errors.push_back("Expected =");
                        return false;
                    }
                    errors.push_back("Expected )");
                    return false;
                }
                errors.push_back("Expected (");
                return false;
            }
            errors.push_back("Expected an identifier");
            return false; 
        }
        errors.push_back("Expected fun expression");
        return false;
    }
    bool declarations() {
        Token t=tokenizer.peek();
        while (t.type==KEYWORD && (t.value=="fun" || t.value=="val")) {
          if (t.value=="fun") {
            if (!fun()) {
				errors.push_back("Expected a fun declaration");
				return false;
			}
		  }
          if (t.value=="val") {
            if (!val()) {
				errors.push_back("Expected a val declaration");
				return false;
			}
		  } 
		  t=tokenizer.peek();
        }
        return true;
    }
    bool let() {
        Token t=tokenizer.next();
        if (t.type==KEYWORD && t.value=="let") {
            if (declarations()) {
                t=tokenizer.next();
                if (t.type==KEYWORD && t.value=="in"){
                    ExpressionTree *exp=new ExpressionTree();
                    if (expression(*exp)) {
                        t=tokenizer.next();
                        if (t.type==KEYWORD && t.value=="end") {
                          inStatement=exp;
                          return true;
                        }
                        errors.push_back("Expected end");
                        return false;
                    }
                    errors.push_back("Expected an expression");
                    return false;
                }
                errors.push_back("Expected 'in' keyword");
                return false;
            }
            errors.push_back("Expected declarations");
            return false;
        }
        errors.push_back("Expected let expression");
        return false;
    }
    string error() {
        stringstream ss;
        ss << tokenizer.getLineNumber() << ':' << tokenizer.getPosition()<<endl;
        return ss.str();
    }
    bool scan(string s){
        tokenizer.start(s);
        if (!let()) {
           tokenizer.output(cout);
           for (unsigned i=0;i<errors.size();i++) 
             cerr << errors[i] << endl;
           return false;
        }
        return true;
    }
};

class Shape {
	vector <Shape *> shapes;
	string id,transform,rotation;
	protected:
	int w,h;
	public:
	int getW() { return w; }
	int getH() { return h; }
	void setId(string newId) { id=newId; }
	void setTransform(int tx,int ty) {
		stringstream ss;
		ss << "translate("<< tx << ' ' << ty << ") ";
		transform=ss.str();
	}
	void setRotation(int tx, int ty) {
		int t=w;
		w=h;
		h=t;
		stringstream ss;
		ss << "rotate(90 " <<tx << ' ' << ty << ") ";
		rotation=ss.str(); }
	void add(Shape *other) {
	    w+=other->w;
	    h=other->h;
		shapes.push_back(other);
	}
	virtual void output(ostream &out) {
		out << "<g ";
		if (id.size()>0) out << "id='"<<id << "'";
		if (transform.size()>0 || rotation.size()>0){
			 out << "transform='";
			 if (transform.size()>0)  out << transform << ' ';
		     if (rotation.size()>0) out << rotation;
		     out << "'";
	    }
		out << ">" << endl;
		for(Shape *s:shapes) s->output(out);
        out << "</g>" << endl;
	}
};

const bool A=false;
const bool B=true;
class Literal:public Shape {
	bool which;
	public:
	Literal(bool newWhich) { 
		which=newWhich; 
		w=20;
		h=20;
	}
	void output(ostream &out) {
		string color;
		if (which==A) color="fill = 'tomato' id='a'"; 
		else color="fill = 'lightgreen' id='b'"; 
		out << "<rect  width = '20' height = '20' "<<color<<" />"<<endl;
		if (which==B) color="fill = 'tomato' "; 
		else color="fill = 'lightgreen'" ; 
        out << "<circle cx='10' cy='5' r='5' "<<color<<"/>" <<endl;
	}
};

class Value:public Shape {
	string id;
	public:
	Value(string newId) { 
		w=20; // visit later
		h=20;
		id=newId;
	}
	void output(ostream &out) {
		out << "<use  href = '#"<< id <<"' />"<<endl;
	}
};

class CodeGenerator{
	public:
	static void preamble(ostream &out) {
        out << "<svg viewBox='-100 -100 200 200' xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink'>" << endl;
        out << "<line x1='0' y1='-40' x2='0' y2='40' style='stroke: black'/>"<<endl;
        out << "<line x1='-40' y1='0' x2='40' y2='0' style='stroke: black'/>"<<endl;
	}
	static void postamble(ostream &out)  {
        out << "</svg>"<<endl;
	}
	void loadSymbols(ostream &out,const SymbolTable &table) {
	}
	Shape *processRecurse(const ExpressionTree *tree) {
		if (tree->operation.type==QLIT) {
			Shape *l;
			if (tree->operation.value=="a") l=new Literal(A);
			else l=new Literal(B);
			Shape *group=new Shape;
			group->add(l);
			return group; 
		}
		if (tree->operation.type==ID) {
			if (tree->parameters.size()==0) {  // Use a Val
				Shape *v=new Value(tree->operation.value);
			    Shape *group=new Shape;
			    group->add(v);
			    return group; 				
			}
		}
		if (tree->operation.type==KEYWORD && tree->operation.value=="turn") {
			Shape *turnExp=new Shape;
			Shape *sub=processRecurse(tree->right);
            turnExp->setRotation(0,0);
            //turnExp->setTransform(sub->getH(),0);
            turnExp->add(sub);
            return turnExp;
		}
		if (tree->operation.type==KEYWORD && tree->operation.value=="sew") {
            Shape *sewExp=new Shape;
            Shape *subL=processRecurse(tree->left);
            subL->setTransform(subL->getW(),0);
            sewExp->add(subL);
            Shape *subR=processRecurse(tree->right);
            subR->setTransform(subL->getW()+subR->getW(),0);
            sewExp->add(subR);
            return sewExp;
		}
		return NULL;
	}
	void process(ostream &out,const ExpressionTree *tree) {
		preamble(out);
		for(SymbolIterator it=symbolTable.begin();it!=symbolTable.end();it++) {
			if (it->second.parameters.size()==0)  {
				out <<"<g visibility='hidden'>" << endl;
  				out <<"  <g id='" << it->first << "'>" << endl;
			    Shape *val=processRecurse(it->second.expression);
			    val->output(out);
			    out << "  </g>" << endl;
			    out << "</g>" << endl;
		    }
		}
		Shape *s=processRecurse(tree);
		s->output(out);
		postamble(out);
	}
};

int main(int argc, char **argv) {
    ifstream in;
    stringstream ss;
    in.open(argv[1]);
    while (!in.eof()) {
        string line;
        getline(in,line);
        ss << line << '\t';
    }
    in.close();
    Parser p;
    if (p.scan(ss.str())) {
		cerr << "Good!" << endl;
        summary(cout);
        ofstream out;
        out.open(argv[2]);
        CodeGenerator cg;
        cg.process(out,inStatement);
        out.close();
        return 0;
	}
	else {
        cerr << argv[1]<<':'<< p.error() << endl;
		cerr << "Bad!" << endl;
        return 1;
    }
}


/*
Sample "Assembly for SVG"
<svg viewBox="-40 0 150 100" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <g transform="rotate(-10 50 100)
                translate(0 10)">
    <rect id="a" x = "0" y = "0" width = "20" height = "20" fill = "tomato" stroke = "lightgreen" stroke-width = "2" />  
    <g transform="rotate(-10 50 100)
                translate(10 0)">
      <use xlink:href="#a"/>
    </g>  
  </g>
  <use xlink:href="#a"/>
</svg>

** Val Declaration ** 
<g visibility='hidden'>
  <g id='x'>
    <rect  width = '20' height = '20' fill = 'lightgreen' />
    <circle cx='10' cy='5' r='5' fill = 'tomato' />
  </g>
</g>

** Val use **
  <use href="#x"/>

*/