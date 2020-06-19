//////////////////////////////////////////////////
//File: middleEnd.c
//Author: Patrick McElvaney
//Born on Date: 2/11/15
//Date Modified: 2/20/15
//Description: This file contains the function calls for producing
// intermediate assembly code for a simple infix calculator
// of the x86_64 architecture flavour.
//
// **need to clean up comments within C_boilerPlate
//


static int FPU_floats_in_use = 0; //use:(--,++) set {0-7} 8 register_max
static int floats_in_use = 0; //for creating unique labels
static int strings_in_use = 0; //for storing a string

/* language does not promote via widening types!!! */
void typeCheck( Type *lhs, Type *rhs){
	if(lhs->type == rhs->type)
		return; 
	/*50 points for type promotion*/
	if( lhs->type == 300){
		if(rhs->type == 301){
			lhs->type = 301;
			lhs->dubval = (double)lhs->longval;
			fprintf( curDataFile, ".f%d:\tdq  %f\n;type promotion\n", ++floats_in_use, lhs->dubval );
			fprintf( textSec, ";;;;replacing stack in type promotion\n\tmovq\txmm0, QWORD [rsp]\n\tadd\trsp, 8\n\t\n\tpush\tQWORD [%s.f%d]\n\tsub\trsp, 8\n\tmovq\t[rsp], xmm0\n", currentScopeStr, floats_in_use);
			return;
		}
	}
	if( rhs->type == 300){
		if(lhs->type == 301){
			rhs->type = 301;
			rhs->dubval = (double)rhs->longval;
			fprintf( curDataFile, ".f%d:\tdq  %f\n;type promotion\n", ++floats_in_use, rhs->dubval );
			fprintf( textSec, ";;;;replacing stack in type promotion\n\tadd\trsp, 8\n\tpush\tQWORD [%s.f%d]\n", currentScopeStr, floats_in_use);
			floats_in_use;
			return;
		}
	}

	printError( "Type mismatch", 4);
}
void retValCheck(int lhs , int rhs){
	if( lhs == rhs){
		switch( lhs ){
			case 300:
				fprintf( textSec, "\t;handle int return val\n\tpop\trax\n");
				break;
			case 301:
				fprintf( textSec, "\t;handle double return val\n\tmovq\txmm0, [rsp]\n\tadd\rsp, 8\n");
				break;
		}
		return;
	}
	if( rhs == 0 ){
			printError( "void", 7);
	} else if( rhs == 300 ){
			if( lhs == 301 ){
				/*type promotion*/
				fprintf( textSec, "\t;handle double return val\n\tmovq\txmm1, [rsp]\n\tadd\rsp, 8\n");
				/* do something to the integer in xmm0 */
				fprintf( textSec, "\t;LOOKS SCARY RIGHT? type promotion of a stack var return val\n\tCVTDQ2PD\txmm0, xmm1\n");
				return;
			}
			printError( "int", 7);
	} else {
			printError( "double", 7);
	}
}
/* used for arg checking and type promotion of types */
int funArgCheck(int g_param, int cur_param, int i){

	if (g_param == cur_param){
		return 1;
	}
	if (g_param == 300 && cur_param == 301){
		/*we can promote the argument*/
		i = ( num_args_to_match - i ) * 8;
		/*use num_args_to_match - i to find the stack offset to affect( * 8)*/
		fprintf( textSec, "\t;handle arg promotion\n\tmovq\txmm1, [rsp + %d]\n\tCVTDQ2PD\txmm0, xmm1\n\tmovq\t[rsp + %d], xmm0\n",i ,i );
		return 1;
	}
	/*cannot match or promote*/
	return 0;
}

