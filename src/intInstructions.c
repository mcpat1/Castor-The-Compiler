//////////////////////////////////////////////////
//File: intInstructions.c
//Author: Patrick McElvaney
//Born on Date: 2/11/15
//Date Modified: 2/27/15
//Description: This file contains function calls for ALL arithmetic 
// integer arithmetic code exists in this file while float math calls
// floatInstructions.c file i.e. is a dependency of the file.
//


/*   */
Type *addition(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	if( lhs->type == 301)
		return faddition(lhs, rhs);
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->longval = lhs->longval + rhs->longval;
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\tpop\trax\n\tadd\t[rsp], rax\n");
		 //check name: if NULL, not id, malloc, o.w. struct remains in symtbl
		/*if(rhs->attr == NULL){
			//rhs is no longer needed, free the memory.
			free(rhs);
			rhs = NULL;
		}*/
		return tmp;//<--should be able to trace to a free in 'declandassign.c'
	}
	lhs->longval += rhs->longval;
	/* asm write */
  fprintf( textSec, "\tpop\trax\n\tadd\t[rsp], rax\n");
  	//check name: if NULL, not id, malloc, o.w. struct remains in symtbl
	/*if(rhs->attr == NULL){
		//rhs is no longer needed, free the memory.
		free(rhs);
		rhs = NULL;
	}*/
  return lhs;
}

/*   */
Type *subtract(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	if( lhs->type == 301)
		return fsubtract(lhs, rhs);
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->longval = lhs->longval - rhs->longval;
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\tpop rax\n\tsub\t[rsp], rax\n");
		
		//check name: if NULL, not id, malloc, o.w. struct remains in symtbl
		/*if(rhs->attr == NULL){
			//free malloc
			free(rhs);
			rhs = NULL;
		}*/
		return tmp;//<--should be able to trace to a free in 'declandassign.c'
	}
	lhs->longval -= rhs->longval;
  fprintf( textSec, "\tpop rax\n\tsub\t[rsp], rax\n");
  
  	//check name: if NULL, not id, malloc, o.w. struct remains in symtbl
	/*if(rhs->attr == NULL){
		//free malloc
		free(rhs);
		rhs = NULL;
	}*/
  return lhs;
}

/*   */
Type *multiply(Type *lhs, Type *rhs) {
	typeCheck(lhs, rhs);
	if( lhs->type == 301)
		return fmultiply(lhs, rhs);
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->longval = lhs->longval * rhs->longval;
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\tpop rax\n\timul\trax, [rsp]\n");
		fprintf( textSec, "\tmov\t[rsp], rax\n");
		
		//check name: if NULL, not id, malloc, o.w. struct remains in symtbl
		/*if(rhs->attr == NULL){
			//free malloc
			free(rhs);
			rhs = NULL;
		}*/
		return tmp;//<--should be able to trace to a free in 'declandassign.c'
	}
	lhs->longval *= rhs->longval;
	/* asm text */
  fprintf( textSec, "\tpop rax\n\timul\trax, [rsp]\n");
  fprintf( textSec, "\tmov\t[rsp], rax\n");
  
  	//check name: if NULL, not id, malloc, o.w. struct remains in symtbl
	/*if(rhs->attr == NULL){
		//free malloc
		free(rhs);
		rhs = NULL;
	}*/
  return lhs;
}
/* performs div by zero error check*/
Type *divide(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	if( lhs->type == 301)
		return fdivide(lhs, rhs);
	if (rhs->longval == 0 && !FLAG_FUN ) /* must ignore if defining a function */
		printError("Divide by Zero!\0", 1 );
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		/* due to interactive nature of language, 
		--floating point exception-- thrown when defining functions */
		if( !FLAG_FUN ){ 
			tmp->longval = lhs->longval / rhs->longval;
		}
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\txor\trdx, rdx\n\tpop\trbx\n");
		fprintf( textSec, "\tpop rax\n\tcqo\n\tidiv\tQWORD rbx\n");
		fprintf( textSec, "\tpush\tQWORD rax\n");
		//check name: if NULL, not id, malloc, o.w. struct remains in symtbl
		/*if(rhs->attr == NULL){
			//free rhs memory
			free(rhs);
			rhs = NULL;
		}*/
		return tmp;//<--should be able to trace to a free in 'declandassign.c'
	}
	lhs->longval /= rhs->longval;

	/* asm write */
  fprintf( textSec, "\txor\trdx, rdx\n\tpop\trbx\n");
  fprintf( textSec, "\tpop rax\n\tcqo\n\tidiv\tQWORD rbx\n");
  fprintf( textSec, "\tpush\tQWORD rax\n");
	//check name: if NULL, not id, malloc, o.w. struct remains in symtbl
	/*if(rhs->attr == NULL){
		//free rhs memory
		free(rhs);
		rhs = NULL;
	}*/
  return lhs;
}

