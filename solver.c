#include <stdlib.h>
#include <stdio.h>

int has_empty(formula f);
int is_unit(clause c);
formula unit_prop(literal l, formula f);
formula pure_assign(literal l, formula f); 

typedef struct literal{
	//Variable read from file
	int id;
	//Is positive if true 
	int is_pos;
	int is_assigned;
	int val;
}literal;

typedef struct clause{
	int len;
	 literal *lits;
}clause;

typedef struct formula{
	int num_clauses;
	clause *clauses;
}formula;

formula f1;
int num_lits;

// a pure literal has a consistent polarity in all instances
int is_pure(int id)
{
	int pure = 0; //boolean set to false
	int has_pos = 0;
  int has_neg = 0;

	// TODO can bail early if we keep a count of appearences per literal
	// loop through all the clause of the formula
	int i;
	int j;
	for(i = 0; i < f1.num_clauses; i++)
	{
		// loop through all the literals of each clause
		clause temp = f1.clauses[i];
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

int is_consistent()
{	
	int is_con = 1;
	int i;
	for(i = 0; i < num_lits; i++)
	{
			is_con = is_pure(i);
			if(is_con == 0)
				return 0;
	}
	return 1;
}

int DPLL(formula f)
{
	if(is_consistent(f))
		return 1;
	
	if(has_empty(f))
		return 0;
	
	int i;
	for(i = 0; i < f.num_clauses; i++)
	{
		if(is_unit(f.clauses[i])) 
			f = unit_prop(f.clauses[i].lits[0], f);
	}
	
	//get pures
	int pures [num_lits];
	for(i = 1; i <= num_lits; i++)
	{
		if(is_pure(i))
			pures[i] = i;
		else
			pures[i] = 0;
	}

	for(i = 0; i <= num_lits; i++)
	{
		if(pures[i])
			f = pure_assign(i, f);
	}
	//choose literal
	
}

int main(int argc, char *argv[])
{
	literal a;
	a.id = 1;
	a.is_pos = 0;
	
	literal b;
	b.id = 5;
	b.is_pos = 0;
	
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
	aaa.is_pos = 0;
	
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
	int i,j;
	
	for (j = 0; j < 3; j++){
		for (i = 0; i < f1.clauses[j].len; i++){
			printf("id %d \n is_pos %d \n", f1.clauses[j].lits[i].id, f1.clauses[j].lits[i].is_pos);
		}
		printf("\n");
	}
	
	int num_lits = 6;//	first.len + second.len + third.len;
	for(i = 1; i < num_lits; i++)
	{
		printf("id %i is_pure %i\n", i, is_pure(i)); 
		printf("is consistent: %i\n", is_consistent());
	}

}