/*prints the top portion of assembly, defines fprintf file middleEnd.asm*/
void O_boilerPlate(){
	globalTextSec = fopen( "globalTextSec", "w+" ); //for placing globals after main
  asmfile = fopen( global_file_output , "w+" );
  textSec = fopen( textPortion , "w+" );
  fprintf( asmfile, "\tsection .bss\n\n\tsection .data\n");
  fprintf( asmfile, "fmt_decimal_nl\t\tdb \"%%ld\", 10, 0\n");
  fprintf( asmfile, "fmt_decimal\t\tdb \"%%ld\", 0\n");
  fprintf( asmfile, "fmt_float_nl\t\tdb \"%%lf\", 10, 0\n");
  fprintf( asmfile, "fmt_float\t\tdb \"%%lf\", 0\n");
  fprintf( asmfile, "fmt_string_nl\t\tdb \"%%s\", 10, 0\n");
  fprintf( asmfile, "fmt_string\t\tdb \"%%s\", 0\n");
  fprintf( asmfile, "\nfloat_print_section:\n.x:\tdq  0  ;storage point for printing floats\nglobal_scope:\n.zero:\tdq 0 ;for float cmp\n");
  fprintf( textSec, "\n\tsection .text\n");
  fprintf( textSec, "\nextern printf\nextern scanf\nextern pow\nglobal main\n");
  fprintf( textSec, "\nloopPow:\n\tpush\trbp\n\tmov\trbp, rsp\n\tmov\tr8, rdi\n\tdec\trcx\n\tjz\t.loopPowExit ;check if exponent is 1\n");
  fprintf( textSec, ".tightLoop:\n\timul\trdi, r8\n\tloop\t.tightLoop\n");
  fprintf( textSec, ".loopPowExit:\n\tmov\trax, rdi\n\tpop rbp\n\tret\n");
	/* moved to parser.yy */
  //fprintf( textSec, "\nmain:\n\n\tpush\trbp\n\tmov\trbp, rsp\n");
}

/* middleman for creating new scope */
void newScope(){
	//stackForScopes[ currentScopeIndex ] = scope_index_val;
	//in uthasFunctions
	new_hashed_scope();
	//cpp function
	//addScopeStack();
}
/* middleman for closing scope, write vars to data section */
void endOfScope(){
	write_hashed_scope();
	//long scope;
	//cpp function
	//scope = writeScopeStack();
	
}

