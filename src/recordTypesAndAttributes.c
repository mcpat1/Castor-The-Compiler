// File: recordTypesAndAttributes.c
// Author: Patrick McElvaney
// Born on Date: 2.24.15
// Date Modified:
// Description:
/*
* This file contains the records (structs) for all Basic Types and their attributes.  Basic types at this time include { INTEGER, FLOAT } .Complex types will be ammended in version 2.0. 
*/
typedef struct Identifications{
	
	/* variables are limited to a size of 255 characters in length*/
	char name[255]; //VAR ID OR IF SCOPE, localScope ID
		/* for scope, use stringStream, write new variables to its local scope
			stream and write to asm file at Closing boilerplate*/
	/* all local variables are stored in their own unordered_map object */
	//char * parentScope;
	int offset;	/* for stack lookups when inside functions*/
	int length; 
} Ident;

typedef struct Types {
		/* pointer to additional attributes */
	Ident *attr;
	/* INTEGER == 300, FLOAT == 301, STRING == 302, BOOL == 303 
										defined in top of parser.yy */
	int type;
	//int offset;	/* for stack lookups when inside functions*/
	/* need to store value when record is assigned to bison stack*/
	long longval;
	double dubval;	
	char * strval;
	//i;
} Type;

/* This is used by uthash.h to store key, value pairs in hash table -VARIABLES- */
struct uthash_struct {
	char name[255]; /* UTHASH: we'll use this field as the key */
	Type *mapped_value; 
	UT_hash_handle hh;/* makes this structure hashable */
};

/* to complete closure of scopes, we must keep track of vars that are passed in and when assigning, if var is of arguments then the last check in uthashFunctions insertSymbol*/
struct arg_var_struct {
	struct arg_var_struct *next; /* linked list */
	char name[255]; /* parameter id */
} LL_ID;

/* This is used by uthash.h for scope resolution -SCOPES- */
struct uthash_scope_struct {
	uthash_struct *mapped_scope;  //var symtbl hash
	uthash_scope_struct *parentScope; //pointer to parent_scope
	/* IMPORTANT!!!!!!!!!!!!! -> when searching for parent scoped vars, currentScope has the symtbl to return to...keep recursing up parentScope until
	parentScope->mapped_scope == NULL (Which is the global scope) if not found, then printError( "INVALID ACCESS OF UNINITIALIZED VARIABLE!", 1); */
	//NOTE FOR ABOVE MEMBER: to use 
	               //HASH_FIND_STR( scope_t->parentScope->mapped_scope, name, s)
	
	FILE *fp; //ptr to scope file for writing var id's, strings, float vals
	arg_var_struct *arg_head; /* head of arg linked_list for closure, always initialized at NULL*/
	char scope[30]; // key identifier of scope in rest of prog
	
	UT_hash_handle sh;/* makes this structure hashable */
};
/*
struct parameters {
	char id[255];
	parameters *next;
};*/	
/* similar to uthash_struct but has an offset value for each variable relative to ebp.  parameters are ebp + 4 + (4 * arg position) [EXAMPLE]: function( a, b, c); where a == 3, b == 2, c == 1. Local vars are ebp - ( 4 * counter at initialization).  

don't need a parent scope becuase the only scope referenced in a definition is itself and the global scope which is always scope0

no need for arg_var_struct either. cannot initialize a var of the same name as a parameter.

FILE * needed for string literals and floating point values.
*/
/*struct function_var_lookup {
	char name[255]; /* UTHASH: we'll use this field as the key * /
	Type *mapped_value; 
	int offset;	/* for stack lookups when inside functions* /
	UT_hash_handle fv;/* makes this structure hashable * /
}*/
/*  */
struct signature {
	int param_type[13]; /* can take 8 params and a ret [0] is ret index, [1-8] are passed parameters */
	//arg_var_struct//parameters *p_head; /* mapping to params */
	uthash_scope_struct *fun_sym_tbl; /* symtbl */
	signature *next; /* for multiple signatures */
	int args; /*for signature matching*/
	//int brothers; /*for labeling */
};
	
/*
 this records information about functions declared before main. There is only
 one skeleton per function, the state is saved on the stack if recursion or
 nested functions are exhibited
 */
struct uthash_function_struct {
	//uthash_struct *fun_vars; //local data
	char fun_name[255]; /* UTHASH key for function lookup. */
	signature *s_head; /*linked list*/
	//in uthash_scope_struct//char scope[30]; // key identifier of scope in rest of prog
	int nest_depth; /* not necessary but more for debugging and statistics */ 
	UT_hash_handle fh; /* c's version of object oriented programming via linker variable */
}FUNC;
	//int args; /*for signature matching*/
/*NEED TO CHANGE THIS TO A SIMPLE param_type[9]*/
struct build_fun {
	Type *arg;
	build_fun *next;
};
