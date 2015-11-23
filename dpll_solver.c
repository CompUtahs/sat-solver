#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dpll_structs.h"
#include "input_verifier.c"

/*Predelcared methods*/
formula assign_val(int val, int index, formula f);
formula unit_prop(clause c, formula f);
formula pure_assign(literal lit, formula f);
int has_empty(formula f);
int is_unit_clause(clause c);
int is_pure(int id, formula f);
int evaluate(formula f);
void print_formula(formula f);
void print_lits(formula f);

/* Old globals from when we had a hardcoded forumla - probably can be removed
formula f1;
int num_lits;
int satisfiable = 0; 
*/

/* returns the evaluation of a literal's is_pos & val 
* 0 0 = 1
* 0 1 = 0
* 1 0 = 0
* 1 1 = 1
*/
int get_eval(val, is_pos)
{
		int eval = -1;
		if(val == 1)
		{
			if(is_pos == 1)
				eval = 1;
			else
				eval = 0;
		}
		else
		{
			if(is_pos == 1)
				eval = 0;
			else
				eval = 1;
		}
		return eval;
}

/* Loops through the formula and checks if a literal has the same sign throughout 
* returns 1 for an all pure positive literal
* returns -1 for an all pure negative literal
* returns 0 for a non pure literal
*/
int is_pure(int id, formula f)
{
	int has_pos = 0;
  int has_neg = 0;

	int i, j;
	for(i = 0; i < f.num_clauses; i++)
	{
		if (f.clauses[i].is_satisfied == 1)
			continue;
		// loop through all the literals of each clause
		for(j = 0; j < f.clauses[i].len; j++)
		{
			if(f.clauses[i].lits[j].id == id)
			{
					if(f.clauses[i].lits[j].is_pos)	
						has_pos = 1;
					else
						has_neg = 1;

					if(has_pos && has_neg)
						return 0;
			}
		}
	}
	if(has_pos)
		return 1;	
	else
		return -1;
}

/* Checks that all of the remaining literals are pure 
* breaks when it finds a non pure literal
* if so it will return true
* else false
*
* We check this at the start of every DPLL call to check 
* the remaining literals for purity
*/
int is_consistent(formula f)
{	
	int is_con = 1;
	int i;
	for(i = 0; i < f.num_lits; i++)
	{
		if (f.all_lits[i].is_assigned == 1)
			continue;
		is_con = is_pure(f.all_lits[i].id, f);
		if(is_con == 0)
			return 0;
	}
	return 1;
}

/* Checks the remaining for any clauses that cause an Unsatisfied formula
* Breaks on the first unassigned variable it finds
* If all variables are assigned return 1?
* This method may need work could possibly be not working.
*/
int has_empty(formula f)
{
	int i,j;
	for(i = 0; i < f.num_clauses; i++)
	{
		int all_assigned = 1;
		if(f.clauses[i].is_satisfied)
			continue;

		for(j = 0; j< f.clauses[i].len; j++)
		{
			if (!(f.clauses[i].lits[j].is_assigned))
			{
				all_assigned = 0;
				break;
			}
		}
		if(all_assigned == 1)
			return 1;
	}	
	return 0;	
}

/* If a pure variable it found set all of it's instances to true
* Takes the literal to assign and should have correct sign 
*/
formula pure_assign(literal lit, formula f)
{
	int i;
	int val;
	if(lit.is_pos == 1)
		val = 1;
	else
		val = 0;

	for(i = 0; i < f.num_lits; i++)
	{
		if(f.all_lits[i].id == lit.id)
		{
			break;
		}
	}	
	assert(i != (f.num_lits +1));
	f = assign_val(val, i, f);
	return f;
}

/* Checks for unit clauses meaning 1 unassigned variable in the entire clause
* continues on statisfied clause
* counts unassigned variables breaks more than 1
* if it is a unit clause propagates that literal through all of its clauses
*/
int is_unit_clause(clause c)
{
	if(c.is_satisfied)
	{
//printf("c is satisfied\n");
		return 0;
	}
//printf("c is unsatified\n");
	
	int i;
	int unassigned_count = 0;
	for(i = 0; i < c.len; i++)
	{
		
		if(c.lits[i].is_assigned == 0)
			unassigned_count++;
		
		if(unassigned_count > 1)
		  return 0; 
	}
	
	if(unassigned_count == 1)
		return 1;
	else
		return 0;
}

/* Propegates the variable that makes this clause true 
*	through all of its clauses making that true.
*/
formula unit_prop(clause c, formula f)
{

	int i,k,val,index;
	for(k = 0; k < c.len; k++)
	{
		if(c.lits[k].is_assigned == 0)
			break;
	}

	if(c.lits[k].is_pos == 1)
		val = 1;
	else
		val = 0;

	for(i = 0; i < f.num_lits; i++)
	{
		if(f.all_lits[i].id == c.lits[k].id)
		{
			index = i;
			break;
		}
	}

	f = assign_val(val, index, f);
//printf("unit prop returning -----------\n");

	return f;

}

