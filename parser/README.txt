=== CSC 254 A2: Syntax Error Recovery ===
Chester Holtz - 28264729 - choltz2@u.rochester.edu
Lee Murphy - 28250920 - lmurp14@u.rochester.edu
    
In this assignment we where required to implement syntax error recovery and construct an ast for programs written in an extended calculator language.

== Files ==
README.txt - This file
run - bash script to pipe --debug option and file input to the program
scan.h - header file for scan.cpp
scan.cpp - scanner logic to tokenize a program and pass individual tokens to the parser. Also notifies the programmer of any invalid tokens present in their program.
parse.cpp - parser logic to construct the syntax tree from tokens passed by the scanner and recover from any errors
input.txt - sample input that results in the successful construction of the ast
err_input.txt - bad input for which our program recovers and provides useful error messages

== Description ==
This assignment tasked us with implimenting a syntax error recovery mechanism for an extended, turing complete calculator language. The calculator language we implimented this for supports ifs, whiles, assignment, and simple number and boolean operations. We where provided with the fundamental components of a scanner and parser for this language and where required to first extend the parser to include certain functionality, exception-based syntax error recovery to out put helpful error messages and to construct a syntax tree given a correct program. This assignment was completed in the C++ language.

As in the A1 language assignment, we represemt an abstract syntax tree as a recursive linear list of parenthesis where a node is denoted by a set of open parenthesis and its children are nested - i.e., the tree 

            a
          / | \
         b  c  d
        /|  |
       e f  g

would be represented by the string (a (b e f) (c g) d).

The LL(1) grammar representing our language is provided below, along with the partial first and follow sets used to complete the syntax recovery process.

GRAMMAR
P	→ 	SL $$
SL	→ 	S SL  |  ε
S	→ 	id := E  |  read id  |  write E  |  if C SL end  |  while C SL end
C	→ 	E ro E
E	→ 	T TT
T	→ 	F FT
F	→ 	( E )  |  id  |  lit
TT	→ 	ao T TT  |  ε
FT	→ 	mo F FT  |  ε
ro	→ 	==  |  !=  |  <  |  >  |  <=  |  >=
ao	→ 	+  |  -
mo	→ 	*  |  /


