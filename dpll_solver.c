#include <stdlib.h>
#include <stdio.h>


typedef struct literal{
	//Variable read from file
	int id;
	//Is positive if true 
	int is_pos;
	int is_assigned;
	int val;
	int eval;
}literal;

typedef struct clause{
	int len;
	literal *lits;
	int is_satisfied;
}clause;

typedef struct formula{
	int num_clauses;
	clause *clauses;
	int num_lits;
	literal *all_lits;
}formula;


formula assign_val(int val, int index, formula f);
int has_empty(formula f);
int is_unit_clause(clause c);
formula unit_prop(literal lit, formula f);
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

	// TODO can bail early if we keep a count of appearences per literal
	// loop through all the clause of the formula
	int i;
	int j;
	for(i = 0; i < f.num_clauses; i++)
	{
		// loop through all the literals of each clause
		clause temp = f.clauses[i];
		for(j = 0; j < temp.len; j++)
		{
			if(temp.lits[j].id == id)
			{
					if(temp.lits[j].is_pos)	
						has_pos = 1;
					else
						has_neg = 1;

					if(has_pos && has_neg)
						return 0;
			}
		}
	}
	return 1;	
}

int is_consistent(formula f)
{	
	int is_con = 1;
	int i;
	for(i = 0; i < f.num_lits; i++)
	{
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
		for(j = 0; j< f.clauses[i].len; j++)
		{
	//		if(f.clauses[i].is_satisfied)
	//			continue;

			if(!(f.clauses[i].lits[j].is_assigned
				&& f.clauses[i].lits[j].eval == 0))
				all_assigned = 0;
					
		}
		if(all_assigned == 1)
			return 1;
	}	
	return 0;	
}

formula pure_assign(literal lit, formula f)
{
	int val;
	if(lit.is_pos == 1)
		val = 1;
	else
		val = 0;

	int i,j;
	for(i = 0; i < f.num_clauses; i++)
	{
		for(j = 0; j < f.clauses[i].len; j++)
		{

			if(f.clauses[i].lits[j].id == lit.id)
			{
				f.clauses[i].is_satisfied = 1;
				f.clauses[i].lits[j].is_assigned = 1;
				f.clauses[i].lits[j].val = val;
				f.clauses[i].lits[j].eval = 1;
			}
		}
		
	}

	printf("pure assign returning ^^^^^^^^^^^\n");

		//print_lits(f);
	return f;
}

int is_unit_clause(clause c)
{
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
formula unit_prop(literal lit, formula f)
{
	int val;
	if(lit.is_pos == 1)
		val = 1;
	else
		val = 0;

	int i,j;
	for(i = 0; i < f.num_clauses; i++)
	{
		for(j = 0; j < f.clauses[i].len; j++)
		{
			if(f.clauses[i].lits[j].id == lit.id)
			{
				f.clauses[i].lits[j].is_assigned = 1;
				f.clauses[i].lits[j].val = val;
				f.clauses[i].lits[j].eval = get_eval(val,f.clauses[i].lits[j].is_pos);
				}
		}
		
		
	}
	printf("unit prop returning -----------\n");

		//print_lits(f);
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


	int i;

//	if(is_consistent(f))
//		return 1;
	
//	if(has_empty(f))
//		return 0;
/*	
	for(i = 0; i < f.num_clauses; i++)
	{

		if(is_unit_clause(f.clauses[i]))
		{	
			f = unit_prop(f.clauses[i].lits[i], f);
		
			printf("Back in Main after unit prop\n");
			print_lits(f);
		}
	}
	
	//get pures
	for(i = 0; i < f.num_lits; i++)
	{
		if(is_pure(f.all_lits[i].id, f))
		{
			f = pure_assign(f.all_lits[i], f);
	

			printf("Back in Main after unit pure assign\n");
		print_lits(f);
		}
	}
*/
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
	
	
//	formula branch_t;
//	printf(" \t\t\t\t\t\t %d\n", f.num_clauses);
//	branch_t  = f;// make_copy(f);
//	print_formula(branch_t);
//		branch_t = assign_val(1,next_lit,f);
	//	printf("\t\t--------- branch t %d\n", f.all_lits[next_lit].id);
		//print_lits(branch_t);
	
	//int temp = DPLL(branch_t);
	
	//f.all_lits[next_lit].is_assigned = 0;	

//	formula branch_f;
//	branch_f = f; // make_copy(f);
//	branch_f  = assign_val(0,next_lit,f);
	//	printf("\t\t--------- branch f %d \n", f.all_lits[next_lit].id);
		//print_lits(branch_f);
	

//	temp += DPLL(branch_f);
//	return temp;
//	return (DPLL(branch_t) | DPLL(branch_f));
	
	int run_stamp = run_count;
	formula branch_t = f;	
	branch_t = assign_val(1,next_lit,branch_t);
	printf("calling t_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
	int temp = DPLL(branch_t);
	printf("returned from t_branch %d run %d\n", f.all_lits[next_lit].id, run_stamp);
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
		
				
				printf("set to: %d\n ", f.clauses[i].lits[j].eval);	
			}
		}
	}
	f.all_lits[index].is_assigned = 1;
	print_formula(f);
	printf("assign val returning -----------\n");

	return f;
}	

