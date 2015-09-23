// File: recordTypesAndAttributes.c
// Author: Patrick McElvaney
// Born on Date: 4.11.15
// Date Modified:
// Description:
/*
/* This is used by uthash.h for scope resolution -SCOPES- 
*/
struct uthash_function_struct *fun_hash = NULL; //head of hash for element lookups
//struct build_fun *global_arg_list = NULL; //for exacting appropriate functions
int g_params[13] = {0};
//int num_args_to_match = 0; //attribute of global_arg_list
int fun_arg_offsets; //that point to location above rbp

/*

need to work on reducing dereferences when initializing function
*/
void insertFunction(char* id, int t){
	/* always need to hop over rbp.*/
	fun_arg_offsets = 1; 

	uthash_function_struct *fun;
	HASH_FIND(fh, fun_hash, id, strlen(id), fun);
	if( fun == NULL ){
		/*HASH_FIND_STR if NULL, then add the function name, o.w. only initialize 	signature*/
		fun = (struct uthash_function_struct*)malloc(sizeof(uthash_function_struct));
		if( fun == NULL){
			printError( "Malloc failure", 3);
		}
		fun->nest_depth = 0;
		strcpy(fun->fun_name, id); //give new function its name
		if( (fun->s_head = (struct signature*)malloc(sizeof(signature))) == NULL){
			printError( "Malloc failure", 3);
		}
		fun->s_head->next = NULL;
		HASH_ADD_KEYPTR( fh, fun_hash, fun->fun_name, strlen(fun->fun_name), fun ); /*consider a smaller hashable struct than uthash_function_struct*/ 
	}
	else{
		struct signature *tmp_s = fun->s_head;

		if( (fun->s_head = (struct signature*)malloc(sizeof(signature))) == NULL){
			printError( "Malloc failure", 3);
		}

		fun->s_head->next = tmp_s;
	}
	fun->s_head->param_type[ 0 ] = t; //set type

	++scopes_in_use; //HACK
	new_hashed_scope();
	fun->s_head->fun_sym_tbl = currentScope;

	++(fun->s_head->args); //return is an arg

	/* asm code */

	fprintf( textSec, "%s%s:\n", fun->fun_name, fun->s_head->fun_sym_tbl->scope );
	fprintf( textSec, "\tpush\trbp\n\tmov\trbp, rsp\n");
	/*assign fun to current_fun global pointer*/
	current_fun = fun;

}