//int run_count =0;
/* Main running loop follows the DPLL Algo on Wiki */
int DPLL(formula f)
{

//run_count ++;


//printf("\n\nDPLL run count %d\n",  run_count);		
//print_formula(f);


	int i, j, next_lit, all_assigned = 0;
	//check if the remaining variables are consistent	
	if(is_consistent(f))
		return 1;

	//check for an unsatisfiable clause aka an empty clause
	if(has_empty(f))
		return 0;

	//unit prop	
	for(i = 0; i < f.num_clauses; i++)
	{

		if(is_unit_clause(f.clauses[i]))
		{
			f = unit_prop(f.clauses[i], f);
			i = 0;
//print_formula(f);
//printf("Back in Main after unit prop\n");
		}
		if(has_empty(f))
			return 0;
	}
	if (evaluate(f))
		return 1;

	//get pures
	for(i = 0; i < f.num_lits; i++)
	{
		if (f.all_lits[i].is_assigned == 1)
			continue;
		int check = is_pure(f.all_lits[i].id, f);
		if(check)
		{	
			if (check > 0)
				f.all_lits[i].is_pos = 1;
			else
				f.all_lits[i].is_pos = 0;

			f = pure_assign(f.all_lits[i], f);
//printf("Back in Main after unit pure assign\n");
		}
		if (evaluate(f))
			return 1;
	}

	//choose next literal
	for(i = 0; i < f.num_lits; i++)
	{
//printf("%d is assigned: %d\n",f.all_lits[i].id,f.all_lits[i].is_assigned);
		if(!(f.all_lits[i].is_assigned))
		{
			next_lit = i;
			all_assigned = 0;
			break;
		}
		else
		{	
			all_assigned = 1;
		}
	}
	
	if(all_assigned == 1)
		return evaluate(f);
	
	
	
//	int run_stamp = run_count;
	
	//branch true
	formula branch_t = f;	
	branch_t = assign_val(1,next_lit,branch_t);
//printf("calling t_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
//printf("returned from t_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
	
	int result = DPLL(branch_t);
	int id;

	//unassign variables from the other branch below this next lit -- this is our backtracking
	for(i = next_lit; i < f.num_lits; i++)
	{
		f.all_lits[i].is_assigned = 0;
		
		for(j = 0; j < f.num_clauses; j++)
		{
			f.clauses[j].is_satisfied = 0;
			int k;
			for(k = 0; k < f.clauses[j].len; k++)
			{
				if(f.clauses[j].lits[k].id == f.all_lits[i].id)
					f.clauses[j].lits[k].is_assigned = 0;
			}
		}	

	}
	// branch false
	formula branch_f = f;
	branch_f = assign_val(0,next_lit,branch_f);

//	printf("calling f_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
	
	result += DPLL(branch_f);
	
//	printf("returned from f_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
	return result;
}

/* Assign a true or false value to a literal
* Goes through all of the literals and sets the value
*/
formula assign_val(int val, int index, formula f)
{
//printf("setting %d to val %d\n", f.all_lits[index].id, val);
	int i,j;
	for(i = 0; i < f.num_clauses; i++)
	{
		for(j = 0; j < f.clauses[i].len; j++)
		{
			if(f.clauses[i].lits[j].id == f.all_lits[index].id)
			{
				f.clauses[i].lits[j].val = val;
				f.clauses[i].lits[j].eval = get_eval(val, f.clauses[i].lits[j].is_pos);	
				f.clauses[i].lits[j].is_assigned = 1;
				if(f.clauses[i].lits[j].eval == 1)
					f.clauses[i].is_satisfied = 1;
				
//printf("eval to: %d\n", f.clauses[i].lits[j].eval);	
			}
		}
	}
	f.all_lits[index].is_assigned = 1;
//print_formula(f);
//print_lits(f);
//printf("assign val returning -----------\n");

	return f;
}	

int evaluate(formula f)
{
	int i;

	for(i = 0; i < f.num_clauses; i ++)
	{	
		if (!(f.clauses[i].is_satisfied == 1))
				return 0;
	}	
	return 1;

}	

void print_formula(formula current_formula)
{
	int j,i;		

	for (j = 0; j < current_formula.num_clauses; j++)
	{
		printf("%d   ||", current_formula.clauses[j].is_satisfied);	
		for (i = 0; i < current_formula.clauses[j].len; i++){
			// print all literals in a clause
			if(!current_formula.clauses[j].lits[i].is_pos)
				printf("-");
			
			printf("%d", current_formula.clauses[j].lits[i].id);
			int k;
			int temp;
			for(k = 0; k < current_formula.num_lits; k++)
			{
				if(current_formula.all_lits[k].id == current_formula.clauses[j].lits[i].id)
					temp =current_formula.all_lits[k].is_assigned;
			}
			if(temp)
			{
			
				if(current_formula.clauses[j].lits[i].eval == 1)
					printf("T");
				else
					printf("F");
			}
			else
				printf("u");
					
			printf(" ");
		}
		printf("\n");
	}


}
/* Debugging prints*/
void print_lits(formula f)
{
	int i,j;
	for(i = 0; i < f.num_clauses; i++)
	{	
		printf("clause %d  ----------------\n", i);
		for(j = 0; j < f.clauses[i].len; j++)
		{
			printf("%i id\n", f.clauses[i].lits[j].id);
			printf("\t%i is_assigned\n", f.clauses[i].lits[j].is_assigned);
			printf("\t%i val\n",f.clauses[i].lits[j].val);
			printf("\t%i is_pos\n",f.clauses[i].lits[j].is_pos);
			printf("\t%i eval\n",f.clauses[i].lits[j].eval);
		}
	}
}


int main(int argc, char *argv[])
{
	int i;	
	int err_val = 0;
	formula f9 = verify(argc,argv,&err_val);
	if(err_val != 0)
	{
		printf("ERROR\n");
		return 0;
	}

//	printf("%d\n",DPLL(f9));
	if(DPLL(f9))
		printf("SATISFIABLE\n");
	else
		printf("UNSATISFIABLE\n");

	annhialate_formula(f9);
	return 0;
}