/* 
*  An ID is just a char * until it gets reduced. So we have a char *
*  and not a Type *  --> Returns: Type *
*/
Type *getID(char *id){
	Type *returnPtr;
	//lookup id in table

	if ( (returnPtr = find_symbol(id)) != NULL){
		//printf("id val out is %d\n", returnPtr->longval);
		/* asm code */
		if( FLAG_FUN && returnPtr->attr->offset != 0){

			fprintf( textSec, "\tpush\tQWORD [rbp - %d] ;push value of %s\n", returnPtr->attr->offset, id);//!!!!!!!!!!!!????????????!?!?!?!?!?!?!PPPPPPPPPPPPPPPPDDDDDDDQQQQQQQQQQQQ
		} else {
			fprintf( textSec, "\tpush\tQWORD [%s.%s] ;push value of .%s\n", currentScopeStr, id, id);
			//return id's struct pointer from symtbl to bison stack
		}
		return returnPtr;
	}

	int i = currentScopeIndex;
	struct uthash_struct *s, *d;
	uthash_scope_struct *temp = currentScope->parentScope;
/*NOTE changed to i-- from i++ on 4/13/15*/
	while( i-- > 0 ){
		
		HASH_FIND_STR( temp->mapped_scope, id, s);


		if(s != NULL){
			if( FLAG_FUN && s->mapped_value->attr->offset != 0){
				fprintf( textSec, "\t; push scoped local data\n\tpush\tQWORD [rbp - %d] ;push value of %s.%s\n", s->mapped_value->attr->offset, temp->scope, id);			
				return s->mapped_value;//QQQQQQQQQQQQQQQQQ
			}
			/* Note due to address of char* there may be problems when hash tables get deleted and uthash_structs get freed*/
			d = (struct uthash_struct*)malloc(sizeof(uthash_struct));
			if ( d == NULL ){
				printError( "Malloc in scope_resolution of getID failed", 3);
			}			
			strcpy(d->name, id);
			HASH_ADD_STR( currentScope->mapped_scope, name, d );
			switch(s->mapped_value->type){
				case 300:
					fprintf( curDataFile, ";scope resolution from %s\n.%s:\tdq  %ld\n", temp->scope , s->name, s->mapped_value->longval);
					d->mapped_value = (Type *) malloc(sizeof(Type *));
					if ( d->mapped_value == NULL ){
						printError( "Malloc in scope_resolution of getID failed", 3);
					}
					if((d->mapped_value->attr = (Ident *)malloc(sizeof(Ident *))) == NULL ){
						printError( "Malloc of identifier struct failed", 3);
					}
					strcpy( d->mapped_value->attr->name, id );
					d->mapped_value->type = 300;
					d->mapped_value->longval = s->mapped_value->longval;
					break;
				case 301:
					fprintf( curDataFile, ";scope resolution from %s\n.%s:\tdq\t%f  \n", temp->scope , s->name, s->mapped_value->dubval);
					d->mapped_value = (Type *) malloc(sizeof(Type *));
					if ( d->mapped_value == NULL ){
						printError( "Malloc in scope_resolution of getID failed", 3);
					}
					if((d->mapped_value->attr = (Ident *)malloc(sizeof(Ident *))) == NULL ){
						printError( "Malloc of identifier struct failed", 3);
					}
					strcpy( d->mapped_value->attr->name, id);
					d->mapped_value->type = 301;
					d->mapped_value->dubval = s->mapped_value->dubval;
					break;
			}
			
			/*if( SET_WHILE ){*/
				/*if( currentScopeStr[5] != '0' ){
					fprintf( textSec, "\tpush\tQWORD [%s.%s]\n", currentScope->parentScope->scope, id );
					fprintf( textSec, "\tpop\tQWORD [%s.%s]  ;int var assignment\n",currentScopeStr, id);
				}*/
			if( SAVE_ID ){
				fprintf( textSec, "\t; special \"while\" or inherited push condition\n\tpush\tQWORD [%s.%s] ;push value of .%s\n", currentScopeStr, id, id);
			//}
			
				/*struct arg_var_struct {
					struct arg_var_struct *next; /* dbly linked list * /
					char name[255]; /* parameter id * /
				};*/ 
				if( currentScope->arg_head == NULL ){
					if((currentScope->arg_head = (struct arg_var_struct*)malloc(sizeof(struct arg_var_struct*))) == NULL ){
						printError( "Malloc of LL_ID struct failed", 3);
					}
					currentScope->arg_head->next = NULL;
					strcpy( currentScope->arg_head->name, id );
				}
				else{
					/*NEED TO CHECK CURRENT LIST IF THERE ARE DUPLICATE ID's as parameters WHEN DELETING LL_ID entries, we avoid the overhead during insertion SO NOTE: duplicate id's are possible in this Linked List!!!!!*/
					struct arg_var_struct *tmpLL_ID = currentScope->arg_head;
					if((currentScope->arg_head = (struct arg_var_struct*)malloc(sizeof(struct arg_var_struct*))) == NULL ){
						printError( "Malloc of LL_ID struct failed", 3);
					}
					currentScope->arg_head->next = tmpLL_ID;
					strcpy( currentScope->arg_head->name, id );
				}
				
			}
			return d->mapped_value;
		}
		temp = temp->parentScope;
	}
	printError( "INVALID ACCESS OF UNINITIALIZED VARIABLE!", 1);
	 
	return NULL;
}


