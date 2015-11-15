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

// a pure literal has a consistent polarity in all instances
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

		print_lits(f);
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
				if(f.clauses[i].lits[j].is_pos == 1 && val == 0)
					f.clauses[i].lits[j].eval = 0;
				else
					f.clauses[i].lits[j].eval = 1;
				}
		}
		
	}
	printf("unit prop returning -----------\n");

		print_lits(f);
	return f;
}
int DPLL(formula f)
{
	print_formula(f);
	printf("^^^^^^^^^^ \t\t DPLL ^^^^YY");
	

	print_lits(f);
	printf("^^^^^^^^^^ \t\t DPLL ^^^^YY");
	if(is_consistent(f))
		return 1;
	
	if(has_empty(f))
		return 0;
	
	int i;
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

	int next_lit;
	int all_assigned = 0;
	for(i = 0; i < f.num_lits; i++)
	{
		if(!(f.all_lits[i].is_assigned))
		{
			next_lit = i;
			all_assigned = 0;
			break;
		}
		else
			all_assigned = 1;
	}	
	if(all_assigned == 1)
		return evaluate(f);
	
	
	formula branch_t = assign_val(1,next_lit,f);
		printf("\t\t--------- branch t %d\n", f.all_lits[next_lit].id);
		print_lits(branch_t);
	formula branch_f = assign_val(0,next_lit,f);
		printf("\t\t--------- branch f %d \n", f.all_lits[next_lit].id);
		print_lits(branch_f);
	return (DPLL(branch_t) | DPLL(branch_f));
}

formula assign_val(int val, int index, formula f)
{
	int i,j;
	for(i = 0; i < f.num_clauses; i++)
	{
		for(j = 0; j < f.clauses[i].len; j++)
		{
			if(f.clauses[i].lits[j].id == f.all_lits[index].id)
			{
				f.clauses[i].lits[j].val = val;
				if(val == 1)
				{
					if(f.clauses[i].lits[j].is_pos == 1)
						f.clauses[i].lits[j].eval = 1;
					else
						f.clauses[i].lits[j].eval = 0;
				}
				else
				{	
					if(f.clauses[i].lits[j].is_pos == 1)
						f.clauses[i].lits[j].eval = 0;
					else
						f.clauses[i].lits[j].eval = 1;
				}
				
				f.clauses[i].lits[j].is_assigned = 1;
			}
		}
	}
	f.all_lits[index].is_assigned = 1;
	
	printf("assign val returning -----------\n");

	print_formula(f);

	return f;
}	

int evaluate(formula f)
{
	int i,j;
	int is_true;
	for(i = 0; i < f.num_clauses; i ++)
	{
		is_true = 0;
		for(j = 0; j < f.clauses[i].len; j++)
		{
			if(f.clauses[i].lits[j].eval == 1)
				is_true = 1; 
		}
		if(is_true == 0)
			return 0;
	}
	return is_true;
}


void print_formula(formula current_formula)
{
	int j,i;		
	for (j = 0; j < current_formula.num_clauses; j++){
		
		for (i = 0; i < current_formula.clauses[j].len; i++){
			// print all literals in a clause
			if(!current_formula.clauses[j].lits[i].is_pos)
				printf("-");
			printf("%d ", current_formula.clauses[j].lits[i].id);		

//		printf("id %d \n is_pos %d \n", current_formula.clauses[j].lits[i].id, current_formula.clauses[j].lits[i].is_pos);
		}
		printf("\n");
	}

	
/*	for(i = 1; i < num_lits; i++)
	{
		printf("id %i is_pure %i\n", i, is_pure(i)); 
		printf("is consistent: %i\n", is_consistent());
	}
*/

}

void print_lits(formula f)
{
	int i,j;
	for(i = 0; i < f.num_clauses; i++)
	{	
		for(j = 0; j < f.clauses[i].len; j++)
		{
			printf("%i id\n", f.clauses[i].lits[j].id);
			printf("\t%i is_assigned\n", f.clauses[i].lits[j].is_assigned);
			printf("\t%i val\n",f.clauses[i].lits[j].val);
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
	
	num_lits = 5;
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
	int num_lits = 6;//	first.len + second.len + third.len;

	int satisfiable = DPLL(f1);
	printf("\n%d\n", satisfiable);
}
