all: src/dpll_solver_v2.c src/input_verifier.c test/r_dimacs_gen.c
	gcc -O3 -o dpll_solver src/dpll_solver_v2.c
	gcc -o r_dimacs_gen test/r_dimacs_gen.c

dpll_solver: src/dpll_solver_v2.c src/input_verifier.c
	gcc -O3 -o dpll_solver src/dpll_solver_v2.c

alt: src/dpll_solver.c src/input_verifier.c
	gcc -o dpll_solver src/dpll_solver.c

gen: test/r_dimacs_gen.c
	gcc -o r_dimacs_gen test/r_dimacs_gen.c

clean: 
	rm r_dimacs_gen
	rm dpll_solver
	rm a.out
	rm ./*~
