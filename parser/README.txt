=== CSC 254 A2: Syntax Error Recovery ===
Chester Holtz - 28264729 - choltz2@u.rochester.edu
Lee Murphy - - lmurp14@u.rochester.edu
    
In this assignment we where required to implement syntax error recovery for an extended calculator language.

== Description ==

This assignment tasked us with implimenting a syntax error recovery mechanism for an extended calculator language. The language we implimented this for supports ifs, whiles, assignment, and simple number and boolean operations. We where provided with the fundamental components of a scanner and parser for this language and where required to first extend the parser to include certain functionality, exception-based syntax error recovery to out put helpful error messages and to construct a syntax tree given a correct program. This assignment was completed in the C++ language.

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
P   $
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

== Instructions ==
In this project, we include a run script which takes either one or two arguments. The user can provide -d, or --debug to run the program in debug mode to print out a trace of predictions, matches, and process of ast construction for the program. The user should always specify a file containing their program to interpret.

== Example Output (n-primes) ==
 » ./run input.txt 
make: 'parse' is up to date.
--------

(program (read n)(:= cp 2)(while (> n 0)((:= found 0)(:= cf1 2)(:= cf1s (* cf1 cf1))(while (<= cf1s cp)((:= cf2 2)(:= pr (* cf1 cf2))(while (<= pr cp)((if (== pr cp)(:= found 1))(:= cf2 (+ cf2 1))(:= pr (* cf1 cf2))))(:= cf1 (+ cf1 1))(:= cf1s (* cf1 cf1))))(if (== found 0)(write cp)(:= n (- n 1)))(:= cp (+ cp 1)))))

example trace block:
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
$$

and when we expand the output, we get the associted syntax tree:
(program 
	(read n)
	(:= cp 2)
	(while (> n 0)(
		(:= found 0)
		(:= cf1 2)
		(:= cf1s 
			(* cf1 cf1)
		)
		(while (<= cf1s cp)(
			(:= cf2 2)
			(:= pr 
				(* cf1 cf2)
			)
			(while (<= pr cp)(
				(if (== pr cp)
					(:= found 1)
				)
				(:= cf2 
					(+ cf2 1)
				)
				(:= pr 
					(* cf1 cf2)
				)
			)
			)
			(:= cf1 
				(+ cf1 1)
			)
			(:= cf1s 
				(* cf1 cf1)
			)
		)
		)
		(if 
			(== found 0)
			(write cp)
			(:= n 
				(- n 1)
			)
		)
		(:= cp 
			(+ cp 1)
		)
		)
	)
)
