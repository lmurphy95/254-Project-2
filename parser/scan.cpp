/* Simple ad-hoc scanner for the calculator language.
*/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <ctype.h>
#include "scan.h"

using namespace std;

char token_image[100];
int cur_line = 1;
int cur_col = 1;

token scan() {
    static int c = ' ';
    static int cc = ' '; // lookahead char
    /* next available char; extra (int) width accommodates EOF */
    int i = 0; /* index into token_image */

    /* skip white space */
    if(c == '\n') {
        cur_col = 1;
        cur_line ++;
        c = getchar();
    }
    while (isspace(c)) {
        c = getchar();
        cur_col ++;
    }
    if (c == EOF)
        return t_eof;

    if (isalpha(c)) {
        do {
            token_image[i++] = c;
            c = getchar();
            cur_col++;
        } while (isalpha(c) || isdigit(c) || c == '_');
        token_image[i] = '\0';
        if (!strcmp(token_image, "read")) return t_read;
        else if (!strcmp(token_image, "write")) return t_write;
        else if (!strcmp(token_image,"while")) return t_while;
        else if (!strcmp(token_image,"if")) return t_if;
        else if (!strcmp(token_image,"end")) return t_end;
        else return t_id;
    }
    else if (isdigit(c)) {
        do {
            token_image[i++] = c;
            c = getchar();
            cur_col++;
        } while (isdigit(c));
        token_image[i] = '\0';
        return t_literal;
    } else switch (c) {
        case ':':
            if ((c = getchar()) != '=') {
                cout << cur_line << ':' << cur_col  << " [Invalid Tok]" << " expected \'=\' after \':\' \n";
                return (t_gets);
                //exit(1);
            } else {
                c = getchar();
                cur_col ++;
                return t_gets;
            }
            break;
        case '+': c = getchar(); cur_col ++; return t_add;
        case '-': c = getchar(); cur_col ++; return t_sub;
        case '*': c = getchar(); cur_col ++; return t_mul;
        case '/': c = getchar(); cur_col ++; return t_div;
        case '=':
            if ((c = getchar()) == '=') {
                c = getchar();
                cur_col ++;
                return t_equals;
            } else {
                c = getchar();
                cur_col ++;
            }
            break;
        case '!':
            if ((c = getchar()) != '=') {
                cout << cur_line << ':' << cur_col << "[Invalid Tok]" << " expected \'=\' after \'!\' \n";
                return t_not_equals;
                //exit(1);
            } else {
                c = getchar();
                cur_col ++;
                return t_not_equals;
            }
            break;
        case '<':
            if ((c = getchar()) == '=') {
                c = getchar();
                cur_col ++;
                return t_less_equal;
            } else {
                c = getchar();
                cur_col ++;
                return t_less;
            }
            break;
        case '>':
            if ((c = getchar()) == '=') {
                c = getchar();
                cur_col ++;
                return t_greater_equal;
            } else {
                c = getchar();
                cur_col ++;
                return t_greater;
            }
            break;
        case '(': c = getchar(); cur_col ++; return t_lparen;
        case ')': c = getchar(); cur_col ++; return t_rparen;
        default: // fix unexpected token loop before eof
            cc = getchar();
            cout << cur_line << ':' << cur_col << " [Scan Err]" << " Invalid token " << '\'' << (char)c << '\'' << " before " << '\'' << (char)cc << '\'' << "\n";
            err = true;
            c = getchar();
            cur_col ++;
            return scan();
    }
}