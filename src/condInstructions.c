//////////////////////////////////////////////////
//File: condInstructions.c
//Author: Patrick McElvaney
//Born on Date: 3/21/15
//Date Modified: 3/26/15
//Description: This file contains function calls for ALL conditional
// evaluations between expressions 
// 
// 


Type *logicalNOT(Type *obj){
	if( obj->type == 303 ){
		fprintf( textSec, "\tmov\trdi, [rsp]\n\tmov\tQWORD [rsp], 0\n\tcmp\trdi, 0\n\tsete\t[rsp]\n");
		obj->longval = !obj->longval;
		return obj;
	}
	Type *tmp = makeTempStruct(303);
	switch( obj->type ){
		case 300:
			//always set to 0 and if zero, set to 1
			fprintf( textSec, "\tmov\trdi, [rsp]\n\tmov\tQWORD [rsp], 0\n\tcmp\trdi, 0\n\tsete\t[rsp]\n");
			tmp->longval = !obj->longval;
			return tmp;
		case 301:
			//always set to 0 and if zero, set to 1
			fprintf( textSec, "\tfld\tQWORD [rsp]\n\tfld\tQWORD [global_scope.zero]\n");
			fprintf( textSec, "\tfcomip\n\tfstp\tst0\n\tmov\tQWORD [rsp], 0\tsete\t[rsp]\n" );
			tmp->longval = !obj->dubval;
			return tmp;
	}
			/*
		case 302:
//DOES C DO STRING BOOLEANS!? i guess if it is == NULL, so yeah
			return tmp->longval = !obj->strval;
			*/

}///!!!///!!!///!!!///!!!///!!!///!!!///!!!/\/\?!?!/\/\?!?!///!!!///!!!

/* logicalAND */
Type *logicalANDfirst( Type *obj){
	OR_SET = 0;
	fprintf( textSec, "\t;left boolean of AND\n");
	if( obj->type == 303 ){
		return obj;
	}
	//handle float as first expression
	if ( obj->type == 301 ){
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		//push value into st(0)
		fprintf( textSec, "\tfld\tQWORD [global_scope.zero] \n");
		fprintf( textSec, "\tsub\trsp, 8\n\tfcomip\n\tfstp\tst0\n\tsetne\t[rsp]\n" );
	}
	fprintf( textSec, "\ttest\tQWORD [rsp], 0\n\tjne\tifCond%lu\n", 
											total_cond_labels);
	if( obj->attr != NULL ){
		Type *tmp = makeTempStruct(303); //longval type is boolean type
		switch( obj->type ){
			case 300:
				if( obj->longval == 0 ){
					tmp->longval = 0;
					break;
				}
				tmp->longval = 1;
				break;
			case 301:
				if( obj->dubval == 0 ){
					tmp->longval = 0;
					break;
				}
				tmp->longval = 1;
				break;
			default:
				printError( "Type mismatch during Boolean evaluation", 4); 
		}
		return tmp;
	}
	switch( obj->type ){
		case 300:
			if( obj->longval == 0 ){
					obj->longval = 0;
					break;
				}
				obj->longval = 1;
				break;
			case 301:
				
				if( obj->dubval == 0 ){
					obj->longval = 0;
					break;
				}
				obj->longval = 1;
				break;
			default:
				printError( "Type mismatch during Boolean evaluation", 4); 
		}
		//set type to boolean-like (which is longval)
		obj->type = 300;
	return obj;
}

