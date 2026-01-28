Documentation for Warmup Assignment 1
=====================================

+------------------------+
| BUILD & RUN (Required) |
+------------------------+

Replace "(Comments?)" below with the command the grader should use to compile
your program (it should simply be "make" or "make warmup1"; minor variation is
also fine).

    To compile your code, the grader should type: make warmup1

If you have additional instruction for the grader, replace "(Comments?)" with your
instruction (or with the word "none" if you don't have additional instructions):

    Additional instructions for building/running this assignment: after building, run follows spec i.e. ./warmup1 sort FILE or via stdin

+-------------------------+
| SELF-GRADING (Required) |
+-------------------------+

Replace each "?" below with a numeric value:

(A) Doubly-linked Circular List : 40 out of 40 pts

(B.1) Sort (file) : 30 out of 30 pts
(B.2) Sort (stdin) : 30 out of 30 pts

Missing required section(s) in README file : -0 pts
Submitted binary file : -0 pts
Cannot compile : -0 pts
Compiler warnings : -0 pts
"make clean" : -0 pts
Segmentation faults : -0 pts
Program never terminates : -0 pts
Separate compilation : -0 pts
Malformed input : -0 pts
Too slow : -0 pts
Bad commandline : -0 pts
Did not use My402List and My402ListElem to implement "sort" in (B) : -0 pts

+---------------------------------+
| BUGS / TESTS TO SKIP (Required) |
+---------------------------------+

Are there are any tests mentioned in the grading guidelines test suite that you
know that it's not working and you don't want the grader to run it at all so you
won't get extra deductions, please replace "(Comments?)" below with your list.
(Of course, if the grader won't run such tests in the plus points section, you
will not get plus points for them; if the garder won't run such tests in the
minus points section, you will lose all the points there.)  If there's nothing
the grader should skip, please replace "(Comments?)" with "none".

Please skip the following tests: none

+--------------------------------------------------------------------------------------------+
| ADDITIONAL INFORMATION FOR GRADER (Optional, but the grader should read what you add here) |
+--------------------------------------------------------------------------------------------+

+-----------------------------------------------+
| OTHER (Optional) - Not considered for grading |
+-----------------------------------------------+

Comments on design decisions:

For implementation of any numeric dollar amounts I went with an inplementation of cents rather than proceeding with floating point dollar amount, we learned this pattern in cs455x and wanted to implement this way as to avoid any unwanted and hard to track floating point arithmentic bugs.

Straigh forward implementation. handled errors with verbosity inside of functions and followed spec and grading guidelines for verbosity/ I maintained the my402list as sorted on every insert to avoid having to sort after insert (way too complex and wouldnt scale). Since we always start with an empty list, the list is always sorted and inserting into a sorted list is much easier.

I parse the input line by line after passing validity checks, since our formatting requirement is strict and values are tab seperated, i am able to parse out the correct values and save in data structire called Txn that i can then use to isnert into our list.

