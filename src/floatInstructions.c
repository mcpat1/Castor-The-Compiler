//File: floatInstructions.c
//Author: Patrick McElvaney
//Born on Date: 2/20/15
//Date Modified: 2/20/15
//Description: 
//
//

//fprintf( textSec, "");

Type *faddition(Type *lhs, Type *rhs){
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->dubval = lhs->dubval + rhs->dubval;

		/* fortunately the assembly world does not care */
		fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
		//push value into st(0)
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		fprintf( textSec, "\tadd\trsp, 8\n");
		fprintf( textSec, "\t;add the previous st0 w/ top and discard top\n");
		fprintf( textSec, "\tfaddp\tst1, st0\n");
		//pop FPU stack and store in rsp
		fprintf( textSec, "\tfstp\t QWORD [rsp]\n");
		
		return tmp;
	}
	lhs->dubval += rhs->dubval;
	//rhs is no longer needed, free the memory.
	/*free(rhs);*/
	//rhs = NULL;
	/* asm write */
	//push value into st(0) but will be in st(1) soon...
	fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
	//push value into st(0)
	fprintf( textSec, "\tfld\tQWORD [rsp]\n");
	fprintf( textSec, "\tadd\trsp, 8\n");
	fprintf( textSec, "\t;add the previous st0 w/ top and discard top\n");
	fprintf( textSec, "\tfaddp\tst1, st0\n");
	//pop FPU stack and store in rsp
	fprintf( textSec, "\tfstp\t QWORD [rsp]\n");
	return lhs;
}
/*  */
Type *fsubtract(Type *lhs, Type *rhs){
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->dubval = lhs->dubval - rhs->dubval;
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
		//push value into st(0)
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		fprintf( textSec, "\tadd\trsp, 8\n");
		fprintf( textSec, "\t;sub the previous st0 w/ top and discard top\n");
		fprintf( textSec, "\tfsubp\tst1, st0\n");
		//pop FPU stack and store in rsp
		fprintf( textSec, "\tfstp\tQWORD [rsp]\n");
		
		return tmp;
	}
	lhs->dubval -= rhs->dubval;
	//free malloc
	/*free(rhs);*/
	//rhs = NULL;
	fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
	//push value into st(0)
	fprintf( textSec, "\tfld\tQWORD [rsp]\n");
	fprintf( textSec, "\tadd\trsp, 8\n");
	fprintf( textSec, "\t;sub the previous st0 w/ top and discard top\n");
	fprintf( textSec, "\tfsubp\tst1, st0\n");
	//pop FPU stack and store in rsp
	fprintf( textSec, "\tfstp\tQWORD [rsp]\n");
  return lhs;
}
/*   */
Type *fmultiply(Type *lhs, Type *rhs) {
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->dubval = lhs->dubval * rhs->dubval;
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
		//push value into st(0)
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		fprintf( textSec, "\tadd\trsp, 8\n");
		fprintf( textSec, "\t;mul the previous st0 w/ top and discard top\n");
		fprintf( textSec, "\tfmulp\tst1, st0\n");
		//pop FPU stack and store in rsp
		fprintf( textSec, "\tfstp\tQWORD [rsp]\n");
		
		return tmp;
	}
	lhs->dubval *= rhs->dubval;
	/*free(rhs);*/
	//rhs = NULL;
	fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
	//push value into st(0)
	fprintf( textSec, "\tfld\tQWORD [rsp]\n");
	fprintf( textSec, "\tadd\trsp, 8\n");
	fprintf( textSec, "\t;mul the previous st0 w/ top and discard top\n");
	fprintf( textSec, "\tfmulp\tst1, st0\n");
	//pop FPU stack and store in rsp
	fprintf( textSec, "\tfstp\tQWORD [rsp]\n");
  return lhs;
}
/* performs div by zero error check*/
Type *fdivide(Type *lhs, Type *rhs){
	if (rhs->dubval == 0 && !FLAG_FUN )
		printError("Divide by Zero!\0", 1 );
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		if( !FLAG_FUN ){ 
			tmp->dubval = lhs->dubval / rhs->dubval;
		}
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
		//push value into st(0)
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		fprintf( textSec, "\tadd\trsp, 8\n");
		fprintf( textSec, "\t;div the previous st0 w/ top and discard top\n");
		fprintf( textSec, "\tfdivp\tst1, st0\n");
		//pop FPU stack and store in rsp
		fprintf( textSec, "\tfstp\tQWORD [rsp]\n");
		
		return tmp;
	}
	lhs->dubval /= rhs->dubval;
	//free rhs memory
	/*free(rhs);*/
	//rhs = NULL;
/* asm write */
	fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
	//push value into st(0)
	fprintf( textSec, "\tfld\tQWORD [rsp]\n");
	fprintf( textSec, "\tadd\trsp, 8\n");
	fprintf( textSec, "\t;div the previous st0 w/ top and discard top\n");
	fprintf( textSec, "\tfdivp\tst1, st0\n");
	//pop FPU stack and store in rsp
	fprintf( textSec, "\tfstp\tQWORD [rsp]\n");
	return lhs;
}
 /*   */
Type *fcomplement(Type *lhs){
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->dubval = -lhs->dubval;
		
		/* fortunately the assembly world does not care */
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		fprintf( textSec, "\tfchs\t\n");
		//pop FPU stack and store in rsp
		fprintf( textSec, "\tfstp\tQWORD [rsp]\n");
		
		return tmp;
	}
	lhs->dubval = -lhs->dubval;
	/* asm write */
	fprintf( textSec, "\tfld\tQWORD [rsp]\n");
	fprintf( textSec, "\tfchs\t\n");
	//pop FPU stack and store in rsp
	fprintf( textSec, "\tfstp\tQWORD [rsp]\n");
  return lhs;
}
 /* just calling the c pow function  */  
Type *fpower(Type *lhs, Type *rhs){
	if( lhs->attr != NULL){
		//we cannot assign the result to a known variable here!
		//so we need to return a temp value
		Type * tmp = makeTempStruct(lhs->type);
		tmp->dubval = pow(tmp->dubval, rhs->dubval);
			/* asm write */
		fprintf( textSec, "\t;float pow() c call\n");
		fprintf( textSec, "\tmovq\txmm0, QWORD [rsp + 8]\n\tmovq\txmm1, QWORD [rsp]\n\tadd\trsp, 8\n");
		// push number of xmm* registers into al then call pow
		fprintf( textSec, "\tmov\tal, 2\n\tcall\tpow\n");
		fprintf( textSec, "\tmovq\tQWORD [rsp], xmm0\n\t;emms  ;yay\n");
		
		return tmp;
	}
	lhs->dubval = pow(lhs->dubval, rhs->dubval);
	//rhs is no longer needed, free the memory.
	//free(rhs);
	//rhs = NULL;
	/* asm write */
	
  fprintf( textSec, "\t;float pow() c call\n");
  fprintf( textSec, "\tmovq\txmm0, QWORD [rsp + 8]\n\tmovq\txmm1, QWORD [rsp]\n\tadd\trsp, 8\n");
  // push number of xmm* registers into al then call pow
  fprintf( textSec, "\tmov\tal, 2\n\tcall\tpow\n");
  fprintf( textSec, "\tmovq\tQWORD [rsp], xmm0\n\t;emms  ;yay\n");
  return lhs;
}

	