Type *logicalFollow( Type *lhs, Type *rhs){
	if ( OR_SET ){
		/* NOTE:  lhs is boolean from logicalORfirst, */
		//handle float as first expression
		fprintf( textSec, "\t;right boolean of OR\n");
		if ( rhs->type == 301 ){
			fprintf( textSec, "\tfld\tQWORD [rsp]\n");
			//push value into st(0)
			fprintf( textSec, "\tfld\tQWORD [global_scope.zero] \n");
			fprintf( textSec, "\tsub\trsp, 8\n\tfcomip\n\tfstp\tst0\n\tsetne\t[rsp]\n" );
			fprintf( textSec, "\tpop\trdi\n\tor\t[rsp], rdi\n\tje\tifCond%lu\n", 
												total_cond_labels + 1);
			if( lhs->longval == 0 && rhs->dubval == 1 ){
				lhs->longval = 1;
			}
			//NOTE: o.w. lhs does not change (See below commented out section for prf)
			return lhs;
		}
		fprintf( textSec, "\tpop\trdi\n\tor\t[rsp], rdi\n\tje\tifCond%lu\n", 
												total_cond_labels);
			/* short-circuit */
		if( lhs->longval == 0 && rhs->longval != 0 ){
			lhs->longval = 1;
		}	
		return lhs;
		/*
		if( lhs->longval == 1 ){
			return lhs;
		}
		/* if we got here, we know left is false * /
		if( rhs->longval == 0 ){
			return lhs;
		}*/	
		
	}
/*else AND */
	/* NOTE:  lhs is boolean from logicalORfirst, */
	//handle float as first expression
	fprintf( textSec, "\t;right boolean of AND\n");
	if ( rhs->type == 301 ){
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		//push value into st(0)
		fprintf( textSec, "\tfld\tQWORD [global_scope.zero] \n");
		fprintf( textSec, "\tsub\trsp, 8\n\tfcomip\n\tfstp\tst0\n\tsetne\t[rsp]\n" );
		fprintf( textSec, "\tpop\trdi\n\tand\t[rsp], rdi\n\tjne\tifCond%lu\n", 
											total_cond_labels);
		if( lhs->longval == 1 && rhs->dubval == 1 ){
			lhs->longval = 1;
		}
		else{
			lhs->longval = 0;
		}
		//NOTE: o.w. lhs does not change (See below commented out section for prf)
		return lhs;
	}
	fprintf( textSec, "\tpop\trdi\n\tand\t[rsp], rdi\n\tje\tifCond%lu\n", 
											total_cond_labels);
		/* short-circuit */
		if( lhs->longval == 1 && rhs->longval == 1 ){
			lhs->longval = 1;
		}
		else{
			lhs->longval = 0;
		}
	return lhs;
	/*
	if( lhs->longval == 1 ){
		return lhs;
	}
	/* if we got here, we know left is false * /
	if( rhs->longval == 0 ){
		return lhs;
	}*/
}

