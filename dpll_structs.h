#ifndef DPLL_STRUCTS
#define DPLL_STRUCTS

/**
 * lit_count allows literals to be tracked by frequency, for future sorting 
 **/
typedef struct lit_count {
  int id;
  int count;
} lit_count;

typedef struct clause_index {
  int clause;
  int index;
} clause_index;

typedef struct literal{
  //Variable read from file
  int id;
  //Is positive if true 
  int is_pos;
  int is_assigned;
  int val;
  int eval;
}literal;

typedef struct lit_clauses{
  literal lit;
  int num_clauses;
  int cur_clause;
  clause_index * clauses;
} lit_clauses;

typedef struct clause{
  int len;
  literal *lits;
  int is_satisfied;
}clause;

typedef struct formula{
  int num_clauses;
  clause *clauses;
  int num_lits;
  lit_clauses * all_lits;
}formula;
#endif