FIRST (partial)
SL  ε, id, read, write, if, while
S   id, read, write, if, while
F   (, id, lit
TT  ε, +, -
FT  ε, *, /
ro  ==, !=, <, >, <=, >=
ao  +, -
mo  *, /
T   (, id, lit
P   ε, id, read, write, if, while
E   (, id, lit
C   (, id, lit

FOLLOW (partial)
P   $$
SL  end, $$
S   ε, id, read, write, if, while
C   ε, id, read, write, if, while
E   ), ==, !=, <, >, <=, >=, ε, id, read, write, if, while
T   ε, +, -
F   ε, *, /
TT  ), ==, !=, <, >, <=, >=, ε, id, read, write, if, while
FT  ε, +, -
ro  (, id, lit
ao  (, id, lit
mo  (, id, lit

Instead of building our syntax tree as an explicit data structure stored in memory, we instead accumulate a string through the parse process. We solve the inherint scope problem a parse tree by constructing our tree bottom up and conglomerating subtrees at key points. 
We implimented a recursive solution to error recovery providing recovery and errors for unexpected eof, unknown tokens, and general programming errors. As we tokenize, we record the current position of the getc buffer in order to provide the programmer with an estimation of the column and line an arbitrary error occurs. This was done for extra credit.

== Exception Handling ==
A compiler will detect a syntax error immediately after the invalid token and exit with error, instead, we impliment a recovery system that prevents our compiler/interpreter from exiting and continues to parse the program to look for further errors - although evaluation at this point is impossible. The primary purpose of this system is to modify the input program as we parse and hopefully continue to produce useful error messages while avoiding spurious cascading errors that might cause the programmer to miss real ones. 
Exception-Based Error Recovery is an alternative to Wirth's technique and is quite simple to impliment. Essentially, we attach an exception handler to case branches that we want to recover from in the event of an error. We determine how to procede in the event of an error based on the first and follow sets of the currently parsed token. If the next token is in the first of the current, we throw out the current token and recursively call the current terminal block. If the next token is in the follow set, we post a useful error message to the console, notify the program that we no longer need to construct an ast for the program, and continue to parse. If the next token is in neither of the above sets, we return unexpected eof or break from the statement. 

== Instructions ==
In this project, we include a run script which takes either one or two arguments. The user can provide -d, or --debug to run the program in debug mode to print out a trace of predictions, matches, and the process of ast construction for the program. The user should always specify a file containing their program to interpret since inline interpretation forgoes the eof, which is mandatory for the construction of the ast.

== Example Output (good) ==
 » ./run input.txt 
make: 'parse' is up to date.
--------

(program (read n)(:= cp 2)(while (> n 0)((:= found 0)(:= cf1 2)(:= cf1s (* cf1 cf1))(while (<= cf1s cp)((:= cf2 2)(:= pr (* cf1 cf2))(while (<= pr cp)((if (== pr cp)(:= found 1))(:= cf2 (+ cf2 1))(:= pr (* cf1 cf2))))(:= cf1 (+ cf1 1))(:= cf1s (* cf1 cf1))))(if (== found 0)(write cp)(:= n (- n 1)))(:= cp (+ cp 1)))))

example trace block (--debug):
predict factor_tail --> epsilon
predict term_tail --> add_op term term_tail
predict add_op --> add
matched add
predict term --> factor factor_tail
predict factor --> literal
matched literal: 1
(program (read n)(:= cp 2)(while (> n 0)((:= found 0)(:= cf1 2)(:= cf1s (* cf1 cf1))(while (<= cf1s cp)((:= cf2 2)(:= pr (* cf1 cf2))(while (<= pr cp)((if (== pr cp)(:= found 1))(:= cf2 (+ cf2 1))(:= pr (* cf1 cf2))))(:= cf1 (+ cf1 1))(:= cf1s (* cf1 cf1))))(if (== found 0)(write cp)(:= n (- n 1)))(:= cp (+ cp 1)))))

The program provided in input is given by
read n
cp := 2
while n > 0
   found := 0
   cf1 := 2
   cf1s := cf1 * cf1
   while cf1s <= cp
       cf2 := 2
       pr := cf1 * cf2
       while pr <= cp
           if pr == cp
               found := 1
           end
           cf2 := cf2 + 1
           pr := cf1 * cf2
       end
       cf1 := cf1 + 1
       cf1s := cf1 * cf1
   end
   if found == 0
       write cp
       n := n - 1
   end
   cp := cp + 1
end

== Example Output (bad) ==
 » ./run err_input.txt
make: 'parse' is up to date.
--------
9:10 [Invalid Tok] expected '=' after ':' 
11:18 [Parse Err] Expected: gets, Received: mul
12:22 [Scan Err] Invalid token ';' before '
'
13:21 [Parse Err] Expected: gets, Received: id
20:7 [Scan Err] Invalid token '#' before 'f'
25:4 [Parse Err] Missing term
25:4 [Parse Err] Expected: end, Received: eof
25:4 [Parse Err] Expected: end, Received: eof

example trace block (--debug):
predict factor_tail --> epsilon
predict term_tail --> add_op term term_tail
predict add_op --> sub
matched sub
23:4 [Parse Err] Missing term
predict term_tail --> epsilon
predict stmt_list --> epsilon
23:4 [Parse Err] Expected: end, Received: eof
predict stmt_list --> epsilon
23:4 [Parse Err] Expected: end, Received: eof
predict stmt_list --> epsilon
matched eof

The program provided in input is given by
read n
cp := 2
while n > 0
   found := 0
   cf1 := 2
   cf1s := cf1 * cf1
   while cf1s <= cp
       cf2 := 2
       A :
       write 2 +
       pr := cf1 * cf2
       while pr <= cp*;
           if pr == cp
               found := 1
           end
           cf2 := cf2 + 1
           pr := cf1 * cf2
       end
       cf1 := cf1 + 1
       cf1s := cf1 * cf1
   if #found == 0
       write cp
       n := n - 1 
   cp := cp + 1-
end
