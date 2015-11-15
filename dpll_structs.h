#ifndef DPLL_STRUCTS
#define DPLL_STRUCTS
typedef struct literal{
	//Variable read from file
	int id;
	//Is positive if true 
	int is_pos;
	int is_assigned;
	int val;
	int eval;
}literal;

typedef struct clause{
	int len;
	literal *lits;
	int is_satisfied;
}clause;

typedef struct formula{
	int num_clauses;
	clause *clauses;
	int num_lits;
	literal *all_lits;
}formula;
#endif
