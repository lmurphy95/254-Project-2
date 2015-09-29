/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.17.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
*/

#include <iostream>
#include <vector>
#include <string.h>
#include <cstdlib>
#include "scan.h"

//contains
#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end
using namespace std;

const char* names[] = {"read", "write", "while", "if", "end", "id", "literal", "gets",
    "add", "sub", "mul", "div", "equals", "not equals", "less than", "greater than", "less than or equal", "greater than or equal", "lparen", "rparen", "eof"};

token first_stmt[] = {t_id, t_read, t_write, t_if, t_while};
token follow_stmt[] = {t_eof, t_id, t_read, t_write, t_if, t_while};
token first_cond[] = {t_if};
token collow_cond[] = {t_eof};
token first_expr[] = {t_lparen, t_id, t_literal};
token follow_expr[] = {t_rparen, t_equals, t_not_equals, t_less, t_greater, t_greater_equal, t_less_equal, t_eof, t_id, t_read, t_write, t_if, t_while};

static token input_token;
std::vector<std::string> ast;
bool d = false; // debug

void pprint(std::vector<std::string>  vec) {
    cout << "\n";
    for (std::vector<string>::const_iterator i = vec.begin(); i != vec.end(); ++i){
        std::cout << *i;
    }
    cout << "\n";
}

void error () {
    //cout << "syntax error\n";
    throw "syntax error";
    exit (1);
}

void match (token expected) {
    if (input_token == expected) {
        if(d)cout << "matched " << names[input_token];
        if (d && (input_token == t_id || input_token == t_literal)) {
            cout << ": " << token_image;
            pprint(ast);
            cout << "\n";
        }
        if(d)cout << "\n";
        input_token = scan();
    }
    else error ();
}

void program ();
void stmt_list ();
void stmt ();
string cond ();
string expr ();
string term_tail (string in);
string term ();
string factor_tail (string in);
string factor ();
string r_op ();
string add_op ();
string mul_op ();
bool contains(token t, token a[]);

void program () {
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            if(d)cout << "predict program --> stmt_list eof\n";
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
            if(d)cout << "predict stmt_list --> stmt stmt_list\n";
            stmt ();
            stmt_list ();
            break;
        case t_end:
        case t_eof:
            if(d)cout << "predict stmt_list --> epsilon\n";
            break;          /*  epsilon production */
        default: error ();
    }
}

void stmt () {
    switch (input_token) {
        case t_id:
            if(d)cout << "predict stmt --> id gets expr\n";
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
            if(d)cout << "predict stmt --> read id\n";
            match (t_read);
            ast.push_back("(read ");
            ast.push_back(token_image);
            match (t_id);
            ast.push_back(")");
            break;
        case t_write:
            if(d)cout << "predict stmt --> write expr\n";
            match (t_write);
            ast.push_back("(write ");
            ast.push_back(expr());
            ast.push_back(")");
            break;
        case t_if:
            if(d)cout << "predict stmt --> if clause\n";
            match (t_if);
            ast.push_back("(if ");
            ast.push_back(cond());
            stmt_list();
            match (t_end);
            ast.push_back(")"); 
            break;
        case t_while:
            if(d)cout << "predict stmt --> while clause\n";
            match (t_while);
            ast.push_back("(while ");
            ast.push_back(cond());
            ast.push_back("(");
            stmt_list();
            ast.push_back(")");
            match (t_end);
            ast.push_back(")");
            break;
        default:
            cout << "syntax error: recieved " << token_image << " expected";
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
            tree.append("(");
            tree.append(rop);
            tree.append(" ");
            tree.append(e1);
            tree.append(" ");
            tree.append(e2);
            tree.append(")");
            return tree;
        default:
            while(true){
                token next_token;
                next_token = scan();
                switch (next_token) {
                    case t_if:
                        stmt();
                        break;
                    case t_end:
                    case t_eof:
                        break;
                    default:
                        continue;
                }
            }
    }
    return tree;
}

string expr () {
	string tree;
	string t;
	string t_tail;
    switch (input_token) {
        case t_id:
            if(d)cout << "predict expr --> term term_tail\n";
            t = term();
            t_tail = term_tail(t);
            if(t_tail=="")
                return t;
            else
                return t_tail;
        case t_literal:
            if(d)cout << "predict expr --> term term_tail\n";
            t = term();
            t_tail = term_tail(t);
            if(t_tail=="")
                return t;
            else
                return t_tail;
        case t_lparen:
            if(d)cout << "predict expr --> term term_tail\n";
            t = term();
            t_tail = term_tail(t);
            if(t_tail=="")
                return t;
            else
                return t_tail;
        default:
            while(true){
                token next_token;
                next_token = scan();
                switch (next_token) {
                    case t_id:
                    case t_literal:
                    case t_lparen:
                        stmt ();
                        break;
                    case t_end:
                    case t_eof:
                    case t_rparen:
                    case t_equals:
                    case t_not_equals:
                    case t_less:
                    case t_greater:
                    case t_less_equal:
                    case t_greater_equal:
                    case t_read:
                    case t_write:
                    case t_if:
                    case t_while:
                        break;
                    default:
                        continue;
                }
            }
    }
}

