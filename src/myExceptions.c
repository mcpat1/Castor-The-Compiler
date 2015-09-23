//////////////////////////////////////////////////
//File: middleEnd.c
//Author: Patrick McElvaney
//Born on Date: 3/20/15
//Date Modified: -/--/--
//Description: 

#include <stdio.h>
#include <stdlib.h>
/*cleans up partial .asm file on syntax error, so far only div by zero*/
void printError( const char * msg, int eType ){
	switch(eType){
	case 1:
		fprintf( stderr, "Error on line [%d]: %s\n", num_lines, msg);
		error_occured = 1;
		return; //error recovery
		//break;
	case 2:
		fprintf( stderr, "Stack Overflow: %s\n", msg);
		break;
	case 3:
		fprintf( stderr, "Out of Memory: %s\n", msg);
		break;
	case 4:
		fprintf( stderr, "Semantic Error: %s\n", msg);
		break;
	case 5:
		fprintf( stderr, "File I/O Error: %s could not open for write\n", msg);
		break;
	case 6:
		fprintf( stderr, "Function %s called without proper definition\n", msg); 
		break;
	case 7:
		fprintf( stderr, "Error: Type being returned from function %s does not match signature \n\tdefinition.  Expecting return type: ", current_fun->fun_name);
		if( current_fun->s_head->param_type[0] == 0 ){
			fprintf( stderr, "void.\n");
		} else if( current_fun->s_head->param_type[0] == 300 ){
			fprintf( stderr, "int.\n");
		} else {
			fprintf( stderr, "double.\n");
		}
		fprintf( stderr, "\tReturning type: %s on line %d.\n", msg, num_lines);
		error_occured = 1;
		return; //error recovery
		//break;
	case 8:
		fprintf( stderr, "Error: No value being returned from function %s when signature has\n\ta return of type %s type before line %d.\n", current_fun->fun_name, msg, num_lines); 
		//goto main_recover;
		error_occured = 1;
		return; //error recovery
		//break;
	case 9:
		fprintf( stderr, "Jump Error: %s instruction outside of a while loop is not permitted.\nSee line %d\n", msg, num_lines );
		break;
	default:
		fprintf( stdout, "\n\nI NEED BETTER ERROR CHECKING\n\n");
	}
  remove(global_file_output);
  //COMMENT OUT LINE BELOW TO KEEP textSection.asm 
  //WHEN DEBUGGING ERRORS!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  remove(textPortion); //clean up occured, error handled return 0
  exit(0);
  /* future improvement */
  /*
  fclose( asmfile ); // close file handle for global_file_output
	rename( global_file_output, errorDataPortion );
	fclose( textSec ); // close file handle for textPortion before remove()
	rename( textPortion, errorTextPortion );
	*/
}