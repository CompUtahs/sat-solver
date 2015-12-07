#include "dpll_structs.h"
#include "input_verifier.c"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

FILE * out;

int contains(formula *f, int guess)
{
  int i = 0;
  for(; i < f->guesses_made; i++)
    {
      if(guess == f->guess_arr[i])
	return i;
    }
  return 0;
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

void print_simple_watch(formula f, int lit_index)
{
  int i = 0;
  printf("%d Overall:  ", lit_index);
  for(; i < f.all_lits[lit_index].num_clauses; i++)
    {
      clause_index ci = f.all_lits[lit_index].clauses[i];\
      literal cur_lit = f.clauses[ci.clause].lits[ci.index];
      printf("%d -> %d.  |  ", ci.clause, ci.index);
    }
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

/** FORWARD DECLARATIONS **/

int guess(formula *f, int lit_index, int guess);
void undo_guess(formula *f);


/**
 * Returns 1 if the formula is satisfied
 **/
int is_satisfied(formula* f) 
{
  return f->num_unsatisfied == 0;
}

/**
 * Returns 1 if the formula contains an "empty" clause
 **/
int has_empty_clause(formula* f)
{
  return f->has_empty_clause;
}

int get_first_guess(formula* f, int lit_index)
{
  lit_clauses lc = f->all_lits[lit_index];
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
int is_pure_literal(lit_clauses lc)
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
int find_unit_lit(clause c)
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
int is_unit_clause(clause c)
{
  return c.num_unassigned == 1 && !c.lits[c.num_unassigned].eval;
}

int next_unsat_clause(formula* f, int last_rand_guess)
{
  int i = last_rand_guess;
  for(; i < f->num_clauses; i++)
    if(!f->clauses[i].lits[f->clauses[i].num_unassigned].eval)
      return i;
  
  return -1;
}

int next_unsat_lit(formula* f, int last_rand_guess)
{
  int i = last_rand_guess;
  for(; i < f->num_lits; i++)
    if(!f->all_lits[i].lit.is_assigned)
      return i;  
  return -1;
}

void add_guess(formula *f, int lit_index)
{
/*
  int x;
  if(x = contains(f, lit_index))
    {
      printf("WHY DID WE GUESS %d AGAIN. At guess %d, previously at %d\n", f->all_lits[lit_index].lit.id, f->guesses_made, x);
      getchar();
    }
*/
  f->guess_arr[f->guesses_made] = lit_index;
  f->guesses_made++;

}

int remove_guess(formula *f)
{ 
  f->guesses_made--;
  return f->guess_arr[f->guesses_made];
}

void undo_to_guess(formula *f, int lit_index)
{
  f->has_empty_clause = 0;
  while(f->guess_arr[f->guesses_made-1] != lit_index)
    {
      undo_guess(f);
    }
  undo_guess(f);
}

void swap_watched_literal(formula *f, int lit_index, int a)
{
  lit_clauses lc = f->all_lits[lit_index];
  int b = lc.num_unsatisfied;

  if(a >= b)
    return;

  clause_index ca = lc.clauses[a];
  clause_index cb = lc.clauses[b];
  lc.clauses[a] = cb;
  lc.clauses[b] = ca;

  f->clauses[ca.clause].lits[ca.index].val = b;
  f->clauses[cb.clause].lits[cb.index].val = a;
}

void swap_clause_literal(formula *f, int c_index, int a)
{
  clause c = f->clauses[c_index];
  int b = c.num_unassigned;

  if(a >= b)
    return;

  literal la = c.lits[a];
  literal lb = c.lits[b];

  c.lits[a] = lb;
  c.lits[b] = la;

  f->all_lits[la.index].clauses[la.val].index = b;
  f->all_lits[lb.index].clauses[lb.val].index = a;
}

void assign_literal(formula *f, int lit_index, int value)
{
  int i;
  lit_clauses lc = f->all_lits[lit_index];
  f->all_lits[lit_index].lit.is_assigned = 1;
  f->all_lits[lit_index].lit.val = !!value;

  // loop through all UNSATISFIED clauses in this literals watch list
  for(i = lc.num_unsatisfied - 1; i >= 0; i--)
    {
      // fetch the position of the literal to evaluate
      clause_index ci = lc.clauses[i];
      // fetch the clause containing the literal
      clause * cur_clause = &(f->clauses[ci.clause]);
      // fetch the actual literal in that clause
      literal * cur_lit = &(cur_clause->lits[ci.index]);
      // Evaluate the literal
      int satisfied = (value && cur_lit->is_pos)||(!value && !cur_lit->is_pos);
      cur_lit->eval = satisfied;

      cur_clause->num_unassigned--;

      swap_clause_literal(f, ci.clause, ci.index);
      if(satisfied)
	{
	  // We satisfied this clause, hooray!
	  f->num_unsatisfied--;
	  f->all_lits[lit_index].num_unsatisfied--;
	  swap_watched_literal(f, lit_index, i);

	  // loop through the UNASSIGNED literals in this clause
	  int l_ind;
	  for(l_ind = cur_clause->num_unassigned -1; l_ind >= 0; l_ind--)
	    {
	      literal lit = cur_clause->lits[l_ind];
	      int all_lits_index = lit.index;	      
	      f->all_lits[all_lits_index].purity -= lit.is_pos;
	      f->all_lits[all_lits_index].num_unsatisfied--;	     
	      swap_watched_literal(f, all_lits_index, lit.val);
	    }
	}
      else
	{
	  if(cur_clause->num_unassigned == 0)
	    f->has_empty_clause = 1;
	}
    }
}

guess_new_units_pures(formula *f, int lit_index, int num_unsat)
{
  int i;
  lit_clauses lc = f->all_lits[lit_index];
  // Loop through all the lit's clauses that weren't satisfied by the last guess
  //printf("checking units/pures for %d\n", f->all_lits[lit_index].lit.id);

  for(i = 0; i < lc.num_unsatisfied; i++)
    {
      clause_index ci= lc.clauses[i];
      clause cur_clause = f->clauses[ci.clause];
      literal cur_lit = cur_clause.lits[ci.index];
      //printf("unit? ");print_clause(cur_clause);

      // if it's a unit clause...
      if(is_unit_clause(cur_clause))
	{
	  literal lit = cur_clause.lits[0];
	  //printf("       unit--> ");print_clause(cur_clause);
	  //if(!f->all_lits[lit.index].lit.is_assigned)	    
	    {
	      if(!guess(f, lit.index, lit.is_pos))	    
		return;	    
	    }
	  /*
	    else if(f->all_lits[lit.index].lit.val == lit.is_pos)
	    {
	    f->has_empty_clause = 1;
	    return;
	    }
	  */
	}
    }
  for(; i < num_unsat; i++)
    {
      clause_index ci= lc.clauses[i];
      clause cur_clause = f->clauses[ci.clause];
      literal cur_lit = cur_clause.lits[ci.index];
      // loop through the UNASSIGNED literals in this clause
      int l_ind = 0;
      //      for(l_ind = cur_clause.num_unassigned - 1; l_ind >= 0; l_ind--)
      //printf("pure? ");print_clause(cur_clause);

      for(; l_ind < cur_clause.num_unassigned; l_ind++)
	{

	  int lit_i = cur_clause.lits[l_ind].index;
	  lit_clauses l_c = f->all_lits[lit_i];
	  if(is_pure_literal(l_c))	
	    {
	      //printf("    pure--> ");print_clause(cur_clause);
	  
	      if(!guess(f, lit_i, l_c.purity))
		return;	
	    }	
	}	    
	
    }
}

int guess(formula *f, int lit_index, int cur_guess)
{  
  if( is_satisfied(f) || f->has_empty_clause)
    return 1;

  //printf("Guessing %d as %d\n", f->all_lits[lit_index].lit.id, cur_guess);
  add_guess(f, lit_index);
  
  int num_unsat = f->all_lits[lit_index].num_unsatisfied;

  assign_literal(f, lit_index, cur_guess);
  
  if(is_satisfied(f))
    return 1;
  if(has_empty_clause(f))
    return 0;
  
  guess_new_units_pures(f, lit_index, num_unsat);
  //printf("FINISHING %d as %d\n", f->all_lits[lit_index].lit.id, cur_guess);
  if(has_empty_clause(f))
    return 0;
  return 1;
}

void undo_guess(formula *f)
{
  int lit_index = remove_guess(f);
  //printf("undoing %d\n", f->all_lits[lit_index].lit.id);
  int i;
  lit_clauses lc = f->all_lits[lit_index];
  int num_clauses = lc.num_clauses;
  int num_unsat = lc.num_unsatisfied;
  f->all_lits[lit_index].lit.is_assigned = 0;
  f->all_lits[lit_index].lit.val = 0;
  // Loop through the the unsatisfied clauses int this literal's watch list
  for(i = 0; i < num_unsat; i++)
    {
      // Decrement the number of "unassigned" literals in the clause
      clause_index ci= lc.clauses[i];      
      clause * cur_clause = &(f->clauses[ci.clause]);
      literal * cur_lit = &(cur_clause->lits[ci.index]);
      cur_lit->eval = 0;

      cur_clause->num_unassigned++;
    }
  for(i = num_unsat; i < num_clauses; i++)
    {
      clause_index ci= lc.clauses[i];      
      clause * cur_clause = &(f->clauses[ci.clause]);
      literal * cur_lit = &(cur_clause->lits[ci.index]);
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
	      literal lit = cur_clause->lits[l_ind];
	      f->all_lits[lit.index].purity += lit.is_pos;
	      f->all_lits[lit.index].num_unsatisfied++;
	    }
	}
      else
	{
	  // cur_clause->num_unassigned++;

	  break;
	}
      cur_clause->num_unassigned++;

    }  
}


int strip_units_and_pures(formula *f)
{
  int i = 0;
  for(; i < f->num_clauses; i++)
    {
      if(is_unit_clause(f->clauses[i]))
	{
	  literal lit = f->clauses[i].lits[find_unit_lit(f->clauses[i])];
	  // printf("stripping %d as unit\n", lit.id); 		
	  guess(f, lit.index, lit.is_pos);
	}
    }

  for(i = 0; i < f->num_lits; i++)
    {
      if(!f->all_lits[i].lit.is_assigned && is_pure_literal(f->all_lits[i]))
	{
	  //printf("stripping %d as pure\n", f->all_lits[i].lit.id); 
	  guess(f, i, f->all_lits[i].purity);
	}
    }
}

int is_satisfiable(formula *f, int last_rand_guess)
{  
  if(is_satisfied(f))
    return 1;
  
  if(has_empty_clause(f))
    return 0;
  
  //  int clause_ind = next_unsat_clause(f, last_rand_guess); 
  //  int lit_ind = f->clauses[clause_ind].lits[0].index;
  int guess_lit = next_unsat_lit(f, last_rand_guess);
  int first_guess = get_first_guess(f, guess_lit);
  
  //printf("random 1st guess: %d, %d\n", f->all_lits[guess_lit].lit.id, first_guess);
  if(guess(f, guess_lit, first_guess))
    if(is_satisfiable(f, guess_lit))
      return 1;
  // printf("undoing %d\n", f->all_lits[guess_lit].lit.id);
  undo_to_guess(f, guess_lit);
  
  //printf("random 2nd guess: %d, %d\n", f->all_lits[guess_lit].lit.id, !first_guess);
  if(guess(f, guess_lit, !first_guess))
    if(is_satisfiable(f, guess_lit))
      return 1;
  undo_to_guess(f, guess_lit);
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
	  lit->val =j;
	  f->all_lits[i].purity += lit->is_pos;
	}
    }
}

int main(int argc, char** argv)
{
  //out = fopen("output.out", "w");
  int err = 0;
  // Parse the formula and verify it is correct
  formula f = verify(argc, argv, &err);
  if(err)    
    printf("ERROR\n");    
  else
    {
      // Fill the formula with indexes
      fill_formula(&f);
      //fprintf(out, "stripping out pures and shit\n");
      strip_units_and_pures(&f);
      //fprintf(out, "Finished stripping out pures and shit\n");

      if(is_satisfiable(&f, 0))
	printf("SATISFIABLE\n");
      else
	printf("UNSATISFIABLE\n");
    }
  
  // Free the formula's memory
  annhialate_formula(f);
}