/*print results to stdout(Default)*/
void print_call(Type *var){
	switch( var->type){
	case 300:
		if(I_MODE){
			printf( "%ld", var->longval);
		}
		fprintf( textSec, "\n\n\t;int printf block\n\tmov\trdi, ");
		fprintf( textSec, "fmt_decimal\n\tpop\trsi\n");
		fprintf( textSec, "\tmov\tal, 0\n\tcall printf\n\n");
		break;
	case 301:
		if(I_MODE){
			printf( "%f", var->dubval);
		}
		fprintf( textSec, "\n\n\t;float printf block\n");
		fprintf( textSec, "\tpop\tQWORD [float_print_section.x]\n");
		
		fprintf( textSec, "\t;special 16 byte offset needed for printing floats\n");
		fprintf( textSec, "\tpush\trsp\n\tand\trsp, 0xfffffffffffffff0\n");
		
		fprintf( textSec, "\tmov\trdi, fmt_float\n\tmovq\txmm0, QWORD [float_print_section.x]\n");
		fprintf( textSec, "\tmov\tal, 1\n\tcall printf\n\n");
		
		fprintf( textSec, "\tpop\trsp\n");
		
		break;
	case 302:
		if(I_MODE){
			printf( "%s", var->strval);
		}
		fprintf( textSec, "\n\n\t;string printf block\n");
		fprintf( textSec, "\tmov\trdi, fmt_string\n\tpop\trsi\n");		
		fprintf( textSec, "\tmov\tal, 0\n\tcall printf\n\n");
		
		break;
	}
}
/*print results to stdout(Default) AAAAND carrage return*/
void printl_call(Type *var){
	switch( var->type){
	case 300:
		if(I_MODE){
			printf( "%ld\n", var->longval);
		}
		fprintf( textSec, "\n\n\t;int printf block\n\tmov\trdi, ");
		fprintf( textSec, "fmt_decimal_nl\n\tpop\trsi\n");
		fprintf( textSec, "\tmov\tal, 0\n\tcall printf\n\n");
		break;
	case 301:
		if(I_MODE){
			printf( "%f\n", var->dubval);
		}
		fprintf( textSec, "\n\n\t;float printf block\n");
		fprintf( textSec, "\tpop\tQWORD [float_print_section.x]\n");
		
		fprintf( textSec, "\t;special 16 byte offset needed for printing floats\n");
		fprintf( textSec, "\tpush\trsp\n\tand\trsp, 0xfffffffffffffff0\n");
		
		fprintf( textSec, "\tmov\trdi, fmt_float_nl\n\tmovq\txmm0, QWORD [float_print_section.x]\n");
		fprintf( textSec, "\tmov\tal, 1\n\tcall printf\n\n");
		
		fprintf( textSec, "\tpop\trsp\n");
		
		break;
	case 302:
		if(I_MODE){
			printf( "%s\n", var->strval);
		}
		fprintf( textSec, "\n\n\t;string printf block\n");
		fprintf( textSec, "\tmov\trdi, fmt_string_nl\n\tpop\trsi\n");
		fprintf( textSec, "\tmov\tal, 0\n\tcall printf\n\n");
		break;
	}
}

void scan_call(/*Type *fmt_string,*/ char *id){
		
	Type *var;
	if ( (var = find_symbol(id)) != NULL ){
		/* asm code */
		fprintf( textSec, "\tpush\tQWORD %s.%s ;push value of .%s\n", currentScopeStr, id, id);
		switch( var->type){
		case 300:
			if(I_MODE){
				scanf( "%ld", &var->longval );
			}
		
			fprintf( textSec, "\n\n\t;scanf block\n");
			fprintf( textSec, "\tmov\trdi, fmt_decimal\n\tpop\trsi\n");
			fprintf( textSec, "\tmov\tal, 0\n\tcall scanf\n\n");
			break;
		case 301:
			if(I_MODE){
				scanf( "%lf", &var->dubval );
			}
			fprintf( textSec, "\n\n\t;scanf block\n");
			fprintf( textSec, "\tmov\trdi, fmt_float\n\tpop\trsi\n");
			fprintf( textSec, "\tmov\tal, 0\n\tcall scanf\n\n");
			break;

		}
		return;
	}
	printError( "UHOH INVALID ACCESS OF UNINITIALIZED VARIABLE!", 1);
	
}

void popGlobalStack(){
	fprintf( globalTextSec, ";just an expressing so take val off stack\n");
	fprintf( globalTextSec, "\tadd\trsp, 8\n");
}

