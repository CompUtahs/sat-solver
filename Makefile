all: dpll_solver_v2.c input_verifier.c r_dimacs_gen.c
	gcc -o dpll_solver dpll_solver_v2.c
	gcc -o r_dimacs_gen r_dimacs_gen.c

dpll_solver: dpll_solver_v2.c
	gcc -o -O1 dpll_solver dpll_solver_v2.c

gen: r_dimacs_gen.c
	gcc -o r_dimacs_gen r_dimacs_gen.c

clean: 
	rm r_dimacs_gen
	rm dpll_solver
	rm a.out
	rm ./*~
