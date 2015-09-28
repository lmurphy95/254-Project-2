/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.17.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
*/

#include <iostream>

#include <cstdlib>
#include "scan.h"
using namespace std;

const char* names[] = {"read", "write", "while", "if", "end", "id", "literal", "gets",
    "add", "sub", "mul", "div", "equals", "not equals", "less than", "greater than", "less than or equal", "greater than or equal", "lparen", "rparen", "eof"};

static token input_token;

void error () {
    cout << "syntax error\n";
    exit (1);
}

void match (token expected) {
    if (input_token == expected) {
        cout << "matched " << names[input_token];
        if (input_token == t_id || input_token == t_literal)
            cout << ": " << token_image;
        cout << "\n";
        input_token = scan ();
    }
    else error ();
}

void program ();
void stmt_list ();
void stmt ();
void cond ();
void expr ();
void term_tail ();
void term ();
void factor_tail ();
void factor ();
void r_op ();
void add_op ();
void mul_op ();

void program () {
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            cout << "predict program --> stmt_list eof\n";
            stmt_list ();
            match (t_eof);
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
            match (t_gets);
            expr ();
            break;
        case t_read:
            cout << "predict stmt --> read id\n";
            match (t_read);
            match (t_id);
            break;
        case t_write:
            cout << "predict stmt --> write expr\n";
            match (t_write);
            expr ();
            break;
        case t_if:
            cout << "predict stmt --> if clause\n";
            match (t_if);
            cond ();
            stmt_list ();
            match (t_end);
            break;
        case t_while:
            cout << "predict stmt --> while clause\n";
            match (t_while);
            cond ();
            stmt_list ();
            match (t_end);
            break;
        default:
            while(){
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
}

void cond () {
    switch(input_token) {
        case t_lparen:
        case t_id:
        case t_literal:
            expr ();
            r_op ();
            expr ();
            break;
        default: error ();
    }
}

void expr () {
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict expr --> term term_tail\n";
            term ();
            term_tail ();
            break;
        default: error ();
    }
}

void term_tail () {
    switch (input_token) {
        case t_add:
        case t_sub:
            cout << "predict term_tail --> add_op term term_tail\n";
            add_op ();
            term ();
            term_tail ();
            break;
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

void term () {
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict term --> factor factor_tail\n";
            factor ();
            factor_tail ();
            break;
        default: error ();
    }
}

void factor_tail () {
    switch (input_token) {
        case t_mul:
        case t_div:
            cout << "predict factor_tail --> mul_op factor factor_tail\n";
            mul_op ();
            factor ();
            factor_tail ();
            break;
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

void factor () {
    switch (input_token) {
        case t_id :
            cout << "predict factor --> id\n";
            match (t_id);
            break;
        case t_literal:
            cout << "predict factor --> literal\n";
            match (t_literal);
            break;
        case t_lparen:
            cout << "predict factor --> lparen expr rparen\n";
            match (t_lparen);
            expr ();
            match (t_rparen);
            break;
        default: error ();
    }
}

void r_op () {
    switch (input_token) {
        case t_equals:
            cout << "predict r_op --> equals\n";
            match(t_equals);
            break;
        case t_not_equals:
            cout << "predict r_op --> not_equals\n";
            match(t_not_equals);
            break;
        case t_less:
            cout << "predict r_op --> less than\n";
            match(t_less);
            break;
        case t_greater:
            cout << "predict r_op --> greater than\n";
            match(t_greater);
            break;
        case t_less_equal:
            cout << "predict r_op --> less than or equal\n";
            match(t_less_equal);
            break;
        case t_greater_equal:
            cout << "predict r_op --> greater than or equal\n";
            match(t_greater_equal);
            break;
        default: error ();
            
    }
}

void add_op () {
    switch (input_token) {
        case t_add:
            cout << "predict add_op --> add\n";
            match (t_add);
            break;
        case t_sub:
            cout << "predict add_op --> sub\n";
            match (t_sub);
            break;
        default: error ();
    }
}

void mul_op () {
    switch (input_token) {
        case t_mul:
            cout << "predict mul_op --> mul\n";
            match (t_mul);
            break;
        case t_div:
            cout << "predict mul_op --> div\n";
            match (t_div);
            break;
        default: error ();
    }
}


int main () {
    input_token = scan ();
    program ();
}