/* performs idiv and pushes remainder register (rdx)*/
Type *mod(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	if (rhs->longval == 0)
		printError("Mod by Zero!\0", 1 );
	if( lhs->type == 301){
		/* language does not support floating point modulus */
		printError( "Language does not support float modulus", 1);
	}
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->longval = lhs->longval % rhs->longval;
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\txor\trdx, rdx\n\tpop\trbx\n"); 
		fprintf( textSec, "\tpop\trax\n\tidiv\trbx\n\tpush\trdx\n");
		//check name: if NULL, not id, malloc, o.w. struct remains in symtbl
		/*if(rhs->attr == NULL){
			//free rhs memory
			free(rhs);
			rhs = NULL;
		}*/
		return tmp;//<--should be able to trace to a free in 'declandassign.c'
	}
	lhs->longval %= rhs->longval;

	/* asm write */
  fprintf( textSec, "\txor\trdx, rdx\n\tpop\trbx\n"); 
  fprintf( textSec, "\tpop\trax\n\tidiv\trbx\n\tpush\trdx\n");
  //check name: if NULL, not id, malloc, o.w. struct remains in symtbl
  	/*if(rhs->attr == NULL){
		//free rhs memory
		free(rhs);
		rhs = NULL;
	}*/
	return lhs;
}

/*  */ /////////////////////Try to negate zero!!!!!!!!!
Type *complement(Type *lhs){
	if( lhs->type == 301)
		return fcomplement(lhs);
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->longval = -lhs->longval;
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\tneg\tQWORD [rsp]\n");
		
		return tmp;//<--should be able to trace to a free in 'declandassign.c'
	}
	lhs->longval = -lhs->longval;
	/* asm write */
  fprintf( textSec, "\tneg\tQWORD [rsp]\n");
  return lhs;
}

/*
  pop exponent into rcx (loop counter), pop base into rax, cp rax to r8,
  loop til counter == 0 and push result (rax)
  MODIFIES lhs->dubval and rhs->dubval (note for future planning)
				^^^ would have to save value before calling fdivide()
*/
Type *power(Type *lhs, Type *rhs){
	Type *tmp;//points to lhs if lhs->attr == NULL, o.w. points to malloc type*
	Type *tempHolder; //UNUSED BUT HERE WHEN WE DECIDE IT'S NECESSARY
	int negFlag = 0; //set to 1 when lhs is negative
	typeCheck(lhs, rhs);
	if( lhs->type == 301)
		return fpower(lhs, rhs);
	/* USING special trick because the pow funct is messy */
	if( lhs->attr != NULL){
		
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		tmp = makeTempStruct(lhs->type);
		tmp->longval = lhs->longval;
	}
	else{
		tmp = lhs;
	}
	if( lhs->longval < 0 ){
		tmp->longval = -tmp->longval;
		negFlag = 1; //set negative flag
		fprintf( textSec, "\tneg\tQWORD [rsp + 8]\n");
	}
	if( rhs->longval < 0 ){
		/* Do something special because assembly code fails */
		// (1) complement the complement
		rhs->longval = -rhs->longval;
		//x^(-p) <==> (HERE) 1/(x^(p))  ==> fdivide( 1, (x^(p) )
		tmp->longval = pow(tmp->longval, rhs->longval);
		//restore rhs value
		rhs->longval = -rhs->longval;
		fprintf( textSec, "\tneg\tQWORD [rsp]\n");
		fprintf( textSec, "\tpop\trcx\n\tpop rdi\n\tcall\tloopPow\n");
		// may need to have storage in .data for these two below
		fprintf( textSec, "\tpush QWORD 1\n\tpush\trax\n");
		//x^(-p) <==> 1/(x^(p)) (HERE) ==> fdivide( 1, (x^(p) )
		rhs->dubval = 1.0;
		// lhs = (double) lhs->longval
		tmp->dubval = tmp->longval;
		//x^(-p) <==> 1/(x^(p)) ==> fdivide( 1, (x^(p) )  (HERE)
		//=====================================================
		rhs->dubval /= tmp->dubval;
		/* asm write */
		fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
		//push value into st(0)
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		fprintf( textSec, "\tadd\trsp, 8\n");
		fprintf( textSec, "\t;div the previous st0 w/ top and discard top\n");
		fprintf( textSec, "\tfdivp\tst1, st0\n");
		//pop FPU stack and store INTEGER in rsp
		fprintf( textSec, "\tfistp\tQWORD [rsp] ;converts float to int!!!\n");
		//=====================================================
		//trucate the result back to an integer
		tmp->longval = (int) rhs->dubval;
		//restore values of lhs->dubval and rhs->dubval
		/* lhs->dubval = savedlhsDUBVAL 
		   rhs->dubval = savedrhsDUBVAL */
		// if lhs is negative and exponent is odd...
		if( negFlag && rhs->longval % 2 ){
			tmp->longval = -tmp->longval;
			/* asm write */
			fprintf( textSec, "\tneg\tQWORD [rsp]\n");
		}
		/* pffffeww... */
		return tmp;
	}
	
	/* update value in symbol table */
	tmp->longval = pow(lhs->longval, rhs->longval);
	/* asm write */
	fprintf( textSec, "\tpop\trcx\n\tpop rdi\n\tcall\tloopPow\n");
	fprintf( textSec, "\tpush\trax\n");
	// if lhs is negative and exponent is odd...
	if( negFlag && rhs->longval % 2 ){
		tmp->longval = -tmp->longval;
		/* asm write */
		fprintf( textSec, "\tneg\tQWORD [rsp]\n");
	}
	//check name: if NULL, not id, malloc, o.w. struct remains in symtbl
  	//if(rhs->attr == NULL)
		//rhs is no longer needed, free the memory.
		/*free(rhs);*/
		//rhs = NULL;
	return tmp;
}
