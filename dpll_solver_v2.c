#include "dpll_structs.h"
#include "input_verifier.c"
#include <stdlib.h>
#include <stdio.h>

void print_formula(formula f)
{
  int i = 0; 
  printf("num_unsatisfied: %d\n", f.num_unsatisfied);
  for(; i < f.num_clauses; i++)
    {
      int j = 0;
      for(; j < f.clauses[i].len; j++)
	{
	  if(!f.clauses[i].lits[j].is_pos) {
	    printf("-");
	  } 
	  printf("%d",f.clauses[i].lits[j].eval);
	  if(!f.clauses[i].lits[j].is_assigned)
	    printf("u");
	  printf(" ");
	}
      printf("num_unassigned: %d, is_satisfied: %d", f.clauses[i].num_unassigned, f.clauses[i].is_satisfied);
      printf("\n");
    }
}

/**
 * Returns the index of the unit literal in the clause
 **/
int find_unit_lit(clause c)
{
  int i = 0;
  for(; i < c.len; i++)
    {
      if(!c.lits[i].is_assigned)
	return i;
    }
  return -1;
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
      clause_index ci = lc.clauses[i];
      literal curr_lit = f.clauses[ci.clause].lits[ci.index];
      if(!curr_lit.is_assigned)
	{
	  if(val != curr_lit.is_pos)
	    return 0;
	}
    }

  return (val ? 1 : -1);
}

int is_unit_clause(clause c)
{
  return c.num_unassigned == 1;
}

int eval_lit(int val, int is_pos)
{
  return ((val && is_pos) || (!val && !is_pos));
}

void guess(formula f, int lit_index, int guess)
{
  //printf("guessing %d to be %d\n", f.all_lits[lit_index].lit.id, guess);
  int i = 0;
  lit_clauses lc = f.all_lits[lit_index];
  for(; i < lc.num_clauses; i++)
    {
      // set the guess for the lit in each clause
      clause_index cl = lc.clauses[i];
      f.clauses[cl.clause].num_unassigned--;
      // set the value of the literal 
      f.clauses[cl.clause].lits[cl.index].is_assigned = 1;
      f.clauses[cl.clause].lits[cl.index].val = guess;
      f.clauses[cl.clause].lits[cl.index].eval = !(guess ^ f.clauses[cl.clause].lits[cl.index].is_pos);

      if(f.clauses[cl.clause].lits[cl.index].eval)
	{
	  if(!f.clauses[cl.clause].is_satisfied)
	    {
	      //print_formula(f);
	      f.num_unsatisfied--;
	    }
	  f.clauses[cl.clause].is_satisfied++;
	}
    }
  f.all_lits[lit_index].lit.is_assigned = 1;
  f.all_lits[lit_index].lit.val = guess;
}

void undo_guess(formula f, int lit_index)
{
  //printf("undoing it...\n");
  int i = 0;
  lit_clauses lc = f.all_lits[lit_index];
  for(; i < lc.num_clauses; i++)
    {
      // unset the guess for the lit in each clause
      clause_index cl = lc.clauses[i];
      if(f.clauses[cl.clause].lits[cl.index].eval)
	{
	  f.clauses[cl.clause].is_satisfied--;
	  if(!f.clauses[cl.clause].is_satisfied)
	    {
	      f.num_unsatisfied++;
	      // printf("plus one...??\n");
	    }
	}

      f.clauses[cl.clause].num_unassigned++;
      // set the value of the literal 
      f.clauses[cl.clause].lits[cl.index].is_assigned = 0;
      f.clauses[cl.clause].lits[cl.index].val = 0;
      f.clauses[cl.clause].lits[cl.index].eval = 0;
    }
}

int is_satisfiable(formula f)
{
  if(is_satisfied(f))
    return 1;
  if(contains_empty_clause(f))
    return 0;
  // Count and map unit clauses
  int i;
  int unit_count = 0;
  for(i = 0; i < f.num_clauses; i++)
    {
      if(!f.clauses[i].is_satisfied && is_unit_clause(f.clauses[i]))
	{
	  // count it
	  unit_count++;
	}
    }
  clause_index unit_clauses[unit_count];
  int clause_index_count = 0;
  for(i = 0; i < f.num_clauses; i++)
    {
      if(!f.clauses[i].is_satisfied && is_unit_clause(f.clauses[i]))
	{
	  // count it
	  clause_index ci;
	  ci.clause = i;
	  ci.index = find_unit_lit(f.clauses[i]);
	  unit_clauses[clause_index_count] = ci;
	  clause_index_count++;
	}
    }
  int l_index = 0;
  for(; l_index < unit_count; l_index++)
    {
      clause_index cur_cl = unit_clauses[l_index];
      literal cur_lit = f.clauses[cur_cl.clause].lits[cur_cl.index];
      for(i = 0; i < f.num_lits; i++)
	{
	  if(f.all_lits[i].lit.id == cur_lit.id && 
	     !f.all_lits[i].lit.is_assigned)
	    {
	      guess(f, i, cur_lit.is_pos);
	      unit_clauses[l_index].index = i;
	      break;
	    }	    
	}
    }

  //  printf("did %d unit clauses.\n", unit_count);
  //  print_formula(f);

  // Count and assign pure literals
  int pure_lit_count = 0;
  for(i = 0; i < f.num_lits; i++)
    {
      if(!f.all_lits[i].lit.is_assigned && is_pure_literal(f, f.all_lits[i]))
	pure_lit_count++;
    }
  
  int pure_lits[pure_lit_count];
  for(i = 0; i < f.num_lits; i++)
    {
      if(!f.all_lits[i].lit.is_assigned)
	{
	  int pure_val = is_pure_literal(f, f.all_lits[i]);

	  if(!pure_val)
	    continue;
	  //printf("pure value of %d for %d\n", pure_val, f.all_lits[i].lit.id);
	  if(pure_val > 0)
	    {
	      guess(f, i, 1);
	    }
	  else
	    {
	      // Assign it false.
	      guess(f, i, 0);
	    }
	  pure_lits[pure_lit_count] = i;
	  pure_lit_count++;
	}
    }
  
  //  printf("did %d pure literals.\n", pure_lit_count);
  // print_formula(f); 

  // pick next literal (index) as i
  for(i = 0; i < f.num_lits; i++)
    {
      if(!f.all_lits[i].lit.is_assigned)
	break;
    }

  // Guess true
  guess(f, i, 1);
  
  if(is_satisfiable(f))
    return 1;
  // Undo guess
  undo_guess(f, i);

  // Guess false
  guess(f, i, 0);
  if(is_satisfiable(f))
    return 1;

  // Undo guess
  undo_guess(f, i);

  // undo unit clauses and pure literals
  int ind = 0;
  for(; ind < unit_count; ind++)
    {
      undo_guess(f, unit_clauses[ind].index);
    }
  for(ind = 0; ind < pure_lit_count; ind++)
    {
      undo_guess(f, pure_lits[ind]);
    }

  // return
  return 0;
}

int main(int argc, char** argv)
{
  int err = 0;
  formula f = verify(argc, argv, &err);
  //print_formula(f);
  if(err)
    {
      printf("ERROR");
      return;
    }
  if(is_satisfiable(f))
    printf("SATISFIABLE\n");
  else
    printf("UNSATISFIABLE\n");
}
