dpll_solver: dpll_solver.c
	gcc -o dpll_solver dpll_solver.c

clean: 
	rm dpll_solver
	rm a.out
	rm ./*~
