#include "dpll_structs3.h"
#include "input_verifier3.c"
#include "dpll_solver.c"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

time_t timeout_time;
int timeout;

int contains3(formula3 *f, int guess)
{
  int i = 0;
  for(; i < f->guesses_made; i++)
    {
      if(guess == f->guess_arr[i])
	return i;
    }
  return 0;
}

/** FORWARD DECLARATIONS **/

int guess3(formula3 *f, int lit_index, int guess);
void undo_guess3(formula3 *f);


/**
 * Returns 1 if the formula is satisfied
 **/
int is_satisfied3(formula3* f) 
{
  return f->num_unsatisfied == 0;
}

/**
 * Returns 1 if the formula contains an "empty" clause
 **/
int has_empty_clause3(formula3* f)
{
  return f->has_empty_clause;
}

int get_first_guess3(formula3* f, int lit_index)
{
  lit_clauses3 lc = f->all_lits[lit_index];
  if(lc.num_unsatisfied - lc.purity < lc.purity)
    return 1;
  return 0;
}

/**
 * Returns 1 if the literal in question is pure
 *
 * Pure literals are defined as only appearing with a single
 * value (+ or -) in the remaining unsatisfied clauses
 **/
int is_pure_literal3(lit_clauses3 lc)
{
  if(!lc.num_unsatisfied || lc.lit.is_assigned)
    return 0;
  if(lc.purity == lc.num_unsatisfied || lc.purity == 0)
    return (lc.purity ? 1 : -1);

  return 0;
}

/**
 * Returns the index of the unit literal in the clause
 **/
int find_unit_lit3(clause3 c)
{
  int i = 0;
  for(; i < c.len; i++)    
    if(!c.lits[i].is_assigned)
      return i;
  
  return -1;
}

/**
 * Returns 1 if the clause is a unit clause
 **/
int is_unit_clause3(clause3 c)
{
  return c.num_unassigned == 1 && !c.lits[c.num_unassigned].eval;
}

int next_unsat_clause3(formula3* f, int last_rand_guess)
{
  int i = last_rand_guess;
  for(; i < f->num_clauses; i++)
    if(!f->clauses[i].lits[f->clauses[i].num_unassigned].eval)
      return i;
  
  return -1;
}

int next_unsat_lit3(formula3* f, int last_rand_guess)
{
  int i = last_rand_guess;
  for(; i < f->num_lits; i++)
    if(!f->all_lits[i].lit.is_assigned)
      return i;  
  return -1;
}

void add_guess3(formula3 *f, int lit_index)
{
  f->guess_arr[f->guesses_made] = lit_index;
  f->guesses_made++;
}

int remove_guess3(formula3 *f)
{ 
  f->guesses_made--;
  return f->guess_arr[f->guesses_made];
}

void undo_to_guess3(formula3 *f, int lit_index)
{
  f->has_empty_clause = 0;
  while(f->guess_arr[f->guesses_made-1] != lit_index)
    {
      undo_guess3(f);
    }
  undo_guess3(f);
}

void swap_watched_literal3(formula3 *f, int lit_index, int a)
{
  lit_clauses3 lc = f->all_lits[lit_index];
  int b = lc.num_unsatisfied;

  if(a >= b)
    return;

  clause_index3 ca = lc.clauses[a];
  clause_index3 cb = lc.clauses[b];
  lc.clauses[a] = cb;
  lc.clauses[b] = ca;

  f->clauses[ca.clause].lits[ca.index].val = b;
  f->clauses[cb.clause].lits[cb.index].val = a;
}

void swap_clause_literal3(formula3 *f, int c_index, int a)
{
  clause3 c = f->clauses[c_index];
  int b = c.num_unassigned;

  if(a >= b)
    return;

  literal3 la = c.lits[a];
  literal3 lb = c.lits[b];

  c.lits[a] = lb;
  c.lits[b] = la;

  f->all_lits[la.index].clauses[la.val].index = b;
  f->all_lits[lb.index].clauses[lb.val].index = a;
}

