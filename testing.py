#!/usr/bin/env python

import subprocess

solver_runs = 0
solver_matches = 0
solver_mismatches = 0

input_valid_runs = 0
input_matches = 0
input_mismatches = 0

literals = 1
clauses = 1
seed = 13

minisat_out = ""
dpll_solver_out = ""

# test the solver
while literals < 100:
	# always end on ten more clauses than literals
	while clauses < (literals + 10):
		# make random dimacs file
		subprocess.call(['./r_dimacs_gen', str(literals), str(clauses), str(seed)])  
	
		# capture minisat output
		minisat_out = subprocess.check_output(['minisat', '-verb=0', 'random_dimacs.txt'])

		#capture dpll_solver output
		dpll_solver_out = subprocess.check_output(['./dpll_solver', str(literals), str(clauses), str(seed)])

		#compare the endings of both captured outputs
		if minisat_out.endswith('SATISFIABLE'):
			if dpll_solver_out.endswith('SATISFIABLE'): # if minisat and dpll_solver match as satisfiable
				solver_matches = solver_matches + 1
			elif dpll_solver_out.endswith('UNSATISFIABLE'): # if minisat and dpll_solver do not match
				solver_mismatches = solver_mismatches + 1
			else:
				print 'error in dppl_solver output'
		elif minisat_out.endswith('UNSATISFIABLE'):
			if dpll_solver_out.endswith('UNSATISFIABLE'): # if minisat and dpll_solver match as unsatisfiable
				solver_matches = solver_matches + 1
			elif dpll_solver_out.endswith('SATISFIABLE'): # if minisat and dpll_solver do not match
				solver_mismatches = solver_mismatches + 1
			else:	
				print 'error in dppl_solver output'
		else:
			print 'error in minisat ouput'

		#increment clauses and total solver runs
		clauses = clauses + 1
		solver_runs = solver_runs + 1
		#********** end of  incrementing clauses

	#increment literals, reset clauses
	literals = literals + 1
	clauses = 1;


# now that iterative testing of dpll_solver is finished, report results

print 'dpll_solver matched minisat ' + str((solver_matches/solver_runs)) + '% of the time.' 
#********* end of iteraivly testing dpll_solver against minisat


