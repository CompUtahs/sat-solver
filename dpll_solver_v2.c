/*******************************************************
 *************** SAT SOLVER DELUXE V2.0 ****************
 *******************************************************/

/* Team: Computahs */

#include "dpll_structs.h"
#include "input_verifier.c"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


/**
 * Concatenates the two arrays, with arr_a's contents preceding arr_b's
 *
 * Returns the pointer to the concatenated array.
 **/
int * concatenate_arrs(int* arr_a, int len_a, int* arr_b, int len_b)
{
  int* new_arr = realloc(arr_a, sizeof(int) * len_a + sizeof(int) * len_b);
  memcpy(new_arr + len_a, arr_b, sizeof(int) * len_b);
  free(arr_b);
  return new_arr;
}

void print_clause(clause c)
{
  int j = 0;
  for(; j < c.len; j++)
    {
      if(!c.lits[j].is_pos) {
	printf("-");
      } 
      printf("%d(%d|%d)", c.lits[j].eval, c.lits[j].id, c.lits[j].index);
      if(!c.lits[j].is_assigned)
	printf("u");
      printf(" ");
    }
  printf("unassigned: %d, satisfied: %d", c.num_unassigned, c.is_satisfied);
  printf("\n");
}

void print_lit_watch(formula f, int lit_index)
{
  int i = 0;
  printf("%d Overall - assigned=%d, num_clauses=%d\n", 
	 lit_index, f.all_lits[lit_index].lit.is_assigned, f.all_lits[lit_index].num_clauses);
  for(; i < f.all_lits[lit_index].num_clauses; i++)
    {
      clause_index ci = f.all_lits[lit_index].clauses[i];\
      literal cur_lit = f.clauses[ci.clause].lits[ci.index];
      printf("%d -> %d. assigned: %d val: %d\n  ", 
	     ci.clause, ci.index, cur_lit.is_assigned, cur_lit.val);
      print_clause(f.clauses[ci.clause]);
    }
}


