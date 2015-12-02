import subprocess
import sys
from subprocess import Popen, PIPE

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
while literals < 75:
	# always end on four times more clauses than literals
	while clauses < (literals * 4):
		# make random dimacs file
		subprocess.call(['./r_dimacs_gen', str(literals), str(clauses), str(seed)]) 
                minisat_out = subprocess.Popen(['minisat', '-verb=0', 'random_dimacs.txt'], stdout=PIPE)
                output_mini, err = minisat_out.communicate("some error message involving minisat")
#                print output
#                subprocess.check_call(['minisat', '-verb=0', 'random_dimacs.txt'])
		# capture minisat output
#		minisat_out = subprocess.check_output(["minisat", "-verb=0", "random_dimacs.txt"])
		#capture dpll_solver output
		#dpll_solver_out = subprocess.check_output(['./dpll_solver', 'random_dimacs.txt'])
                dpll_solver_out = subprocess.Popen(['./dpll_solver', 'random_dimacs.txt'], stdout=PIPE)
                output_dpll, err = dpll_solver_out.communicate("some error message involving our solver")
 #               print dpll_solver_out
		#compare the endings of both captured outputs
		if output_mini.endswith('UNSATISFIABLE\n'):
			if output_dpll.endswith('UNSATISFIABLE\n'): # if minisat and dpll_solver match as satisfiable
				solver_matches = solver_matches + 1
			elif output_dpll.endswith('SATISFIABLE\n'): # if minisat and dpll_solver do not match
				solver_mismatches = solver_mismatches + 1
			else:
				print 'error in dppl_solver output'
		elif output_mini.endswith('SATISFIABLE\n'):
			if output_dpll.endswith('SATISFIABLE\n'): # if minisat and dpll_solver match as unsatisfiable
				solver_matches = solver_matches + 1
			elif output_dpll.endswith('UNSATISFIABLE\n'): # if minisat and dpll_solver do not match
				solver_mismatches = solver_mismatches + 1
			else:	
				print 'error in dppl_solver output'
		else:
			print 'error in minisat ouput'

		#increment clauses and total solver runs
		clauses = clauses + 1
		solver_runs = solver_runs + 1
                #print str(clauses)
		#********** end of  incrementing clauses

	#increment literals, reset clauses
	literals = literals + 1
	clauses = 1;
        #print str(literals)

# now that iterative testing of dpll_solver is finished, report results

print str(solver_matches) + " matches."
print str(solver_mismatches) + " mismatches."
print str(solver_runs) + " runs."
print 'dpll_solver matched minisat ' + str((solver_matches/solver_runs)*100) + '% of the time.' 
#********* end of iteraivly testing dpll_solver against minisat


