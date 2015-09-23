//////////////////////////////////////////////////
//File: declarationsAndAssignments.c
//Author: Patrick McElvaney
//Born on Date: 2/26/15
//Date Modified: 2/26/15
//Description: 
//
//
/* Thought that return type should be void bc no expr ever assigned */
Type *initializeInt(char *id){
	/* Check if declared via lookup in symbol table */
	// returns Type * initialized in table if no record, o.w. 0
	Type *structPtr;
	//this might be the problem, sol: id is a malloc'd ptr. use ->->name
	/*strcpy(structPtr->attr->name, id);*/
	 //if (structPtr = insert_symbol(id/*structPtr->attr->name*/)){
		 structPtr = insert_symbol(id/*structPtr->attr->name*/);
		 /* initialize record */
		 
		if((structPtr->attr = (Ident *) malloc(sizeof(Ident *))) == NULL){
			printError( "Malloc of identifier struct failed", 3);
		}
		if( FLAG_FUN)
		structPtr->longval = NULL;
		structPtr->type = 300;
		strcpy(structPtr->attr->name, id);

		/* asm code */
		fprintf( curDataFile , ".%s:\tdq  0  ;intVar\n", id);
		if( FLAG_FUN ){
			structPtr->attr->offset = 8 * fun_body_offsets++;
			fprintf( textSec, "\tpush\tQWORD 0\n");
			return structPtr;
		}
		if( currentScopeStr[5] != '0' ){ 
			fprintf( textSec, "\n\tpush\tQWORD [%s.%s]\n", currentScopeStr, id);
			return structPtr;
		}
		fprintf( globalTextSec, "\n\tpush\tQWORD [%s.%s]\n", currentScopeStr, id);
/*START: attempt to return the modified value to parent scope*/
//	arg_var_struct *var_search = currentScope->arg_head;
//	while( var_search != NULL ){
//		if( strcmp( var_search->name, id ) ){
//			/*no match at this instance*/
//			/*move to next iteration*/
//			var_search = var_search->next;
//			continue;
//		}
//		var_search->name = NULL; /*remove var id from list*/
//		var_search = var_search->next;
//	}
/*END attempt*/
		return structPtr;
	 //}
	 /* else */
	// printError( "Variable was previously initialized!", 1);
}

Type *initializeFloat(char *id){
	/* Check if declared via lookup in symbol table */
	// returns Type * initialized in table if no record, o.w. 0
	Type *structPtr;
	// if (structPtr = insert_symbol(id)){
		structPtr = insert_symbol(id);
		 /* initialize record */
		if((structPtr->attr = (Ident *) malloc(sizeof(Ident *))) == NULL){
			printError( "Malloc of identifier struct failed", 3);
		}
		structPtr->dubval = NULL;
		structPtr->type = 301;
		strcpy(structPtr->attr->name, id);
		/* asm code */
		fprintf( curDataFile, ".%s:\tdq  0  ;floatVar\n", id);
		if( FLAG_FUN ){
			structPtr->attr->offset = 8 * fun_body_offsets++;
			fprintf( textSec, "\tpush\tQWORD 0\n");
			return structPtr;
		}
		if( currentScopeStr[5] != '0' ){ 
			fprintf( textSec, "\n\tpush\tQWORD [%s.%s]\n", currentScopeStr, id);
			return structPtr;
		}
		fprintf( globalTextSec, "\n\tpush\tQWORD [%s.%s]\n", currentScopeStr, id);
/*START: attempt to return the modified value to parent scope*/
//	arg_var_struct *var_search = currentScope->arg_head;
//	while( var_search != NULL ){
//		if( strcmp( var_search->name, id ) ){
//			/*no match at this instance*/
//			/*move to next iteration*/
//			var_search = var_search->next;
//			continue;
//		}
//		var_search->name = NULL; /*remove var id from list*/
//		var_search = var_search->next;
//	}
/*END attempt*/
		return structPtr;
	//}
	 /* else */
	// printError( "Variable was previously initialized!", 1);
}

void varAssignment(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	if( lhs->type == rhs->type ){
		/* assign val to apropriate struct member */
		switch(lhs->type){
			case 300:

			if( FLAG_FUN ){
				fprintf( textSec, "\tpop\tQWORD [rbp - %d]\n", lhs->attr->offset);
				return;
			}
			/* Update symbol table */
			lhs->longval = rhs->longval;
			/* asm code */
			if( currentScopeStr[5] != '0' ){ 
				fprintf( textSec, "\tpop\tQWORD [%s.%s]  ;int var assignment\n",currentScopeStr, lhs->attr->name);
			} else {
					fprintf( globalTextSec, "\tpop\tQWORD [%s.%s]  ;int var assignment\n",currentScopeStr, lhs->attr->name);
				}
			/* 
			* NOTE(future thinking): we do not set other types to 
			* NULL since they may have a value of a different type 
			*/
			break;
			case 301:
			lhs->dubval = rhs->dubval;
			/* asm code */
			if( FLAG_FUN ){
				fprintf( textSec, "\tpop\tQWORD [rbp - %d]\n", lhs->attr->offset);
				return;
			}
			if( currentScopeStr[5] != '0' ){ 
				fprintf( textSec, "\tpop\tQWORD[%s.%s]  ;float var assignment\n",currentScopeStr, lhs->attr->name);
			} else {
					fprintf( textSec, "\tpop\tQWORD[%s.%s]  ;float var assignment\n",currentScopeStr, lhs->attr->name);
				}
			/* 
			* NOTE(future thinking): we do not set other types to 
			* NULL since they may have a value of a different type 
			*/
			break;
		}
		//check name: if NULL, not id so malloc...
		//if(expr->attr == NULL)
			//free malloc
			/*free(expr);*/
		/* clean up pointer */
		////////////////idPtr = NULL;
		return;
	}
	printError( "Type mismatch during assignment", 4);
	
}