Type *insertParameter(char* id, int t){
	
	uthash_struct *s; //obj to find sym_tbl 
	/* already a parameter with same id in the hash? */

    HASH_FIND_STR( currentScope->mapped_scope, id, s); 
	if (s==NULL) {
		s = (struct uthash_struct*)malloc(sizeof(uthash_struct));
		//return of -1 here if malloc fail
		if ( s == NULL ){
			printError( "Malloc in insertParameter failed", 3);
		}			
		strcpy(s->name, id);
		arg_var_struct *tmp;
		tmp = currentScope->arg_head;
		if( (currentScope->arg_head = (struct arg_var_struct*)malloc(sizeof(arg_var_struct))) == NULL ){
			printError( "Malloc failure", 3);
		}
		current_fun->s_head->param_type[ current_fun->s_head->args ] = t;
		strcpy( currentScope->arg_head->name, id );
		++(current_fun->s_head->args);
		currentScope->arg_head->next = tmp; //append the linked list
		
		
		HASH_ADD_STR( currentScope->mapped_scope, name /*id*/, s );
		s->mapped_value = (Type *) malloc(sizeof(Type));
		if ( s->mapped_value == NULL ){
			printError( "Malloc in insert_symbol failed", 3);
		}
		if((s->mapped_value->attr = (Ident *) malloc(sizeof(Ident))) == NULL){
			printError( "Malloc of identifier struct failed", 3);
		}
		/* QWORD * arg index + 8 to get to rbp offset */

		//s->mapped_value->attr->offset = -( 8 * fun_arg_offsets + 8 ); //NEW!!!!!!!!!!!!!!!!!!!!!!!!!!
		//--fun_arg_offsets;
		if( t == 300 ){
			s->mapped_value->longval = NULL;
			s->mapped_value->type = 300;
			fprintf( curDataFile , ".%s:\tdq  0  ;intVar\n", id);
		}
		else {
			s->mapped_value->dubval = NULL;
			s->mapped_value->type = 301;
			fprintf( curDataFile, ".%s:\tdq  0  ;floatVar\n", id);
			}
		strcpy(s->mapped_value->attr->name, id);
		/* asm code */
		/*if( current_fun->s_head->args == 2 ){
		fprintf( textSec, "\tmov\trsp, rbp\n\tsub\trsp, 8\n");
		} else {
				fprintf( textSec, "\tsub\trsp, 16\n");
			}
		fprintf( textSec, "\tpop\tQWORD [%s.%s]\n", currentScopeStr, id);*/
		/* finally done */
		return s->mapped_value;
	}
	printError( "Cannot have two parameters with the same identification", 1);
	
}
void writeOffset(Type* obj){
	
	obj->attr->offset = -( 8 * fun_arg_offsets + 8 );
	++fun_arg_offsets;
	
}
//bssID(){}
//bssWrite(){}
/* return pointer to specific function 
from: 
HASH_ADD_KEYPTR( fh, fun_hash, current_fun->fun_name, strlen(current_fun->fun_name), fun );

HASH_ADD_KEYPTR
(hh_name, head, key_ptr, key_len, item_ptr)

HASH_FIND
(hh_name, head, key_ptr, key_len, item_ptr)*/
Type *getSignature(char *name){
	


	struct uthash_function_struct *fun;
	HASH_FIND(fh, fun_hash, name, strlen(name), fun);
	if( fun != NULL ){


		struct signature *cur;
		cur = fun->s_head;
		while( cur != NULL ){

			if(cur->args - 1 != num_args_to_match ){
				/* quick test not matching...*/
				cur = cur->next;
				continue;
			}

			int i = 1;
			while( funArgCheck(g_params[i], cur->param_type[i], i)){

				/*args matched == args in function or functions without arguments*/
				if( i == num_args_to_match || num_args_to_match == cur->args - 1){
					/*signature matched*/
					
					fprintf( textSec, "\t;call %d %s (", fun->s_head->param_type[0], fun->fun_name);
					int i = 1;
					while( i++ < cur->args ){
						if( cur->param_type[i - 1] == 300 ){
						fprintf( textSec, " INT ");
						continue;
						}
						fprintf( textSec, " DOUBLE ");
					}
					fprintf( textSec, ")\n\tcall\t%s%s\n", fun->fun_name, cur->fun_sym_tbl->scope);
					fprintf( textSec, "\tadd\trsp, %d\n", num_args_to_match * 8);
					Type* retOBJ = NULL;
					switch( cur->param_type[0] ){
						case 0 :
							retOBJ = NULL;
							break;
						case 300:
							RET_VAL = 1;
							retOBJ = makeTempStruct( 300 );
							retOBJ->longval = 1;
							fprintf( textSec, "\tpush\trax\n  ;int fun retval assignment\n");
							break;
						case 301:
							RET_VAL = 1;
							retOBJ = makeTempStruct( 301 );
							retOBJ->dubval = 1.0;
							fprintf( textSec, "\tmovq\t[rsp], xmm0\n  ;float fun retval assignment\n");
							break;
						default:
							;
					}
					/*Because Castor does not support Functions in Interactive Mode
					functions will always return a 1 or 1.0. This may produce hazardous errors to the rest of the program.*/
					
					return retOBJ;
				}
				++i;
				
			}
			/*calling functions arguments does not match 
			current signatures params. Extensive testing*/
			cur = cur->next;
		}	
	}
	printError( name, 6 );
}

void appendArg(Type *obj){
	
	g_params[ ++num_args_to_match ] = obj->type;

}

void endFun(){
	FLAG_FUN = 0; /* turn off flag to locate variables as stack offsets */
	switch( current_fun->s_head->param_type[ 0 ] ){
		case 300:
			if( !RET_VAL ){ printError( "int", 8); }
			break;
		case 301:
			if( !RET_VAL ){ printError( "double", 8); }
			break;
	}
	fprintf( textSec, "\n\tmov\trsp, rbp\n\tpop\trbp\n\tret\n");
}
