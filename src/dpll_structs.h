#ifndef DPLL_STRUCTS
#define DPLL_STRUCTS

/**
 * lit_count allows literals to be tracked by frequency, for future sorting 
 **/
typedef struct lit_count {
  int id;
  int count;
} lit_count;

/**
 * clause_index tracks two things:
 * clause - which clause the associated literal appears in
 * index - where in the clause's lits this literal appears (index)
 **/ 
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
  int index;
}literal;

/**
 * lit_clauses tracks a literal's positions in the formula
 * lit - the literal being tracked
 * num_clauses - the number of clauses in which this literal appears
 * cur_clause - used when filling this struct, NOT RELEVANT FOR EXTERNAL USE
 * clauses - the clause_indexes identifying which clauses this literal appears in,
 *           and their associated indexes
 * 
 * To iterate through a literal's clauses, use the following:
 * 
 * formula f;
 * lit_clause lc = all_lits[n]; // Whatever literal/lit_clause you're on at the moment.
 * int i;
 * // Loop through the clause_indexes
 * for(i = 0; i < lc.num_clauses; i++)
 * {
 *   clause_index ci = lc.clauses[i];
 *   // Access the clause at ci.clause, and get the literal at ci.index
 *   literal lit_to_work_on = f.clauses[ci.clause].lits[ci.index]; 
 * }
 * 
 **/
typedef struct lit_clauses{
  literal lit;
  int num_clauses;
  int cur_clause;
  int purity;
  int num_unsatisfied;
  clause_index * clauses;
} lit_clauses;

typedef struct clause{
  int len;
  int num_unassigned;
  literal *lits;
  int is_satisfied;
}clause;

typedef struct formula{
  int num_clauses;
  clause *clauses;
  int num_lits;
  lit_clauses * all_lits;
  int num_unsatisfied;
  int has_empty_clause;
  int potential_unit_count;
  int potential_pure_count;
  int* potential_units;
  int* potential_pures;
}formula;
#endif

