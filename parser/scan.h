/* definitions the scanner shares with the parser */

enum token {t_read, t_write, t_while, t_if, t_end, t_id, t_literal, t_gets,
                t_add, t_sub, t_mul, t_div, t_lparen, t_rparen, t_eof};

extern char token_image[100];

extern token scan();
