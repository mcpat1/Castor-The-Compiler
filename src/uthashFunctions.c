//////////////////////////////////////////////////
//File: uthashFunctions.c
//Author: Troy D. Hanson <<tdh@tkhanson.net>
//Modified By: Patrick McElvaney <mcpat1@umbc.edu>
//Born on Date: 2/27/15
//Date Modified: 2/26/15
//Description: 
/** This file contains the function calls to set/get variables in/from
* the the hash table developed by the author all code in this file can be * found at http://troydhanson.github.io/uthash/userguide.html
* with slight modifications to fit the Castor compiler
*
* NEED TO ADD MALLOC CHECKING
*/
/* <<<
/ * This is used by uthash.h to store key, value pairs in hash table * /
struct uthash_struct {
	char * name; / * UTHASH: we'll use this field as the key * /
	Type *mapped_value; 
	UT_hash_handle hh;/ * makes this structure hashable * /
}
*/
//global_symtbl is on its last leg. remove in proj4
//uthash_struct *global_symtbl = NULL; //important!! must initialize to NULL
struct uthash_scope_struct *scope_symtbl = NULL; //hash for all scope lookups
struct uthash_struct *local_symtbl = NULL; //hash for current scope's variables!!!
struct uthash_scope_struct *currentScope; 
struct uthash_scope_struct *globalScope; //for returning to global scope
//???????????????????????????????????????????????????????????????????

/* Returns Type*(value) of char* name(key) */
Types *find_symbol(char *name){
	struct uthash_struct *s;
	//currentScope->mapped_scope = NULL;
	
	HASH_FIND_STR( currentScope->mapped_scope, name, s);  /* id already in the hash? */

	if(s == NULL){
		return NULL;
	}	
	return s->mapped_value;
}


Types *insert_symbol(char *name) {
	struct uthash_struct *s;
    //Type *returnPtr; <--use when malloc checking in future

	/* id already in the hash? */
    HASH_FIND_STR( currentScope->mapped_scope, name, s); 
    if (s==NULL) {
		s = (struct uthash_struct*)malloc(sizeof(uthash_struct));
		//return of -1 here if malloc fail
		if ( s == NULL ){
			printError( "Malloc in insert_symbol failed", 3);
		}			
		strcpy(s->name, name);
		
		/* id: name of key field */
		HASH_ADD_STR( currentScope->mapped_scope, name, s );  
		//return of Type * if successful addition to symbol table, o.w. return 0
		//may want to return the uthash_struct * in future

		//HASH_FIND_STR( currentScope->mapped_scope, name, s);
		//printf("hash find str ptr: %p\nname: %s\n", s, s->name);
		s->mapped_value = (Type *) malloc(sizeof(Type *));
		if ( s->mapped_value == NULL ){
			printError( "Malloc in insert_symbol failed", 3);
		}
		return s->mapped_value;
    }
	 ////////////NEW//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else{
		 /* do last check in currentScope->arg_head */
		 struct arg_var_struct *itr, *tmp;
		 itr = currentScope->arg_head; 
		int found = 0;
		while( itr != NULL ){
			 /*traverse ll*/
			if( strcmp( itr->name, name ) == 0 ){
				/* match found */
				found = 1;
				/* first remove all entries with id */
				tmp = itr;
				if( itr->next != NULL ){
					itr = itr->next;
					
					continue;
				}
				tmp->next = NULL;
				free( tmp );
				tmp = NULL;
								
			}
		}
		if( found ){
			/* then return the pointer */
			return find_symbol( name );
		}
	}
	printError( "Variable was previously initialized!", 1);
	 //return 0;
	 /* Consider returning Type *, and catch with an if true continue; if var already exists return 0, malloc error return -1*/
	 //then handle with and if (0) printError( "VARIABLE WAS PREVIOUSLY INITIALIZED!", 1);
	 //                     else (-1) printError( "Malloc of identifier struct failed", 3);
	return NULL;    
}


////////////////////NEW -------------- NEW ///////////////////////////////

