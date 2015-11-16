#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dpll_structs.h"
#include "input_verifier.c"


formula assign_val(int val, int index, formula f);
int has_empty(formula f);
int is_unit_clause(clause c);
formula unit_prop(clause c, formula f);
formula pure_assign(literal lit, formula f); 
int evaluate(formula f);
void print_formula(formula f);
void print_lits(formula f);

formula f1;
int num_lits;
int satisfiable = 0; 
// a pure literal has a consistent polarity in all instances

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
formula make_copy(formula f)
{
	
	printf("here %d", f.num_clauses);
	int i,j;
	clause clauses[f.num_clauses];
	printf("here");
	for(i = 0;i < f.num_clauses; i++)
	{
		clause c;
		literal lits[f.clauses[i].len];
		for(j = 0; j < f.clauses[i].len; j++)
		{
			literal a;
			a.id = f.clauses[i].lits[j].id;

			a.is_assigned = f.clauses[i].lits[j].is_assigned;
			a.eval = f.clauses[i].lits[j].eval;
			a.val = f.clauses[i].lits[j].val;
			a.is_pos = f.clauses[i].lits[j].is_pos;
			lits[j] = a;

	printf("here");
		}
		
		c.lits = lits;

		clauses[i] = c;
	
	printf("here");
	}
	
	formula result;
	result.num_clauses = f.num_clauses;
	result.num_lits = f.num_lits;
	result.clauses = clauses;
	result.all_lits = f.all_lits;

	printf("there");
	return result;
}

int is_pure(int id, formula f)
{
	int pure = 0; //boolean set to false
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

int is_unit_clause(clause c)
{
	if(c.is_satisfied)
	{
printf("c is satisfied\n");
		return 0;
	}
printf("c is unsatified\n");
	
	int i;
	int unassigned_count = 0;
	for(i = 0; i < c.len; i++)
	{
		
		if(c.lits[i].is_assigned == 0)
			unassigned_count++;
		
		if(unassigned_count > 1)
		  return 0; 


printf("c's unassigned count %d\n", unassigned_count);
	}
	
	if(unassigned_count == 1)
		return 1;
	else
		return 0;
}
formula unit_prop(clause c, formula f)
{

	int k;
	for(k = 0; k < c.len; k++)
	{
		if(c.lits[k].is_assigned == 0)
			break;
	}

printf("%d id\t %d is pos\n", c.lits[k].id, c.lits[k].is_pos);


	int val;
	if(c.lits[k].is_pos == 1)
		val = 1;
	else
		val = 0;

	int index,i;
	for(i = 0; i < f.num_lits; i++)
	{
		if(f.all_lits[i].id == c.lits[k].id)
		{
			index = i;
			break;
		}
	}

	f = assign_val(val, index, f);
	printf("unit prop returning -----------\n");

	return f;

}

/*BENBS ADDED GLOBAL OF CRAP*/
int run_count =0;

int DPLL(formula f)
{

	run_count ++;


	//int x = evaluate(f);
	printf("\n\nDPLL run count %d\n",  run_count);		

	print_formula(f);


	int i,j;

	if(is_consistent(f))
		return 13;
	
	if(has_empty(f))
		return 0;
	
	for(i = 0; i < f.num_clauses; i++)
	{

		if(is_unit_clause(f.clauses[i]))
		{
				
			f = unit_prop(f.clauses[i], f);
		
			print_formula(f);
			printf("Back in Main after unit prop\n");
			i = 0;
		}
		if(has_empty(f))
			return 0;

	}
	if (evaluate(f))
		return 11;

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
			printf("Back in Main after unit pure assign\n");
		}
		if (evaluate(f))
			return 7;
	}
	

	int next_lit;
	int all_assigned = 0;
	
	for(i = 0; i < f.num_lits; i++)
	{
		printf("%d is assigned: %d\n",f.all_lits[i].id,f.all_lits[i].is_assigned);
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
	
	
	
	int run_stamp = run_count;
	
	//branch true
	formula branch_t = f;	
	branch_t = assign_val(1,next_lit,branch_t);
	printf("calling t_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
	int temp = DPLL(branch_t);

	printf("returned from t_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
	
	int id;
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

	printf("calling f_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
	
	temp += DPLL(branch_f);
	
	printf("returned from f_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
	return temp;
}

formula assign_val(int val, int index, formula f)
{
	printf("setting %d to val %d\n", f.all_lits[index].id, val);
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
				
				printf("eval to: %d\n", f.clauses[i].lits[j].eval);	
			}
		}
	}
	f.all_lits[index].is_assigned = 1;
	print_formula(f);
	//print_lits(f);
	printf("assign val returning -----------\n");

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
		printf("ERROR");
		return 0;
	}

//	printf("%d\n",DPLL(f9));
	if(DPLL(f9))
		printf("SATISFIABLE");
	else
		printf("UNSATISFIABLE");

	return 0;
}
