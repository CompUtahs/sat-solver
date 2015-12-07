#include <stdio.h>
#include <stdlib.h>
#include "dpll_structs.h"
#include <string.h>    

clause parse_clause(char* line, int num_params, lit_count* lits_seen, int* err);

/**
 * lit_count comparer - to sort lit_counts in descending order
 **/
int compare_lit_count (const void * a, const void * b)
{
  return ( (*(lit_count *) b).count - (*(lit_count *) a).count);
}

/**
 * Parse the supplied input file into a formula struct
 **/
formula verify(int argc, char** argv, int* err)
{
  FILE* input;
  char curr_line[65536];
  int num_params = 0;
  int num_statements = 0;
  long statements_read = 0;
  char prob_start[2];
  char cnf[4];
  formula* f;

  f = malloc(sizeof(formula));
  memset(curr_line, 0, sizeof(curr_line));

  if(argc != 2) 
    {
      *err = 1;
      return *f;
    }
  input = fopen(argv[1], "r"); // open file for reading only.
  
  if (input == NULL) 
    {
      *err = 1;
      return *f;
    }

  while(fgets(curr_line, sizeof(curr_line), input)) 
    {
      // Strip out comment lines beginning with 'c'
      if(curr_line[0] == 'c') 
	{
	  continue;
	}    
      // Parse problem definition line starting with 'p'
      else if(curr_line[0] == 'p')
	{
	  sscanf(curr_line, "%s %s %i %i",
	         prob_start,             
		 cnf,                 
		 &num_params,
		 &num_statements);
	  
	  // Verify all parts of p line were correct
	  if(strcmp("p ", prob_start) &&
	     strcmp("cnf ", cnf) &&
	     num_params > 0 &&
	     num_statements > 0)
	    {
	      break;
	    }
	  else
	    {
	      *err = 1;
	      return *f;
	    }
	}   
      else
	{
	  *err = 1;
	  return *f;
	}
    }
  // Use array 'possible_lits' to track frequency of appearance of literals
  lit_count possible_lits[num_params + 1];
  memset(possible_lits, 0, sizeof(possible_lits));
  lit_clauses possible_watches[num_params + 1];
  memset(possible_watches, 0, sizeof(possible_watches));

  // Secure memory for the array of clauses
  clause* clauses = malloc(num_statements * sizeof(clause));

  int erro = 0;
  int index = 0;
  // Verify file contents are valid CNF
  while(fgets(curr_line, sizeof(curr_line), input)) 
    {
      if(curr_line[0] == 'c')
	continue;
      clause curr_clause = parse_clause(curr_line, num_params, possible_lits, &erro);
      if(erro)
	{
	  *err = erro;
	  return *f;
	}
      clauses[index] = curr_clause;

      index++;
    }

  // Determine how many literals appeared during parsing
  int num_actual_lits = 0;
  
  for(index = 0; index <= num_params; index++) 
    {      
      if(possible_lits[index].count) {
	num_actual_lits++;
      }
    }

  // Strip out literals that COULD have appeared in clauses, but did not.
  lit_count lit_counts[num_actual_lits];
  int curr_lit = 0;
  for(index = 0; index <= num_params ; index++) 
    {
      if(possible_lits[index].count)
	{
	  lit_count lc;
	  lc.id = index;
	  lc.count = possible_lits[index].count;
	  lit_counts[curr_lit] = lc;
	  curr_lit++;
	}
    }

  // Sort the literals from most frequent to least frequently seen, to optimize solution
  qsort(lit_counts, num_actual_lits, sizeof(lit_count), compare_lit_count);

  // Convert the lit_counts to lit_clauses structs for the formula to use
  lit_clauses* all_lits = malloc(num_actual_lits * sizeof(lit_clauses));
  for(index = 0; index < num_actual_lits; index++)
    {
      lit_clauses lc;
      lc.cur_clause = 0;
      lc.num_clauses = lit_counts[index].count;
      lc.clauses = malloc(lc.num_clauses * sizeof(clause_index));
      literal l;
      l.id = lit_counts[index].id;
      l.is_assigned = 0;
      lc.lit = l;
      possible_watches[l.id] = lc;
    }

  for(index = 0; index < num_statements; index++)
    {
      int lit_c;
      for(lit_c = 0; lit_c < clauses[index].len; lit_c++)
	{
	  int cur_count = possible_watches[clauses[index].lits[lit_c].id].cur_clause;
	  int cur_lit = clauses[index].lits[lit_c].id;
	  possible_watches[cur_lit].clauses[cur_count].clause = index;
	  possible_watches[cur_lit].clauses[cur_count].index = lit_c;
	  possible_watches[cur_lit].cur_clause++;
	}
    }

  for(index = 0; index < num_actual_lits; index++)
    {
      all_lits[index] = possible_watches[lit_counts[index].id];
    }

  
  f->num_clauses = num_statements;
  f->num_lits = num_actual_lits;
  f->clauses = clauses;
  f->all_lits = all_lits;
  f->num_unsatisfied = num_statements;
  f->has_empty_clause = 0;
  f->potential_units = NULL;
  f->potential_pures = NULL;
  f->potential_unit_count = num_statements;
  f->potential_pure_count = num_actual_lits;
  f->guesses = NULL;
  f->guess_arr = malloc(sizeof(int) * num_actual_lits);
  f->guesses_made = 0;
  return *f;
}

/**
 * Parse the provided line into a clause struct
 * Also tracks occurences of literals in the clause
 **/
clause parse_clause(char* line, int num_params, lit_count* lits_seen, int* err)
{
  clause c;
  int lit_index;
  int num_tokens = 0;
  int line_len = 0;
  int prev_was_whitespace = 1;
  int i;
  
  for(i = 0; 1; i++)
    {
      char curr = line[i];
      if(curr == '0' && prev_was_whitespace)
	break;
      if(curr == ' ' || curr == '\t')
	{
	  if(!prev_was_whitespace)
	    {
	      num_tokens++;
	    }
	  prev_was_whitespace = 1;
	}
      else {
	prev_was_whitespace = 0;
      }
    }

  literal* lits = malloc(sizeof(literal) * num_tokens);
  char* token = strtok(line, " \t");
  for(lit_index = 0;lit_index < num_tokens; lit_index++)
    {            
      literal lit;
      char* lastChar;
      // atoi can be used in this instance, because 0 should never be parsed
      int val = atoi(token);
      // Determine if an error occured while parsing the string
      if(val == 0) {
	// Error from strtol()
	*err = 1;
	return c;
      }     
      if(val < 0)
	{
	  val = -1 * val;
	  lit.is_pos = 0;
	}
      else
	lit.is_pos = 1;
      
      lit.id = val;
      lit.is_assigned = 0;
      lit.val = 0;
      lit.eval = 0;
      lit.index = 0;
      lits_seen[val].count++;
      lits_seen[val].id = val;
      lits[lit_index] = lit;

      token = strtok(NULL, " \t");

    }
  
  c.len = num_tokens;
  c.num_unassigned = num_tokens;
  c.lits = lits;
  c.is_satisfied = 0;

  return c;
}

// Formula destructor
void annhialate_formula(formula f)
{
  int lit_count = 0;
  for(; lit_count < f.num_lits; lit_count++)
    {
      free(f.all_lits[lit_count].clauses);
    }
  free(f.all_lits);

  // Free all literals in clauses
  int c_count = 0;
  for(; c_count < f.num_clauses; c_count++)
    {
      free(f.clauses[c_count].lits);
    }

  free(f.clauses);

  free(f.potential_units);
  free(f.potential_pures);
  free(f.guess_arr);
}
