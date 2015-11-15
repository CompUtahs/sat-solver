#include <stdio.h>
#include <stdlib.h>
#include "dpll_structs.h"
#include <string.h>      
//************************

clause parse_clause(char* line, int num_params, int* lits_seen, int* err);

/**
 * Determine that the supplied file is a valid format
 *
 * Returns 0 if valid, and 1 otherwise
 **/
formula verify(int argc, char** argv, int* err)
{
  FILE* input;
  char curr_line[65536];
  int problem_is_defined = 0;
  int num_params = 0;
  int num_statements = 0;
  long statements_read = 0;
  //char* prob_start, cnf;      // if you do it this way, I'm pretty sure the only way to avoid the seg fault is by malloc-ing later
  char prob_start[2];         //  I think you meant to place the "p", here right? --> should only ever hold a single char
  char cnf[4];                // should (indeed it will!) always be "cnf".
  formula* f;                            // both these arrays need an extra space to account for the trailing space in the input line from file
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

  f = malloc(sizeof(formula));

  while(fgets(curr_line, sizeof(curr_line), input)) 
    {
      if(curr_line[0] == 'c') 
	{
	  continue;
	}    
      // Parse line starting with 'p'
      else if(curr_line[0] == 'p')
	{
	  sscanf(curr_line, "%s %s %i %i", // works now 
	         prob_start,             
		 cnf,                 
		 &num_params,
		 &num_statements);
	  
	  // Verify all parts of p line were correct
	  if(strcmp("p ", prob_start) &&
	     strcmp("cnf ", cnf) &&        // *****************
	     num_params >= 0 &&
	     num_statements >= 0)
	    {
	      problem_is_defined = 1;
	      break;
	    }
	  else
	    {
	      *err = 1;
	      return *f;
	    }
	}      
    }
  
  int possible_lits[num_params + 1];
  memset(possible_lits, 0, sizeof(possible_lits));
  clause* clauses = malloc(num_statements * sizeof(clause));

  int index = 0;
  // Verify file contents are valid CNF
  while(fgets(curr_line, sizeof(curr_line), input)) 
    {
      //printf("Reading clause: %i --- %s\n", index, curr_line);
      clause curr_clause = parse_clause(curr_line, num_params, possible_lits, err);
      if(*err)
	return *f;
      clauses[index] = curr_clause;
      index++;
    }

  int num_actual_lits = 0;

  for(index = 0; index <= num_params; index++) 
    {
      if(possible_lits[index])
	num_actual_lits++;
    }

  literal* all_lits = malloc(num_actual_lits * sizeof(literal));
  int curr_lit = 0;
  for(index = 0; index < num_params; index++) 
    {
      if(possible_lits[index])
	{
	  literal l;
	  l.id = index;
	  l.is_assigned = 0;
	  all_lits[curr_lit] = l;
	  curr_lit++;
	}
    }
  
  f->num_clauses = num_statements;
  f->num_lits = num_actual_lits;
  f->clauses = clauses;
  f->all_lits = all_lits;

  return *f;
}

clause parse_clause(char* line, int num_params, int* lits_seen, int* err)
{
  clause c;
  int lit_index;
  int num_tokens = 0;
  int line_len = 0;
  char prev = '\0';
  int i;
  for(i = 0; 1; i++)
    {
      char curr = line[i];
      if(curr == '0' && prev == ' ')
	break;
      if(curr == ' ' && prev != ' ')
	{
	  num_tokens++;
	}
	  prev = curr;
    }
  literal* lits = malloc(sizeof(literal) * num_tokens);
  char* token = strtok(line, " ");

  for(lit_index = 0;lit_index < num_tokens; lit_index++)
    {            
      literal lit;
      char* lastChar;
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
      lits_seen[val]++;
      lits[lit_index] = lit;
      token = strtok(NULL, " ");
    }
  
  c.len = num_tokens;
  c.lits = lits;
  c.is_satisfied = 0;
  return c;
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

main(int argc, char** argv)
{
  int error_val = 0;
  formula f = verify(argc, argv, &error_val);
  
  if(!error_val)
    {
      printf("Maybe not an error?\n");
      print_formula(f);
      printf("num_lits: %i, num_clauses: %i.\n", f.num_lits, f.num_clauses);
    }
  else {
    printf("Prolly an error.\n");
  }
}
