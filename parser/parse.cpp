/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.17.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
*/

#include <iostream>
#include <vector>
#include <cstdlib>
#include "scan.h"
using namespace std;

const char* names[] = {"read", "write", "while", "if", "end", "id", "literal", "gets",
    "add", "sub", "mul", "div", "equals", "not equals", "less than", "greater than", "less than or equal", "greater than or equal", "lparen", "rparen", "eof"};

static token input_token;
std::vector<std::string> ast;

void pprint(std::vector<std::string>  vec) {
    cout << "\n";
    for (std::vector<string>::const_iterator i = vec.begin(); i != vec.end(); ++i){
        std::cout << *i;
    }
    cout << "\n";
}


void error () {
    cout << "syntax error\n";
    exit (1);
}

void match (token expected) {
    if (input_token == expected) {
        cout << "matched " << names[input_token];
        if (input_token == t_id || input_token == t_literal) {
            cout << ": " << token_image;
            pprint(ast);
            cout << "\n";
        }
        cout << "\n";
        input_token = scan ();
    }
    else error ();
}

void program ();
void stmt_list ();
void stmt ();
string cond ();
string expr ();
string term_tail ();
string term ();
string factor_tail ();
string factor ();
string r_op ();
string add_op ();
string mul_op ();

void program () {
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            cout << "predict program --> stmt_list eof\n";
            ast.push_back("(program ");
            stmt_list ();
            match (t_eof);
            ast.push_back(")");
            pprint(ast);
            break;
        default: error ();
    }
}

void stmt_list () {
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
            cout << "predict stmt_list --> stmt stmt_list\n";
            stmt ();
            stmt_list ();
            break;
        case t_end:
        case t_eof:
            cout << "predict stmt_list --> epsilon\n";
            break;          /*  epsilon production */
        default: error ();
    }
}

void stmt () {
    switch (input_token) {
        case t_id:
            cout << "predict stmt --> id gets expr\n";
            match (t_id);
            ast.push_back("(:=");
            ast.push_back(" ");
            ast.push_back(token_image);
            ast.push_back(" ");
            match (t_gets);
            ast.push_back(expr());
            ast.push_back(")");
            break;
        case t_read:
            cout << "predict stmt --> read id\n";
            match (t_read);
            ast.push_back("(read ");
            ast.push_back(token_image);
            match (t_id);
            ast.push_back(")");
            break;
        case t_write:
            cout << "predict stmt --> write expr\n";
            match (t_write);
            ast.push_back("(write ");
            ast.push_back(expr());
            ast.push_back(")");
            break;
        case t_if:
            cout << "predict stmt --> if clause\n";
            match (t_if);
            ast.push_back("(if ");
            cond ();
            stmt_list ();
            match (t_end);
            ast.push_back(")"); 
            break;
        case t_while:
            cout << "predict stmt --> while clause\n";
            match (t_while);
            ast.push_back("(while ");
            ast.push_back(cond());
            stmt_list();
            match (t_end);
            ast.push_back(")");
            break;
        default:
            while(true){
                token next_token;
                next_token = scan();
                switch (next_token) {
                    case t_id:
                    case t_read:
                    case t_write:
                    case t_if:
                    case t_while:
                        stmt ();
                        return;
                    case t_end:
                    case t_eof:
                        return;
                    default:
                        continue;
                }
            }
    }
}

string cond () {
    string tree="";
    string e1;
    string e2;
    string rop;
    switch(input_token) {
        case t_lparen:
        case t_id:
        case t_literal:
            e1 = expr ();
            rop = r_op ();
            e2 = expr ();
 /*           tree.append("(");
            tree.append(rop);
            tree.append(" ");
            tree.append(e1);
            tree.append(" ");
            tree.append(e2);
            tree.append(")");*/
            return tree;
           // break;
        default: error ();
    }
    return tree;
}