void popStack(){
	fprintf( textSec, ";just an expressing so take val off stack\n");
	fprintf( textSec, "\tadd\trsp, 8\n");
}

/*program is exiting, restore registers that were swept under rug*/
void C_boilerPlate(){
	if( error_occured ){
		//disallow executable to be produced
		remove(global_file_output);
		return;
	}
	//Write the global scope to .data SECTION
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
	
	/* close fp */
  fprintf( textSec, "\n\n\t;clean up and get out of here\n");
  fprintf( textSec, "\tmov\trsp, rbp\n");
  fprintf( textSec, "\tpop\trbp\n\tret\n");

  if( I_MODE ){
		
		rewind( textSec );
		int c;
		while(1){
			c = fgetc(textSec);
			if(feof(textSec)){
				break;
			}
			fprintf( asmfile, "%c", c);
		}
		//remove(scope*);
		//remove(globalTextSec);
	}
  //important!
  fclose(textSec);
  //cat textSec >> asmfile
  /*textSec = fopen(textPortion, "r");
  if( ferror(textSec) )
	printError( "Could not open textSection.asm to read", 1);
	while(
	*/
  //also important
  fclose(asmfile);

 }

/*push INTEGER onto stack, return struct Type ptr */
Type *intNUM(long num){
	/* initialize record */
	
	Type *structPtr;
	//check return val for malloc
	if((structPtr = (Type *) malloc(sizeof(Type *))) == NULL)
		printError( "Malloc of int type struct failed", 3);
	
	structPtr->longval = num;
	structPtr->dubval = (double)NULL;
	structPtr->strval = NULL;
	structPtr->type = 300; //enum val for INTEGER
	structPtr->attr = NULL; //pointer to Attributes(additional info)
	/* asm write */
	fprintf( textSec, "\tpush\tQWORD %ld\n", num );
	return structPtr;
}
/*push FLOAT onto the FPU register stack*/ //NOT TESTED
Type *floatNUM(double num){
	/* !!!future application:
	make PFU_floats_in_use a locking thread 
	avoiding race conditons */
	Type *structPtr;
	//check return val of malloc
	if((structPtr = (Type*) malloc(sizeof(Type*))) == NULL)
		printError( "Malloc of float type struct failed", 3);
	
	structPtr->dubval = num;
	structPtr->longval =(long) NULL;
	structPtr->strval = NULL;
	structPtr->type = 301; //enum val for FLOAT
	structPtr->attr = NULL; //pointer to Attributes(additional info)
	/* asm code */
	fprintf( curDataFile, ".f%d:\tdq  %f\n", floats_in_use, num);
	fprintf( textSec, "\tpush\tQWORD [%s.f%d]\t;%f\n", currentScopeStr, floats_in_use++, num);
	return structPtr;
	
}

Type *makeTempStruct(int type){
	Type *tempStruct;
	if((tempStruct = (Type *) malloc(sizeof(Type *))) == NULL)
		printError( "Malloc of int type struct failed", 3);
	tempStruct->longval =(long)NULL;
	tempStruct->dubval = (double)NULL;
	tempStruct->strval = NULL;
	tempStruct->attr = NULL; //pointer to Attributes(additional info)
	switch (type){
		case 0:
			tempStruct->type = 0; //enum val for VOID
			break;
		case 300:
			tempStruct->type = 300; //enum val for INTEGER
			break;
		case 301:
			tempStruct->type = 301; //enum val for DOUBLE
			break;
		case 302:
			tempStruct->type = 302; //enum val for STRING
			break;
		case 303:
			tempStruct->type = 303; //enum val for BOOL
	}
	return tempStruct;
}
/*
void interactiveIf(Type* ifExpr){
	switch( ifExpr->type ){
		case 300:
			ifExpr->longval ? TRUE = 1 : TRUE = 0;
			break;
		case 301:
			ifExpr->dubval ? TRUE = 1 : TRUE = 0;
			break;
		case 303:
			ifExp->longval ? TRUE = 1 : TRUE = 0;
			break;
	}
}*/
