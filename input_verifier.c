#include <stdio.h>
#include <stdlib.h>

/**
 * Determine that the supplied file is a valid format
 *
 * Returns 0 if valid, and 1 otherwise
 **/
int verify(int argc, char** argv)
{
  FILE* input;
  char curr_line[4096];
  int problem_is_defined = 0;
  long num_params = 0;
  long num_statements = 0;
  long statements_read = 0;
  char* prob_start, cnf;
  
  if(argc != 2) 
    {
      return 1;
    }
  
  input = fopen(argv[1], "r");
  if (input == NULL) 
    {
      return 1;
    }
  
  // Verify file contents are valid CNF
  while(fgets(curr_line, sizeof(curr_line), input)) 
    {
      // If p line has been read, verify statements
      if(problem_is_defined) 
	{
	  if(is_statement_invalid(curr_line, num_params))
	    {
	      return 1;
	    }
	  continue;
	}

      // Ignore leading comments
      else if(curr_line[0] == 'c') 
	{
	  continue;
	}
      
      // Parse line starting with 'p'
      else if(curr_line[0] == 'p')
	{
	  // TODO: Check return value.
	  sscanf(curr_line, "%s %s %i %i", 
	         prob_start, 
		 cnf, 
		 &num_params,
		 &num_statements);

	  // Verify all parts of p line were correct
	  if(strcmp("p", prob_start) ||
	     strcmp("cnf", cnf) || 
	     num_params <= 0 ||
	     num_statements <= 0)
	    {
	      return 1;
	    }
	  problem_is_defined = 1;
	}
	
    }
}

int is_statement_invalid(char* line, long num_params)
{
  // Verify line is valid statement/term
}