string expr () {
	string tree;
	string t;
	string t_tail;
    switch (input_token) {
        case t_id:
            cout << "predict expr --> term term_tail\n";
            t = term();
            t_tail = term_tail();
            tree.append("(");
            tree.append(t_tail);
            tree.append(" ");
            tree.append(t);
            tree.append(")");
            return tree;
        case t_literal:
            cout << "predict expr --> term term_tail\n";
            t = term();
            t_tail = term_tail();
            tree.append(t_tail);
            tree.append(t);
            return tree;
        case t_lparen:
            cout << "predict expr --> term term_tail\n";
            t = term();
            t_tail = term_tail();
            tree.append("(");
            tree.append(t_tail);
            tree.append(" ");
            tree.append(t);
            tree.append(")");
            return tree;
            //break;
        default: error ();
    }
}

string term_tail () {
	string tree;
	string a_op;
	string t;
	string t_tail;
    switch (input_token) {
        case t_add:
        case t_sub:
            cout << "predict term_tail --> add_op term term_tail\n";
            a_op=add_op();
            t=term();
            t_tail=term_tail ();
            tree.append(a_op);
            tree.append(" ");
            tree.append(t_tail);
            tree.append(t);
            return tree;
            //break;
        case t_rparen:
        case t_equals:
        case t_not_equals:
        case t_less:
        case t_greater:
        case t_less_equal:
        case t_greater_equal:
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_end:
        case t_eof:
            cout << "predict term_tail --> epsilon\n";
            break;          /*  epsilon production */
        default: error ();
    }
}

string term () {
	string tree;
	string fac_tail;
	string fac;
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict term --> factor factor_tail\n";
            fac = factor();
            fac_tail=factor_tail();
            tree.append(fac);
            tree.append(fac_tail);
            return tree;
            //break;
        default: error();
    }
}

string factor_tail () {
	string tree;
	string mul;
	string fac;
	string fac_tail;
    switch (input_token) {
        case t_mul:
        case t_div:
            cout << "predict factor_tail --> mul_op factor factor_tail\n";
            mul=mul_op();
            fac=factor();
            fac_tail=factor_tail();
            tree.append(mul);
            tree.append(fac);
            tree.append(fac_tail);
            return tree;
            //break;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_equals:
        case t_not_equals:
        case t_less:
        case t_greater:
        case t_less_equal:
        case t_greater_equal:
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_end:
        case t_eof:
            cout << "predict factor_tail --> epsilon\n";
            break;          /*  epsilon production */
        default: error ();
    }
}

string factor () {
	string image;
    switch (input_token) {
        case t_id :
            cout << "predict factor --> id\n";
            image = token_image;
            match (t_id);
            return image;
            //break;
        case t_literal:
            cout << "predict factor --> literal\n";
            image = token_image;
            match (t_literal);
            return image;
            //break;
        case t_lparen:
            cout << "predict factor --> lparen expr rparen\n";
            match (t_lparen);
            expr ();
            match (t_rparen);
            break;
        default: error ();
    }
}

string r_op () {
    string rop;
    switch (input_token) {
        case t_equals:
            cout << "predict r_op --> equals\n";
            match(t_equals);
            return("==");
            //break;
        case t_not_equals:
            cout << "predict r_op --> not_equals\n";
            match(t_not_equals);
            return("!=");
            //break;
        case t_less:
            cout << "predict r_op --> less than\n";
            match(t_less);
            return("<");
            //break;
        case t_greater:
            cout << "predict r_op --> greater than\n";
            match(t_greater);
            return(">");
            //break;
        case t_less_equal:
            cout << "predict r_op --> less than or equal\n";
            match(t_less_equal);
            return("<=");
            //break;
        case t_greater_equal:
            cout << "predict r_op --> greater than or equal\n";
            match(t_greater_equal);
            return(">=");
            //break;
        default: error ();
            
    }
}

string add_op () {
    switch (input_token) {
        case t_add:
            cout << "predict add_op --> add\n";
            match (t_add);
            return("+");
            //break;
        case t_sub:
            cout << "predict add_op --> sub\n";
            match (t_sub);
            return("-");
            //break;
        default: error ();
    }
}

string mul_op () {
    switch (input_token) {
        case t_mul:
            cout << "predict mul_op --> mul\n";
            match (t_mul);
            return("*");
            //ast.push_back(" * ");
            break;
        case t_div:
            cout << "predict mul_op --> div\n";
            match (t_div);
            return("/");
            //ast.push_back(" / ");
            break;
        default: error ();
    }
}


int main (int argc, char* argv[]) {
    input_token = scan ();
    program ();
    return 0;

}