/* logicalOR */
Type *logicalORfirst( Type *obj/*, unsigned long cond_label_num*/){
	OR_SET = 1;
	//handle float as first expression
	fprintf( textSec, "\t;left boolean of OR\n");
	if ( obj->type == 301 ){
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		//push value into st(0)
		fprintf( textSec, "\tfld\tQWORD [global_scope.zero] \n");
		fprintf( textSec, "\tfcomip\n\tfstp\tst0\n\tsub\trsp, 8\n\tsetne\t[rsp]\n" );
	}
	fprintf( textSec, "\ttest\tQWORD [rsp], 1\n\tjne\tifCond%lu\n", 
											total_cond_labels);
	if( obj->attr != NULL ){
		Type *tmp = makeTempStruct(300); //longval type is boolean type
		switch( obj->type ){
			case 300:
				if( obj->longval == 0 ){
					tmp->longval = 0;
					break;
				}
				tmp->longval = 1;
				break;
			case 301:
				if( obj->dubval == 0 ){
					tmp->longval = 0;
					break;
				}
				tmp->longval = 1;
				break;
			default:
				printError( "Type mismatch during Boolean evaluation", 4); 
		}
		return tmp;
	}
	switch( obj->type ){
		case 300:
			if( obj->longval == 0 ){
					obj->longval = 0;
					break;
				}
				obj->longval = 1;
				break;
			case 301:
				
				if( obj->dubval == 0 ){
					obj->longval = 0;
					break;
				}
				obj->longval = 1;
				break;
			default:
				printError( "Type mismatch during Boolean evaluation", 4); 
		}
		//set type to boolean-like (which is longval)
		obj->type = 300;
	return obj;

}
Type *logicalORfollow( Type *lhs, Type *rhs){
	/* NOTE:  lhs is boolean from logicalORfirst, */
	//handle float as first expression
	fprintf( textSec, "\t;right boolean of OR\n");
	if ( rhs->type == 301 ){
		fprintf( textSec, "\tfld\tQWORD [rsp]\n");
		//push value into st(0)
		fprintf( textSec, "\tfld\tQWORD [global_scope.zero] \n");
		fprintf( textSec, "\tfcomip\n\tfstp\tst0\n\tsub\trsp, 8\n\tsetne\t[rsp]\n" );
		fprintf( textSec, "\tpop\trdi\n\tor\t[rsp], rdi\n\tjne\tifCond%lu\n", 
											total_cond_labels + 1);
		if( lhs->longval == 0 && rhs->dubval == 1 ){
			lhs->longval = 1;
		}
		//NOTE: o.w. lhs does not change (See below commented out section for prf)
		return lhs;
	}
	fprintf( textSec, "\tpop\trdi\n\tor\t[rsp], rdi\n\tjne\tifCond%lu\n", 
											total_cond_labels);
		/* short-circuit */
	if( lhs->longval == 0 && rhs->longval != 0 ){
		lhs->longval = 1;
	}
	return lhs;
	/*
	if( lhs->longval == 1 ){
		return lhs;
	}
	/* if we got here, we know left is false * /
	if( rhs->longval == 0 ){
		return lhs;
	}*/


}
Type *isLessThan(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	Type *tmp = makeTempStruct(lhs->type);
	switch( lhs->type ){
		case 300:
			fprintf( textSec, "\t;test %d < %d\n", lhs->longval, rhs->longval );
			fprintf( textSec, "\tpop\trsi\n\tpop\trdi\n\tsub\trsp, 8\n");
			fprintf( textSec, "\tcmp\trdi, rsi\n\tsetl\t[rsp]\n");
			//fprintf( textSec, "\ttest\t[rsp], 1\n\tjne\t");
			if ( (lhs->longval - rhs->longval) < 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		case 301:
			fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
			//push value into st(0)
			fprintf( textSec, "\tfld\tQWORD [rsp]\n");
			fprintf( textSec, "\tfcomip\n\tfstp\tst0\n\tsub\trsp, 8\n\tseta\t[rsp]\n" );
			if ( (lhs->dubval - rhs->dubval) < 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		/*future implementation to include strings*/
		/*case 302:
			if ( (lhs->strval - rhs->strval) < 0 ){
				return tmp->longval = 1;
			}
			return tmp->longval = 0;
			*/
	}
}

Type *isGreaterThan(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	Type *tmp = makeTempStruct(lhs->type);
	switch( lhs->type ){
		case 300:
			fprintf( textSec, "\t;test %d > %d\n", lhs->longval, rhs->longval );
			fprintf( textSec, "\tpop\trsi\n\tpop\trdi\n\tsub\trsp, 8\n");
			fprintf( textSec, "\tcmp\trdi, rsi\n\tsetg\t[rsp]\n");
			//fprintf( textSec, "\ttest\t[rsp], 1\n\tjne\t");
			if ( (lhs->longval - rhs->longval) > 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		case 301:
			fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
			//push value into st(0)
			fprintf( textSec, "\tfld\tQWORD [rsp]\n");
			fprintf( textSec, "\tfcomip\n\tfstp\tst0\n\tsub\trsp, 8\n\tseta\t[rsp]\n" );
			if ( (lhs->dubval - rhs->dubval) > 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		/*future implementation to include strings*/
		/*case 302:
			if ( (lhs->strval - rhs->strval) < 0 ){
				return tmp->longval = 1;
			}
			return tmp->longval = 0;
			*/
	}
}


Type *isLessThanOrEq(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	Type *tmp = makeTempStruct(lhs->type);
	switch( lhs->type ){
		case 300:
			fprintf( textSec, "\t;test %d <= %d\n", lhs->longval, rhs->longval );
			fprintf( textSec, "\tpop\trsi\n\tpop\trdi\n\tsub\trsp, 8\n");
			fprintf( textSec, "\tcmp\trdi, rsi\n\tsetle\t[rsp]\n");
			//fprintf( textSec, "\ttest\t[rsp], 1\n\tjne\t");
			if ( (lhs->longval - rhs->longval) <= 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		case 301:
			fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
			//push value into st(0)
			fprintf( textSec, "\tfld\tQWORD [rsp]\n");
			fprintf( textSec, "\tsub\trsp, 8\n\tfcomip\n\tfstp\tst0\n\tsetbe\t[rsp]\n" );
			if ( (lhs->dubval - rhs->dubval) <= 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		/*future implementation to include strings*/
		/*
		case 302:
			if ( (lhs->strval - rhs->strval) <= 0 ){
				return tmp->longval = 1;
			}
			return tmp->longval = 0;
		*/
	}
}

Type *isGreaterThanOrEq(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	Type *tmp = makeTempStruct(lhs->type);
	switch( lhs->type ){
		case 300:
			fprintf( textSec, "\t;test %d >= %d\n", lhs->longval, rhs->longval );
			fprintf( textSec, "\tpop\trsi\n\tpop\trdi\n\tsub\trsp, 8\n");
			fprintf( textSec, "\tcmp\trdi, rsi\n\tsetge\t[rsp]\n");
			//fprintf( textSec, "\ttest\t[rsp], 1\n\tjne\t");
			if ( (lhs->longval - rhs->longval) >= 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		case 301:
			fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
			//push value into st(0)
			fprintf( textSec, "\tfld\tQWORD [rsp]\n");
			fprintf( textSec, "\tfcomip\n\tfstp\tst0\n\tsub\trsp, 8\n\tsetae\t[rsp]\n" );
			if ( (lhs->dubval - rhs->dubval) >= 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		/*future implementation to include strings*/
		/*
		case 302:
			if ( (lhs->strval - rhs->strval) >= 0 ){
				return tmp->longval = 1;
			}
			return tmp->longval = 0;
		*/
	}
}


/* Note: isNotEqual is just !(isEqual) */
/* Does this work for textfile? jeeze... */
Type *isEqual(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	Type *tmp = makeTempStruct(lhs->type);
	switch( lhs->type ){
		case 300:
			fprintf( textSec, "\t;test %d == %d\n", lhs->longval, rhs->longval );
			fprintf( textSec, "\tpop\trsi\n\tpop\trdi\n\tsub\trsp, 8\n");
			fprintf( textSec, "\tcmp\trdi, rsi\n\tsete\t[rsp]\n");
			//fprintf( textSec, "\ttest\t[rsp], 1\n\tjne\t");
			if ( (lhs->longval == rhs->longval) < 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		case 301:
			fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
			//push value into st(0)
			fprintf( textSec, "\tfld\tQWORD [rsp]\n");
			fprintf( textSec, "\tfcomip\n\tfstp\tst0\n\tsub\trsp, 8\n\tsete\t[rsp]\n" );
			if ( (lhs->dubval == rhs->dubval) < 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		/*future implementation to include strings*/
		/*
		case 302:
			if ( (lhs->strval == rhs->strval) < 0 ){
				return tmp->longval = 1;
			}
			return tmp->longval = 0;
		*/
	}
}

Type *isNotEqual(Type *lhs, Type *rhs){
	typeCheck(lhs, rhs);
	Type *tmp = makeTempStruct(lhs->type);
	switch( lhs->type ){
		case 300:
			fprintf( textSec, "\t;test %d != %d\n", lhs->longval, rhs->longval );
			fprintf( textSec, "\tpop\trsi\n\tpop\trdi\n\tsub\trsp, 8\n");
			fprintf( textSec, "\tcmp\trdi, rsi\n\tsetne\t[rsp]\n");
			//fprintf( textSec, "\ttest\t[rsp], 1\n\tjne\t");
			if ( (lhs->longval == rhs->longval) < 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		case 301:
			fprintf( textSec, "\tfld\tQWORD [rsp + 8]\n");
			//push value into st(0)
			fprintf( textSec, "\tfld\tQWORD [rsp]\n");
			fprintf( textSec, "\tfcomip\n\tfstp\tst0\n\tsub\trsp, 8\n\tsetne\t[rsp]\n" );
			if ( (lhs->dubval == rhs->dubval) < 0 ){
				tmp->longval = 1;
				return tmp;
			}
			tmp->longval = 0;
			return tmp;
		/*future implementation to include strings*/
		/*
		case 302:
			if ( (lhs->strval == rhs->strval) < 0 ){
				return tmp->longval = 1;
			}
			return tmp->longval = 0;
		*/
	}
}