string term_tail (string input) {
	string tree;
	string a_op;
	string t;
	string t_tail;
    string s;
    switch (input_token) {
        case t_add:
        case t_sub:
            if(d)cout << "predict term_tail --> add_op term term_tail\n";
            a_op=add_op();
            t = term();
            s = "(" + a_op + " " + input + " " + t + ")";
            t_tail=term_tail(s);
            if(t_tail=="")
                tree.append(s);
            else
                tree.append(t_tail);
            return tree;
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
            if(d)cout << "predict term_tail --> epsilon\n";
            return "";          /*  epsilon production */
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
            if(d)cout << "predict term --> factor factor_tail\n";
            fac = factor();
            fac_tail=factor_tail(fac);
            if(fac_tail=="")
                return fac;
            else
                return fac_tail;
        default: error();
    }
}

string factor_tail (string input) {
	string tree;
	string mul;
	string fac;
	string fac_tail;
    string s;
    switch (input_token) {
        case t_mul:
        case t_div:
            if(d)cout << "predict factor_tail --> mul_op factor factor_tail\n";
            mul=mul_op();
            fac=factor();
            s = "(" + mul + " " + input + " " + fac + ")";
            fac_tail=factor_tail(s);
            if(fac_tail=="")
                tree.append(s);
            else
                tree.append(fac_tail);
            return tree;
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
            if(d)cout << "predict factor_tail --> epsilon\n";
            tree= "";
            return tree;          /*  epsilon production */
        default: error ();
    }
}

string factor () {
	string image;
    string first;
    string second;
    string s;
    switch (input_token) {
        case t_id :
            if(d)cout << "predict factor --> id\n";
            image = token_image;
            match (t_id);
            return image;
        case t_literal:
            if(d)cout << "predict factor --> literal\n";
            image = token_image;
            match (t_literal);
            return image;
        case t_lparen:
            if(d)cout << "predict factor --> lparen expr rparen\n";
            match (t_lparen);
            s=expr ();
            match (t_rparen);
            return s;
        default: error ();
    }
}

string r_op () {
    string rop;
    switch (input_token) {
        case t_equals:
            if(d)cout << "predict r_op --> equals\n";
            match(t_equals);
            return("==");
        case t_not_equals:
            if(d)cout << "predict r_op --> not_equals\n";
            match(t_not_equals);
            return("!=");
        case t_less:
            if(d)cout << "predict r_op --> less than\n";
            match(t_less);
            return("<");
        case t_greater:
            if(d)cout << "predict r_op --> greater than\n";
            match(t_greater);
            return(">");
        case t_less_equal:
            if(d)cout << "predict r_op --> less than or equal\n";
            match(t_less_equal);
            return("<=");
        case t_greater_equal:
            if(d)cout << "predict r_op --> greater than or equal\n";
            match(t_greater_equal);
            return(">=");
        default: error ();
            
    }
}

string add_op () {
    switch (input_token) {
        case t_add:
            if(d)cout << "predict add_op --> add\n";
            match (t_add);
            return("+");
        case t_sub:
            if(d)cout << "predict add_op --> sub\n";
            match (t_sub);
            return("-");
        default: error ();
    }
}

string mul_op () {
    switch (input_token) {
        case t_mul:
            if(d)cout << "predict mul_op --> mul\n";
            match (t_mul);
            return("*");
            break;
        case t_div:
            if(d)cout << "predict mul_op --> div\n";
            match (t_div);
            return("/");
            break;
        default: error ();
    }
}

bool contains(token x, token a[]) {
    int arraySize = (sizeof(a)/sizeof(*a));
    for(int i = 0; i < arraySize; i++){
         if(a[i] == x){
             return true;
         }
    }
    return false;
}

int main (int argc, char* argv[]) {
    if (argc == 2) {
        if(!strcmp(argv[1], "--debug") || !strcmp(argv[1], "-d")) {
            d = true;
        }
    }
    input_token = scan ();
    program ();
    return 0;
}