void print_formula(formula f)
{
  int i = 0; 
  printf("num_unsatisfied: %d\n", f.num_unsatisfied);
  for(; i < f.num_clauses; i++)
    {
      print_clause(f.clauses[i]);
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

/**
 * Returns 1 if the formula is satisfied
 **/
int is_satisfied(formula* f)
{
  return f->num_unsatisfied == 0;
}

/**
 * Returns 1 if the clause is "empty"
 **/
int is_empty_clause(clause c)
{
  return !c.is_satisfied && c.num_unassigned == 0;
}

/**
 * Returns 1 if the formula contains an "empty" clause
 **/
int contains_empty_clause(formula* f)
{
  int i = 0;
  for(; i < f->num_clauses; i++)
    {
      if(is_empty_clause(f->clauses[i]))
	{
	  //printf("%d is empty: ", i);
	  //print_clause(f->clauses[i]);
	  return 1;
	}
    }
  return 0;
}

/**
 * Returns 0 if the literal in question is not pure
 * Returns positive value if the literal is pure AND should be set to 1
 * Returns negative value if the literal is pure AND should be set to -1
 *
 * Pure literals are defined as only appearing with a single
 * value (+ or -) in the remaining unsatisfied clauses
 **/
/*
int is_pure_literal(formula* f, lit_clauses lc)
{
  
  int is_set = 0;
  int val = 0;
  int i = 0;
  // Find the value of the first unassigned literal
  for(; i < lc.num_clauses; i++)
    {
      if(is_set)
	break;

      clause_index ci = lc.clauses[i];
      literal curr_lit = f->clauses[ci.clause].lits[ci.index];
      if(!f->clauses[ci.clause].is_satisfied && !curr_lit.is_assigned)
	{
	  is_set = 1;
	  val = curr_lit.is_pos;
	}
    }
  
  // Loop through to guarantee purity
  for(; i < lc.num_clauses; i++)
    {
      clause_index ci = lc.clauses[i];
      literal curr_lit = f->clauses[ci.clause].lits[ci.index];
      if(!f->clauses[ci.clause].is_satisfied && !curr_lit.is_assigned)
	{
	  if(val != curr_lit.is_pos)
	    return 0;
	}
    }

  // Return 1 if val != 0, and -1 otherwise
  return (val ? 1 : -1);
 
}
*/

int is_pure_literal(lit_clauses lc)
{
  if(!lc.num_unsatisfied)
    return 0;

  if(lc.purity == lc.num_unsatisfied || lc.purity == 0)
    return (lc.purity ? 1 : -1);

  return 0;
}

/**
 * Returns 1 if the clause is a unit clause
 **/
int is_unit_clause(clause c)
{
  return c.num_unassigned == 1;
}

/**
 * Carries out the guess action for the literal at f->all_lits[lit_index]
 * with the given value for the guess.
 *
 * Assigns all instances in the watch list for the literal
 **/
void guess(formula* f, int lit_index, int guess)
{
  // Immediately return if this value has already been guessed
  if(f->all_lits[lit_index].lit.is_assigned)
      return;   .

  int i;
  lit_clauses lc = f->all_lits[lit_index];

  // Loop through the literals in this literal's watch lit, assigning each one
  for(i = 0; i < lc.num_clauses; i++)
    {
      clause_index cl = lc.clauses[i];      
      
      // This literal should NOT yet be currently assigned yet
      assert(!f->clauses[cl.clause].lits[cl.index].is_assigned);
      
      // Decrement the number of "unassigned" literals in the clause
      f->clauses[cl.clause].num_unassigned--;

      // Assign the guess to the specific literal 
      f->clauses[cl.clause].lits[cl.index].is_assigned = 1;
      f->clauses[cl.clause].lits[cl.index].eval = !(guess ^ f->clauses[cl.clause].lits[cl.index].is_pos);

      // In the case that this literal satisfies the clause
      if(f->clauses[cl.clause].lits[cl.index].eval)
	{

	  // If this clause hasn't been satisfied UNTIL NOW
	  if(!f->clauses[cl.clause].is_satisfied)
	    {
	      // Decrement the number of "unsatisfied" clauses in the formula
	      f->num_unsatisfied--;
	      int lit_ind = 0;
	      for(; lit_ind < f->clauses[cl.clause].len; lit_ind++)
		{
		  int all_lits_ind = f->clauses[cl.clause].lits[lit_ind].index;
		  f->all_lits[all_lits_ind].num_unsatisfied--;
		  f->all_lits[all_lits_ind].purity -= f->clauses[cl.clause].lits[lit_ind].is_pos;
		}
	    }
	  // Increment the is_satisfied counter/"boolean"
	  f->clauses[cl.clause].is_satisfied++;
	  
	  // is_satisfied SHOULD be positive after the previous operation
	  assert(f->clauses[cl.clause].is_satisfied > 0);
	}
    }

  // Assign the value of the literal in the all_lits array
  f->all_lits[lit_index].lit.is_assigned = 1;
}

/**
 * Reverts a previous "guess" to the literal all_lits[lit_index]
 **/
void undo_guess(formula* f, int lit_index)
{
  // If this literal is not assigned currently, return immediately.
  if(!f->all_lits[lit_index].lit.is_assigned)
    return;

  int i = 0;
  lit_clauses lc = f->all_lits[lit_index];
  // Loop through the literals in this literal's watch lit, unassigning each one
  for(; i < lc.num_clauses; i++)
    {
      clause_index cl = lc.clauses[i];
      
      // If this literal satisfied the current clause, account for that
      if(f->clauses[cl.clause].lits[cl.index].eval)
	{
	  // Decrement the clause's counter/"boolean" is_satisfied
	  f->clauses[cl.clause].is_satisfied--;

	  // If the clause is no longer satisfied
	  if(!f->clauses[cl.clause].is_satisfied)
	    {
	      // Increment the number of unsatisfied clauses in the formula
	      f->num_unsatisfied++;
	      int lit_ind = 0;
	      for(; lit_ind < f->clauses[cl.clause].len; lit_ind++)
		{		  
		  int all_lits_ind = f->clauses[cl.clause].lits[lit_ind].index; 
		  f->all_lits[all_lits_ind].num_unsatisfied++;
		  f->all_lits[all_lits_ind].purity += f->clauses[cl.clause].lits[lit_ind].is_pos;
		}
	    }
	}

      // Increment the number of unassigned literals in the current clause
      f->clauses[cl.clause].num_unassigned++;
      // Unset the value of the literal 
      f->clauses[cl.clause].lits[cl.index].is_assigned = 0;
    }

  // Unset the literal in the all_lits array
  f->all_lits[lit_index].lit.is_assigned = 0;
}

/**
 * Recursive formula that determines satisfiability of a given formula
 **/
int is_satisfiable(formula * f, int last_guess_clause_ind)
{
  // Return 1 if the formula has been satisfied
  if(is_satisfied(f))
    return 1;
  
  // Return 0 if an empty clause is found
  if(contains_empty_clause(f))
    return 0;
  
  int at_least_one_reduced = 1;
  int* unit_lits = NULL;
  int* pure_lits = NULL;
  int unit_count = 0;
  int pure_count = 0;
  // Repeatedly find unit clauses and pure literals, and assign them
  while(at_least_one_reduced)
    {
      int i;
      int cur_unit_count = 0;
      /** Propogate the unit clauses **/

      // Count unit clauses
      for(i = 0; i < f->num_clauses; i++)	
	if(!f->clauses[i].is_satisfied && is_unit_clause(f->clauses[i]))	
	  cur_unit_count++;	    	
      
      int* cur_unit_lits = NULL;
      clause_index* cur_unit_clauses = NULL;
  
      // If unit clauses were found, unit propogate them
      if(cur_unit_count)
	{
	  // Malloc space for the current unit clauses
	  cur_unit_clauses = malloc(sizeof(clause_index) * cur_unit_count);
	  cur_unit_lits = malloc(sizeof(int) * cur_unit_count);

	  // Determine the clause_index for each unit clause
	  int clause_index_count = 0;
	  for(i = 0; i < f->num_clauses; i++)
	    if(!f->clauses[i].is_satisfied && is_unit_clause(f->clauses[i]))
	      {
		clause_index ci;
		ci.clause = i;
		ci.index = find_unit_lit(f->clauses[i]);
		cur_unit_clauses[clause_index_count] = ci;
		clause_index_count++;
	      }	   	
	  
	  // Assign the associated literal
	  int l_index = 0;
	  for(; l_index < cur_unit_count; l_index++)
	    {
	      clause_index cur_cl = cur_unit_clauses[l_index];
	      literal cur_lit = f->clauses[cur_cl.clause].lits[cur_cl.index];
	      cur_unit_lits[l_index] = cur_lit.index;
	      guess(f, cur_lit.index, cur_lit.is_pos);	  	  	   
	    }  
	  // Free this struct array, it's no longer needed
	  if(cur_unit_clauses != NULL)
	    free(cur_unit_clauses);

	  // If no unit clauses have been found before this, use this array
	  if(unit_lits == NULL) 	    
	      unit_lits = cur_unit_lits;  
	  else // Otherwise, concatenate the existing and new unit clause arrays
	    unit_lits = concatenate_arrs(cur_unit_lits,cur_unit_count, unit_lits,unit_count);
	  unit_count += cur_unit_count;    	 
	}
      
      /** Count and assign pure literals **/

      // Count pure literals
      int cur_pure_count = 0;
      for(i = 0; i < f->num_lits; i++)	
	if(!f->all_lits[i].lit.is_assigned &&
	   is_pure_literal(f->all_lits[i]))
	  cur_pure_count++;
      
      // If any pure literals were found
      int* cur_pure_lits = NULL;
      if(cur_pure_count)
	{
	  int pure_ind = 0;
	  cur_pure_lits = malloc(sizeof(int)*cur_pure_count);
	  
	  // Fill cur_pure_lits with pure literals
	  for(i = 0; i < f->num_lits; i++)	   
	    if(!f->all_lits[i].lit.is_assigned && is_pure_literal(f->all_lits[i]))
	      {
		cur_pure_lits[pure_ind] = i;
		pure_ind++;
	      }
	  
	  // Propogate the pure literals
	  for(i = 0; i < cur_pure_count; i++)
	    {
	      int pure_val = is_pure_literal(f->all_lits[cur_pure_lits[i]]);
	      
	      if(pure_val > 0) // pure_val > 0 means set literal to 1
		guess(f, cur_pure_lits[i], 1);
	      else if (pure_val < 0) // pure_val < 0 means set literal to 0
		guess(f, cur_pure_lits[i], 0);
	    }
	  
	  // Use this current array as the base if no pure literals existed
	  if(pure_lits == NULL)
	    pure_lits = cur_pure_lits;
      	  else // Otherwise, concatenate the existing and the current pures
	    pure_lits = concatenate_arrs(cur_pure_lits,cur_pure_count, pure_lits,pure_count);	    
	  pure_count += cur_pure_count;    
	}
      
      // Determine if at leas one clause/literal was eliminated
      at_least_one_reduced = cur_unit_count | cur_pure_count;
    }

  //printf("Just did %d units, %d pures.\n", unit_count, pure_count);
  
  // If the previous guesses brought us to a satisfied state, that's awesome!
  if(is_satisfied(f))
    return 1;

  // Otherwise, check if an empty clause exists (because then we can backtrack)
  if(!contains_empty_clause(f))
    {
      int found = 0;
      // pick next literal to guess as i
      int i;
      for(i = 0/*last_guess_clause_ind*/; i < f->num_clauses; i++)
	{
	  // First literal that appears in an unsatisfied clause is chosen
	  // VERY naive, but it works
	  if(!f->clauses[i].is_satisfied && f->clauses[i].num_unassigned)
	    {	      
	      found = 1;
	      i = f->clauses[i].lits[find_unit_lit(f->clauses[i])].index;
	      break;
	    }
	}

      // In the case a literal was found (may be unnecessary check)
      if(found)
	{
	  // Guess true and check for satisfiability after the guess
	  guess(f, i, 1);
      
	  if(is_satisfiable(f, i)) 
	    {
	      if(pure_lits != NULL)
		free(pure_lits);
	      if(unit_lits != NULL)
		free(unit_lits);  
	      return 1;
	    }

	  // Undo guess, because it wasn't satisfiable
	  undo_guess(f, i);
  
	  // Guess false for the same lit, and check for satisfiability again
	  guess(f, i, 0);

	  if(is_satisfiable(f, i))
	    {
	      if(pure_lits != NULL)
		free(pure_lits);
	      if(unit_lits != NULL)
		free(unit_lits);  
	      return 1;
	    }

	  // Undo guess, because we were wrong again!
	  undo_guess(f, i);
	}
    }

  // undo unit clauses and pure literals guesses
  int ind = 0;
  for(; ind < unit_count; ind++)
    {
      undo_guess(f, unit_lits[ind]);
    }
  for(ind = 0; ind < pure_count; ind++)
    {
      undo_guess(f, pure_lits[ind]);
    }

  // Free arrays
  if(pure_lits != NULL)
    free(pure_lits);
  if(unit_lits != NULL)
    free(unit_lits);  

  return 0;
}

/**
 * Adds the appropriate/associated indexes to the clauses' literals
 * The literal.index value corresponds to where the literal exists in all_lits
 **/
void fill_formula(formula* f)
{
  int i = 0;
  for(; i < f->num_lits; i++)
    {
      int j = 0;
      f->all_lits[i].purity = 0;
      f->all_lits[i].num_unsatisfied = f->all_lits[i].num_clauses;
      for(; j < f->all_lits[i].num_clauses; j++)
	{
	  clause_index ci = f->all_lits[i].clauses[j];
	  literal* lit = &(f->clauses[ci.clause].lits[ci.index]);
	  lit->index = i;
	  f->all_lits[i].purity += lit->is_pos;
	}
    }
}

/**
 * Function that debugs the watch list, and loudly complains if there is a bug
 **/
void debug_watch_list(formula* f)
{
  // loop through each literal
  int i = 0;
  for(; i < f->num_lits; i++)
    {
      int j = 0;     
      int occ[f->num_clauses];

      for(; j < f->num_clauses; j++)
	{
	  occ[j] = 0;
	}
      for(j = 0; j < f->all_lits[i].num_clauses; j++)
	{
	  clause_index ci = f->all_lits[i].clauses[j];	  
	  occ[ci.clause]++;
	  if(occ[ci.clause] > 1)
	    {
	      printf("THERE'S A BUG IN THE WATCHLIST (it's at %d - clause: %d, lit: %d\n",
		     occ[ci.clause], ci.clause, ci.index);
	      int x;
	      for(x = 0; x < f->all_lits[i].num_clauses; x++)
		{
		  clause_index ci = f->all_lits[i].clauses[x];
		  printf("%d -> %d\n", ci.clause, ci.index);
		}
	    }
	}
    }
}

int main(int argc, char** argv)
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
