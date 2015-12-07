TEAM COMPUTAHS

Jon Curry | Mike Fleming | AJ Johnson | Ben Naugle

Nov 2015

CS 5959 - Writing Solid Code

This sat solver is designed to work with dimacs cnf files, determinig if they are "SATISFIABLE" or "UNSATISFIABLE".

Running the "make" command will create an executable "dpll_solver", which can then be executed with the command "./dpll_solver FILE_NAME", where FILE_NAME is the input dimacs file. Additionally, a "r_dimacs_gen" executable will be created to allow users to generate random dimacs files of varying amounts of literals and clauses. Currently, the Makefile is using the file dpll_solver_v2.c, which is our most updated version, but using the command "make_alt" will use our alternate dpll_solver.c file with the executable "dpll_solver"

Testing:

Testing involved running larger and larger randomly generated cnf files against minisat for correctness via the testing.py script. Also, we tested timing against minisat for some medium sized (e.g. 2300 literals and 2300 clauses) randomly generated cnf files. We also made a point of testing for larger files that were unsatisfiable. 
Randomly generated files were made with the r_dimacs_gen executable, which takes in the number of desired litrerals, clauses, and finally the random seed.
We also testing for improperly formatted cnf files with some included files in the test directory.