void assign_literal3(formula3 *f, int lit_index, int value)
{
  int i;
  lit_clauses3 lc = f->all_lits[lit_index];
  f->all_lits[lit_index].lit.is_assigned = 1;
  f->all_lits[lit_index].lit.val = !!value;

  // loop through all UNSATISFIED clauses in this literals watch list
  for(i = lc.num_unsatisfied - 1; i >= 0; i--)
    {
      // fetch the position of the literal to evaluate
      clause_index3 ci = lc.clauses[i];
      // fetch the clause containing the literal
      clause3 * cur_clause = &(f->clauses[ci.clause]);
      // fetch the actual literal in that clause
      literal3 * cur_lit = &(cur_clause->lits[ci.index]);
      // Evaluate the literal
      int satisfied = (value && cur_lit->is_pos)||(!value && !cur_lit->is_pos);
      cur_lit->eval = satisfied;

      cur_clause->num_unassigned--;

      swap_clause_literal3(f, ci.clause, ci.index);
      if(satisfied)
	{
	  // We satisfied this clause, hooray!
	  f->num_unsatisfied--;
	  f->all_lits[lit_index].num_unsatisfied--;
	  swap_watched_literal3(f, lit_index, i);

	  // loop through the UNASSIGNED literals in this clause
	  int l_ind;
	  for(l_ind = cur_clause->num_unassigned -1; l_ind >= 0; l_ind--)
	    {
	      literal3 lit = cur_clause->lits[l_ind];
	      int all_lits_index = lit.index;	      
	      f->all_lits[all_lits_index].purity -= lit.is_pos;
	      f->all_lits[all_lits_index].num_unsatisfied--;	     
	      swap_watched_literal3(f, all_lits_index, lit.val);
	    }
	}
      else
	{
	  if(cur_clause->num_unassigned == 0)
	    f->has_empty_clause = 1;
	}
    }
}

guess_new_units_pures3(formula3 *f, int lit_index, int num_unsat)
{
  int i;
  lit_clauses3 lc = f->all_lits[lit_index];
  // Loop through all the lit's clauses that weren't satisfied by the last guess
  for(i = 0; i < lc.num_unsatisfied; i++)
    {
      clause_index3 ci= lc.clauses[i];
      clause3 cur_clause = f->clauses[ci.clause];
      literal3 cur_lit = cur_clause.lits[ci.index];

      // if it's a unit clause...
      if(is_unit_clause3(cur_clause))
	{
	  literal3 lit = cur_clause.lits[0];
	  if(!guess3(f, lit.index, lit.is_pos))	    
	    return;	    
	}
    }
  for(; i < num_unsat; i++)
    {
      clause_index3 ci= lc.clauses[i];
      clause3 cur_clause = f->clauses[ci.clause];
      literal3 cur_lit = cur_clause.lits[ci.index];
      // loop through the UNASSIGNED literals in this clause
      int l_ind = 0;

      for(; l_ind < cur_clause.num_unassigned; l_ind++)
	{
	  int lit_i = cur_clause.lits[l_ind].index;
	  lit_clauses3 l_c = f->all_lits[lit_i];
	  if(is_pure_literal3(l_c))		  
	    if(!guess3(f, lit_i, l_c.purity))
	      return; 
	}	    
	
    }
}

int guess3(formula3 *f, int lit_index, int cur_guess)
{  
  if( is_satisfied3(f) || f->has_empty_clause)
    return 1;

  add_guess3(f, lit_index);
  
  int num_unsat = f->all_lits[lit_index].num_unsatisfied;

  assign_literal3(f, lit_index, cur_guess);
  
  if(is_satisfied3(f))
    return 1;
  if(has_empty_clause3(f))
    return 0;
  
  guess_new_units_pures3(f, lit_index, num_unsat);
  if(has_empty_clause3(f))
    return 0;
  return 1;
}