int evaluate(formula f)
{
	

	print_formula(f);

	int i,j;
	int is_true;
	int all_true = 1;
	for(i = 0; i < f.num_clauses; i ++)
	{
		is_true = 0;
		for(j = 0; j < f.clauses[i].len; j++)
		{
			if(f.clauses[i].lits[j].eval == 1)
				is_true = 1;
			 
		}
		if(is_true == 0)
			all_true = 0;

		
	}
	if(all_true == 1)
	{
		printf("-----   \t\t\t\t TRUE \n");
		satisfiable = 1;
	}
printf("returning %d from evaluate\n", all_true);
	return all_true;
}


void print_formula(formula current_formula)
{
	int j,i;		
	for (j = 0; j < current_formula.num_clauses; j++){
		
		for (i = 0; i < current_formula.clauses[j].len; i++){
			// print all literals in a clause
			if(!current_formula.clauses[j].lits[i].is_pos)
				printf("-");
			
			printf("%d", current_formula.clauses[j].lits[i].id);
			if(current_formula.clauses[j].lits[i].is_assigned)
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
	literal a;
	a.id = 1;
	a.is_pos = 1;
	
	literal b;
	b.id = 5;
	b.is_pos = 1;
	
	literal c;
	c.id = 4;
	c.is_pos = 0;

	clause first;
	first.len = 3;
	literal d[3];
	d[0] = a;
	d[1] = b;
  d[2] = c;
	first.lits = d;
	
	literal aa;
	aa.id = 1;
	aa.is_pos = 0;
	
	literal bb;
	bb.id = 5;
	bb.is_pos = 0;
	
	literal cc;
	cc.id = 3;
	cc.is_pos = 0;

	literal bba;
	bba.id = 4;
	bba.is_pos = 0;
	
	clause second;
	second.len = 4;
	literal dd[4];
	dd[0] = aa;
	dd[1] = bb;
  dd[2] = cc;
	dd[3] = bba;
	second.lits = dd;
	
	literal aaa;
	aaa.id = 3;
	aaa.is_pos = 1;
	
	literal bbb;
	bbb.id = 4;
	bbb.is_pos = 1;
	
	clause third;
	third.len = 2;
	literal ddd[2];
	ddd[0] = aaa;
	ddd[1] = bbb;
	third.lits = ddd;
	
	
	f1.num_clauses = 3;
	clause all[3];
	all[0] = first;
	all[1] = second;
	all[2] = third;
	
	f1.clauses = all;
	literal lits[4];
	literal xa;
	xa.id = 1;
	lits[0] = xa;;
	
	literal xb;
	xb.id = 3;
	lits[1] = xb;
		
	literal xc;
	xc.id = 4;
	lits[2] = xc;
	
	literal xd;
	xd.id = 5;
	lits[3] = xd;
	f1.all_lits = lits;
	int i,j;

	f1.num_lits = 5;	

	literal alpha;
	alpha.id = 1;
	alpha.is_pos = 1;

	literal beta;
	beta.id = 2;
	beta.is_pos = 0;

	clause c1;
	c1.len = 2;
	literal c1lits[2];
	c1lits[0] = alpha;
	c1lits[1] = beta;
	c1.lits = c1lits;
	
	literal alpha1;
	alpha1.id = 1;
	alpha1.is_pos = 1;

	literal beta1;
	beta1.id = 2;
	beta1.is_pos = 1;

	clause c11;
	c11.len = 2;
	literal c11lits[2];
	c11lits[0] = alpha1;
	c11lits[1] = beta1;
	c11.lits = c11lits;

	formula f2;
	f2.num_lits = 2;
	f2.num_clauses = 2;
	clause ccs[2];
	ccs[0] = c1;
	ccs[1] = c11;
	f2.clauses = ccs;
	literal litsf[2];
	litsf[0] = alpha;
	litsf[1] = beta;
	f2.all_lits = litsf;


	literal alphaz;
	alphaz.id = 1;
	alphaz.is_pos = 1;


	clause c1z;
	c1z.len = 1;
	literal c1litsz[1];
	c1litsz[0] = alphaz;
	c1z.lits = c1litsz;
	

	formula f3;
	f3.num_lits = 1;
	f3.num_clauses = 1;
	clause ccsz[1];
	ccsz[0] = c1z;
	f3.clauses = ccsz;
	literal litsfz[1];
	litsfz[0] = alphaz;
	f3.all_lits = litsfz;


	DPLL(f2);
	printf("\n%d\n", satisfiable);
}