void new_hashed_scope() {

	//struct uthash_struct *s = NULL; //new scopes var symtbl hash, must = NULL!!
	struct uthash_scope_struct *s;
	int error_check;
	/* id will always be in hash, so don't HASH_FIND!!! */
    //HASH_FIND( sh, scope_symtbl, currentScopeStr, strlen(currentScopeStr), s);  
	//printf( "%p\n", s );
    //if (s == NULL) {
		
		s = (struct uthash_scope_struct*)malloc(sizeof(uthash_scope_struct));
		/* Set parent location: scope_t ptr */
		s->parentScope = currentScope; //dereference curScope ptr(* or &)?
		currentScope = s;
		error_check = sprintf( currentScopeStr, "scope%d", scopes_in_use);
		if (error_check == -1){
			printError( "Malloc of sprintf failed", 3);
		}	
		//might need stpcpy ( , ) ...see scanner.l !!!cld be NULL @free(curScopeStr)
		strcpy( s->scope, currentScopeStr );
	/* Set FILE *fp *///set hashed scope fp and update curDataFile global var
		curDataFile = s->fp = fopen( currentScopeStr , "w+" );
		if( curDataFile == NULL ){
			printError( currentScopeStr , 5);
		}
		s->mapped_scope = NULL; //hash symtbl for this scope's ids 
		/* AS REFERENCE:
		struct arg_var_struct {
			struct arg_var_struct *next; /* dbly linked list * /
			char name[255]; /* parameter id * /
		};
		*/
/**/	s->arg_head = NULL;
		//s->arg_head->name[0] = NULL; /*just to be safe, but will never be accessed*/
		HASH_ADD_KEYPTR( sh, scope_symtbl, s->scope, strlen(s->scope), s ); 
		
///////////////////////////////////////////////////////////////////////////////
	//THIS LIFO MAY REDUCE SIZE OF MALLOC in main OR for tracking FUNCTION CALLS
		stackForScopes[ ++currentScopeIndex ] = scopes_in_use;
		//success...
		return;
    //}
	//something went wrong... 
	 printError( "\nCompiler crapped itself while initializing a new scope...\n", 1);
}

/* clean sweep of allocated memory of a single symbol table */
void delete_symbol_table() {
	
  struct uthash_struct *symbol_instance, *tmp;
	/* HASH_ITER is a macro that acts like a for loop with an iterator */
  HASH_ITER(hh, currentScope->mapped_scope, symbol_instance, tmp) {
	  /* returns a pointer to the key*/
    HASH_DEL(currentScope->mapped_scope, symbol_instance);  /* delete; cur_symtbl advances to next */
	/* Is this necessary? All symbol_vars must have an id which is ->attr->name */

	//if (symbol_instance->mapped_value->attr != NULL){
		//free(symbol_instance->mapped_value->attr);/* free Attributes struct */
		//symbol_instance->mapped_value->attr = NULL;
	//}

	//if (symbol_instance->mapped_value != NULL){
		//free(symbol_instance->mapped_value); /* free Type struct */
		//symbol_instance->mapped_value = NULL;
	//}
    //free(symbol_instance);            /* free hash table instance */
	//symbol_instance = NULL;
  }

  //IDK ABOUT THE STUFF BELOW THOOOOOO...........though it looks right
  HASH_DELETE(sh, scope_symtbl, currentScope);
  //free(currentScope);
}
void write_hashed_scope(){
	/* print out fd to asmfile ..currentScope->fp */

	rewind( curDataFile );
	fprintf( asmfile, "%s:\n", currentScopeStr);
	int c;
	while(1){
		c = fgetc(curDataFile);
		if(feof(curDataFile)){
			break;
		}
		fprintf( asmfile, "%c", c);
	}
/*START: attempt to return the modified value to parent scope*/
//	arg_var_struct *var_search = currentScope->arg_head;
//	/*check prevents functions and main program from modifying global vars*/
//	if( currentScope->parentScope->scope[5] != '0' ){
//		struct uthash_struct *s;
//		while( var_search != NULL ){
//			if( var_search->name != NULL ){
//				/*write the value of this variable to the value of the parent scope*/
//				/*as long as the parent scope is not 0*/
//				
//				HASH_FIND_STR( currentScope->mapped_scope, var_search->name, s);
//				if(s == NULL){
//					;/*something went wrong; should not happen*/
//				}	
//				switch( s->mapped_value ){
//				fprintf( textSec, "\n\tpush\tQWORD [%s.%s]\n", currentScopeStr, id);
//			}
//		}
//	}
		
/*END attempt*/	
	
	//fprintf( asmfile, "%s", cBUFF);

	/* close fp */
	//fclose( curDataFile );

	//remove( currentScopeStr );

	/*uthash_scope_struct *temp = &currentScope */
	uthash_scope_struct *temp = currentScope->parentScope;
	/* call delete symbol table */
	//delete_symbol_table();//uses

	/* move parent scope to current scope. currentScope */
	currentScope = temp;
	/* update currentScopeStr */
	strcpy( currentScopeStr, currentScope->scope);

	/* update FILE* curDataFile */
	curDataFile = currentScope->fp;
	/* decrement currentScopeIndex */
	--currentScopeIndex;

}

void insertGlobalDeclarations(){
	fprintf( textSec, ";start initialization of global variables\n;");
	rewind( globalTextSec );
	int c;
	while(1){
		c = fgetc( globalTextSec );
		if(feof(globalTextSec)){
			break;
		}
		fprintf( textSec, "%c", c);
	}
	fprintf( textSec, ";\n;end initialization of global variables\n");
	/*this is only one instance. work to remove this file then apply to write_hashed_scope() and delete_symbol_table()*/
}
