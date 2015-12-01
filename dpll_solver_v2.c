#include "dpll_structs.h"
#include "input_verifier.c"
#include <stdlib.h>
#include <stdio.h>

void print_formula(formula f)
{
  printf("Formula has %d clauses.\n", f.num_clauses);
}

int is_satisfied(formula f)
{
  return f.num_unsatisfied == 0;
}

int is_empty_clause(clause c)
{
  return !c.is_satisfied && c.num_unassigned == 0;
}

int contains_empty_clause(formula f)
{
  int i = 0;
  for(; i < f.num_clauses; i++)
    {
      if(is_empty_clause(f.clauses[i]))
	return 1;
    }
  return 0;
}

int is_pure_literal(formula f, lit_clauses lc)
{
  int is_set = 0;
  int val = 0;
  int i = 0;
  for(; i < lc.num_clauses; i++)
    {
      if(is_set)
	break;

      clause_index ci = lc.clauses[i];
      literal curr_lit = f.clauses[ci.clause].lits[ci.index];
      if(!curr_lit.is_assigned)
	{
	  is_set = 1;
	  val = curr_lit.is_pos;
	}
    }
  
  for(; i < lc.num_clauses; i++)
    {
      if(is_set)
	break;
      
      clause_index ci = lc.clauses[i];
      literal curr_lit = f.clauses[ci.clause].lits[ci.index];
      if(!curr_lit.is_assigned)
	{
	  if(val != curr_lit.is_pos)
	    return 0;
	}
    }

  return 1;
}

int is_unit_clause(clause c)
{
  return c.num_unassigned == 1;
}

 DPLL(Φ)
   if Φ is a consistent set of literals
       then return true;
   if Φ contains an empty clause
       then return false;
   for every unit clause l in Φ
      Φ ← unit-propagate(l, Φ);
   for every literal l that occurs pure in Φ
      Φ ← pure-literal-assign(l, Φ);
   l ← choose-literal(Φ);
   return DPLL(Φ ∧ l) or DPLL(Φ ∧ not(l));

int is_satisfiable(formula f)
{
  if(is_satisfied(f))
    return true;
  if(contains_empty_clause(f))
    return false;
  int i;
  for(i = 0; i < f.num_clauses; i++)
    {
      if(!f.clauses[i].is_satisfied && is_unit_clause(f.clauses[i]))
	{
	  // Unit propogate it.
	}
    }

  for(i = 0; i < f.num_lits; i++)
    {
      if(!f.all_lits[i].is_assigned && is_pure_literal(f.all_lits.[i]))
	{
	  // Pure literal assign it.
	}
    }
  
  // pick next literal (index)!
  for(i = 0; i < f.num_lits; i++)
    {
 
    }

  // Guess true

  // Guess false
  
}

int main(int argc, char** argv)
{
  int err = 0;
  formula f = verify(argc, argv, &err);
  if(err)
    {
      printf("ERROR");
      return;
    }
}