void undo_guess3(formula3 *f)
{
  int lit_index = remove_guess3(f);
  int i;
  lit_clauses3 lc = f->all_lits[lit_index];
  int num_clauses = lc.num_clauses;
  int num_unsat = lc.num_unsatisfied;
  f->all_lits[lit_index].lit.is_assigned = 0;
  f->all_lits[lit_index].lit.val = 0;
  // Loop through the the unsatisfied clauses int this literal's watch list
  for(i = 0; i < num_unsat; i++)
    {
      // Decrement the number of "unassigned" literals in the clause
      clause_index3 ci= lc.clauses[i];      
      clause3 * cur_clause = &(f->clauses[ci.clause]);
      literal3 * cur_lit = &(cur_clause->lits[ci.index]);
      cur_lit->eval = 0;

      cur_clause->num_unassigned++;
    }
  for(i = num_unsat; i < num_clauses; i++)
    {
      clause_index3 ci= lc.clauses[i];      
      clause3 * cur_clause = &(f->clauses[ci.clause]);
      literal3 * cur_lit = &(cur_clause->lits[ci.index]);
      // If this guess satisfied this clause...
      if(cur_lit->eval)
	{
	  cur_lit->eval = 0;
	  // Increment the number of "unsatisfied" clauses in the formula
	  f->num_unsatisfied++;
	  f->all_lits[lit_index].num_unsatisfied++;
	      
	  // Since this was just satisfied, we can check for new pures!
	  int l_ind;
	  for(l_ind = cur_clause->num_unassigned - 1; l_ind >= 0; l_ind--)
	    {
	      literal3 lit = cur_clause->lits[l_ind];
	      f->all_lits[lit.index].purity += lit.is_pos;
	      f->all_lits[lit.index].num_unsatisfied++;
	    }
	}
      else
	break;
	
      cur_clause->num_unassigned++;
    }  
}


int strip_units_and_pures3(formula3 *f)
{
  int i = 0;
  for(; i < f->num_clauses; i++)
    if(is_unit_clause3(f->clauses[i]))
      {
	literal3 lit = f->clauses[i].lits[find_unit_lit3(f->clauses[i])];
	guess3(f, lit.index, lit.is_pos);
      }

  for(i = 0; i < f->num_lits; i++)
    if(!f->all_lits[i].lit.is_assigned && is_pure_literal3(f->all_lits[i]))
      guess3(f, i, f->all_lits[i].purity);
}

int is_satisfiable3(formula3 *f, int last_rand_guess)
{  
  if(time(0) > timeout_time)
    {
      timeout = 1;
      return 1;
    }
  if(is_satisfied3(f))
    return 1;
  
  if(has_empty_clause3(f))
    return 0;

  int guess_lit = next_unsat_lit3(f, last_rand_guess);
  int first_guess = get_first_guess3(f, guess_lit);
  
  if(guess3(f, guess_lit, first_guess))
    if(is_satisfiable3(f, guess_lit))
      return 1;
  undo_to_guess3(f, guess_lit);
  
  if(guess3(f, guess_lit, !first_guess))
    if(is_satisfiable3(f, guess_lit))
      return 1;
  undo_to_guess3(f, guess_lit);
  return 0;
}


/**
 * Adds the appropriate/associated indexes to the clauses' literals
 * The literal.index value corresponds to where the literal exists in all_lits
 **/
void fill_formula3(formula3* f)
{
  int i = 0;
  for(; i < f->num_lits; i++)
    {
      int j = 0;
      f->all_lits[i].purity = 0;
      f->all_lits[i].num_unsatisfied = f->all_lits[i].num_clauses;
      for(; j < f->all_lits[i].num_clauses; j++)
	{
	  clause_index3 ci = f->all_lits[i].clauses[j];
	  literal3* lit = &(f->clauses[ci.clause].lits[ci.index]);
	  lit->index = i;
	  lit->val =j;
	  f->all_lits[i].purity += lit->is_pos;
	}
    }
}

int main(int argc, char** argv)
{
  timeout = 0;
  timeout_time = time(0) + 180;
  int err3 = 0;
  // Parse the formula and verify it is correct
  formula3 f3 = verify3(argc, argv, &err3);
  if(err3)    
    printf("ERROR\n");    
  else
    {
      // Fill the formula with indexes
      fill_formula3(&f3);
      strip_units_and_pures3(&f3);

      int result = is_satisfiable3(&f3, 0);
      if(!timeout)
	{
	  if(result)
	    {
	      printf("SATISFIABLE\n");
	      // Free the formula's memory
	      annhialate_formula3(f3);  
	    }
	  else
	    {
	      printf("UNSATISFIABLE\n");
	      // Free the formula's memory
	      annhialate_formula3(f3);  
	    }
	}
      else
	{
	  int err = 0;
	  // Parse the formula and verify it is correct
	  formula f = verify(argc, argv, &err);
	  if(err)    
	    printf("ERROR");    
	  else
	    {
	      // Fill the formula with indexes
	      fill_formula(&f);

	      if(is_satisfiable(&f, 0))
		printf("SATISFIABLE\n");
	      else
		printf("UNSATISFIABLE\n");
	    }
  
	  // Free the formula's memory
	  annhialate_formula(f);
	}
    } 

}
