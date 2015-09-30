/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.17.  Prints a trace of productions predicted and
    tokens matched with --debug on.
*/

#include <iostream>
#include <vector>
#include <string.h>
#include <cstdlib>
#include "scan.h"

using namespace std;

const char* names[] = {"read", "write", "while", "if", "end", "id", "literal", "gets",
    "add", "sub", "mul", "div", "equals", "not equals", "less than", "greater than", "less than or equal", "greater than or equal", "lparen", "rparen", "eof"};

// first, follow sets
token first_stmtlist[] = {t_id, t_read, t_write, t_if, t_while, t_eof};
token follow_stmtlist[] = {t_end,t_eof};
token first_stmt[] = {t_id, t_read, t_write, t_if, t_while};
token follow_stmt[] = {t_eof, t_id, t_read, t_write, t_if, t_while};
token first_fact[] = {t_lparen, t_id, t_literal};
token follow_fact[] = {t_eof,t_mul,t_div};
token first_facttail[] = {t_eof,t_mul,t_div};
token follow_facttail[] = {t_eof,t_add,t_sub};
token first_term[] = {t_lparen,t_id,t_literal};
token follow_term[] = {t_eof,t_add,t_sub};
token first_termtail[] = {t_eof,t_add,t_sub};
token follow_termtail[] = {t_rparen, t_equals, t_not_equals, t_less, t_greater, t_greater_equal, t_less_equal, t_eof, t_id, t_read, t_write, t_if, t_while};
token first_cond[] = {t_lparen,t_id,t_literal};
token follow_cond[] = {t_eof,t_id,t_read,t_write,t_if,t_while};
token first_expr[] = {t_lparen, t_id, t_literal};
token follow_expr[] = {t_rparen, t_equals, t_not_equals, t_less, t_greater, t_greater_equal, t_less_equal, t_eof, t_id, t_read, t_write, t_if, t_while};
token first_ro[] = {t_equals, t_not_equals, t_less, t_greater, t_less_equal, t_greater_equal};
token follow_ro[] = {t_lparen,t_id,t_literal};
token first_ao[] = {t_add,t_sub};
token follow_ao[] = {t_lparen,t_id,t_literal};
token first_mo[] = {t_mul,t_div};
token follow_mo[] = {t_lparen,t_id,t_literal};

static token input_token;
std::vector<std::string> ast;
bool d = false; // debug tag prints trace & predict
bool err = false; // error


/* out error and exit */
void error (string err) {
    cout << err << "\n";
    exit (1);
}
/* print ast vector */
void pprint(std::vector<std::string>  vec) {
    cout << "\n";
    for (std::vector<string>::const_iterator i = vec.begin(); i != vec.end(); ++i){
        std::cout << *i;
    }
    cout << "\n";
}

/* is a token in a set? */
bool contains(token x, token a[]) {
    int arraySize = (sizeof(a)/sizeof(*a));
    for(int i = 0; i < arraySize; i++){
         if(a[i] == x){
             return true;
         }
    }
    return false;
}

/* print lines before err */
void p_lines(){
   cout << cur_line << ':' << cur_col << " ";
}

void match (token expected) {
    if (input_token == expected) {
        if(d)cout << "matched " << names[input_token];
        if (d && (input_token == t_id || input_token == t_literal)) {
            cout << ": " << token_image;
            if (!err)pprint(ast);
            cout << "\n";
        }
        if(d)cout << "\n";
        input_token = scan();
    }
    else{
        p_lines();
        cout << "[Parse Err]" << " Expected: " << names[expected]  << "," << " Received: " << names[input_token] << endl;
        if(input_token==t_eof)
            exit(1);
        err = true;
    }
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
            if (!err)pprint(ast);
            break;
        default: error ("unknown syntax error");
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
            break;/* epsilon production */
        default: //error ("syntax error");
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return stmt_list ();
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
            if (contains(input_token,follow_stmt)){
                p_lines();
                cout << "[Parse Err] Expected stmt\n";
                err = true;
                input_token = scan();
                return;
            }
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return stmt ();
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
            if (contains(input_token,follow_cond)){
                p_lines();
                cout << "[Parse Err] Expected cond\n";
                err = true;
                input_token = scan();
                return "";
            }
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return cond();
    }
    return tree;
}

string expr () {
	string tree;
	string t;
	string t_tail;
    token next_token;
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
            if (contains(input_token,follow_expr)){
                p_lines();
                cout << "[Parse Err] Expected expr\n";
                err = true;
                input_token = scan();
                return "";
            }
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return expr();
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
/*            p_lines();
            cout << "[Parse Err] Expected term tail\n";
*/            return ""; /* epsilon production */
        default:
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return term_tail(input);
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
        default:
            if (contains(input_token,follow_term)){
                p_lines();
                cout << "[Parse Err] Expected term\n";
                err = true;
                input_token = scan();
                return "";
            }
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return term();
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
/*            p_lines();
            cout << "[Parse Err] Expected factor tail\n";
            tree= "";
*/            return tree;/* epsilon production */
        default:
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return factor_tail(input);
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
        default:
            if (contains(input_token,follow_fact)){
                p_lines();
                cout << "[Parse Err] Expected factor\n";
                err = true;
                input_token = scan();
                return "";
            }
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return factor();
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
        default: error ("syntax error");
            if (contains(input_token,follow_ro)){
                p_lines();
                cout << "[Parse Err] Expected rop\n";
                err = true;
                input_token = scan();
                return "";
            }
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return r_op();
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
        default: error ("syntax error");
            if (contains(input_token,follow_ao)){
                p_lines();
                cout << "Expected ao\n";
                err = true;
                input_token = scan();
                return "";
            }
            if(input_token == t_eof){
                error("[Parse Err] unexpected EOF");
            }
            input_token = scan();
            return add_op();
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
        default: error ("syntax error");
            if (contains(input_token,follow_mo)){
                p_lines();
                cout << "[Parse Err] Expected factor\n";
                err = true;
                input_token = scan();
                return "";
            }
            if(input_token == t_eof){
                error("unexpected EOF");
            }
            input_token = scan();
            return mul_op();
    }
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