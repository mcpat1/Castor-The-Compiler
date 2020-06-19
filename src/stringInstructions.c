//File: stringInstructions.c
//Author: Patrick McElvaney
//Born on Date: 3/11/15
//Date Modified: 3/11/15
//Description: 
/*
	handle string literals. 
	
	FUTURE:
		this file will contain char[] (((byte size characters that are pushed on stack))) and char* (((pointer to a memory location pushed onto stack)))
*/




/*
check for \n \t \a (look up the list of \. characters.
use a switch statement for each symbol and continue. 
		thus need a while loop to iterate over the string.
		*/
void buildStr(char *str){

	unsigned long index = 1;
	
	while( str[index] != '"' ){ 
		
		if( str[index] == '\\' ){
			++index;
			switch(str[index]){
				case 'n':
					fprintf( curDataFile, "',10,'");
					break;
				case 't':
					fprintf( curDataFile, "',9,'");
					break;
				case 'a':
					fprintf( curDataFile, "',7,'");
					break;
				
				case '"':
					fprintf( curDataFile, "',34,'");
					break;
				default:
					printf( "DID NOT CATCH THIS \\%c\n", str[index]);
	
			}
		}
		else {
			switch (str[index]){
				case 39: /* ' */
					fprintf( curDataFile, "',39,'");
					break;
				case ',':
					fprintf( curDataFile, "',44,'");
					break;
				case '.':
					fprintf( curDataFile, "',46,'");
					break;
				default:
					fprintf( curDataFile, "%c", str[index] );
			}
		}
		++index;
	}
	fprintf( curDataFile, "',0\n");
}


/* Pushes char * (pointer to a string) on the stack  */
Type* strLiteral(char *string){
	/* initialize record */
	Type *structPtr;
	if((structPtr = (Type *) malloc(sizeof(Type *))) == NULL)
		printError( "Malloc of string type struct failed", 3);
	structPtr->longval = (long)NULL;
	structPtr->dubval = (double)NULL;
	structPtr->strval = string;
	structPtr->type = 302; //enum val for STRING
	structPtr->attr = NULL; //pointer to Attributes(additional info)
	/* asm write */
	fprintf( curDataFile, ".sLit%d:\tdb  '", strings_in_use);
	
	/* parse the string for escape sequences that must be handled for NASM */
	buildStr(string); 
	
	fprintf( textSec, "\tpush\tQWORD %s.sLit%d\n", currentScopeStr, strings_in_use++);
	return structPtr;
}
