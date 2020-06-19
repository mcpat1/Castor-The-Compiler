%{
#include <stdio.h>  
#include <iostream>
#include <cmath>
#include <string.h>
#include <stdlib.h>

//Global Variables
int error_occured = 0; //for error recovery
int nestedWhiles; //indexer for whileJumpArray
int whileJumpArray[24]; //can manage a dozen nested while loops
int WHILE_FLAG = 0; //for break and while validation
int num_args_to_match = 0; //attribute of global_arg_list
int fun_body_offsets = 1; //for local function variables
int RET_VAL = 0;
int FLAG_FUN = 0; //for locating variables as stack offsets instead of labels inside functions
int FUN_SET = 0; //for function branch of decl in global_scope
int OR_SET; //for solving follow
int SAVE_ID = 0; //for while and function call closure enabling declaration of same name but probably mostly for while and if conditionals because of the function format of sym_tbl this can be avoided
char global_file_output[256]; //MAX_FILE_NAME_LENGTH
const char* textPortion = "src/tests/textSection.asm";
unsigned long num_lines_of_code = 0;
//////////////////////////
extern int scopes_in_use;//used for declaring unique label for every scope
static long stackForScopes[1024]; // LIFO scope num tracker for .data writes upon '}'
static long currentScopeIndex = 0;
static unsigned long condLabelIndex = 0; //cur pos in condLabels[]
static unsigned long condLabels[1024]; //for backtracking out of a cond build
static unsigned long total_cond_labels = 0; //...
FILE *globalTextSec; //for global vars to be placed after main
FILE *curDataFile; //used by fprintf for writing .data section
char currentScopeStr[30] = {0}; //fprintf writing .text sec push and pop label.<id>
						//NOTE: NULL fixes errors when using asprintf!!!
int I_MODE = 0; //flag for fprint to terminal or not
FILE* asmfile;	// file object pointer to .data and boilerplate
FILE* textSec; // file pointer to .text section and closing boilerplate

//////////////////////////

//preprocessor global vars override(#define)
//#define YYSTYPE double		//override yystype of int
/* Parser error reporting routine */
void yyerror(const char *msg, int type = 0);
extern FILE * yyin; /* enable reading from a file */
extern int yylineno;
extern char *yytext;
extern char linebuf[496];
extern unsigned long num_lines;
extern unsigned long num_chars;
extern char* lex_var_str;

//////////////////////////

/* user files */
//CPP files

//C files
#include "uthash.h" //hash implementation Author: Troy Hanson
#include "recordTypesAndAttributes.c" //contains all symbol table structs
struct uthash_function_struct *current_fun = NULL; //used for propagating functions parameters, etc...
#include "myExceptions.c"
#include "uthashFunctions.c" //interface funtions into uthash.h
#include "middleEnd.c" //contains boilerPlate so, above all .c
#include "stringInstructions.c" //NEW!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/////////////////////////////////////////////////
#include "floatInstructions.c" //floating point assembly
#include "intInstructions.c" //integer assembly
#include "condInstructions.c" //conditional evaluation code
#include "declarationsAndAssignments.c" //variable decl & assign found here
#include "funInstructions.c"

/* Scannar routine defined by Flex */
int yylex();

using namespace std;
//prints y.output for debugging
#define YYDEBUG 1
%}

/* yylval union type */
%union {
    long longval;
	double dubval;
	char * strval;
	//void * voidval;
	struct Types *type;
}

/* Miscellaneous token types */
%token <longval> INTEGER "int"
%token <dubval> FLOAT
%token <strval> STRING
%token <strval> KEYWORD
%token <strval> ID
//%token <tptr> VAR FNCT /* for variables and functions */


/* Operators */
%right '='
%left '+' '-'
%left '*' '/' '%'
%left OR
%left AND
%right NOT
%left '>' '<' GE LE EQ NEQ
%right UMINUS
%right POW
%right _int_
%right _double_
%right _void_ _return_
%right _print_ _printl_ _scan_

%token _IF_ _ELSE_ _WHILE_ _main_ _break_ _continue_


/* Nonterminal types */
%type <type> expr
%type <type> logic_expr
/* proj 5 additions */
%type <type> global_val
%type <type> decl
%%

start: { O_boilerPlate(); }
program { C_boilerPlate(); 
			  //print statistics
  printf( "# of lines = %lu, # of chars = %lu, lines of code %lu\n", num_lines + 1, num_chars, num_lines_of_code ); 
  YYACCEPT;} //YYACCEPT ADDED, not tested
;									//but is logical
program: 
global_set main_set function_definitions
;

global_set:
/* empty string */
| global_set global
;
global:
global_job
| decl '=' global_val ';' { /*write to global sym_tbl*/varAssignment($1, $3); popStack(); }
| decl ';' {  popGlobalStack();/* bss_decl ';' write to .bss section in future */ }
;

global_val:
INTEGER { $$ = intNUM($1); }
| FLOAT { $$ = floatNUM($1); }
;

param_list:
/* empty string */
| decl   ',' param_list { writeOffset($1); }
| decl {  writeOffset($1); }
;
/*
bss_decl:
_int_ ID {/*bssID($2, 1);* /}
| _double_ ID {/*bssID($2, 2);* /}
;*/

decl:
_int_ ID {  FUN_SET ? $$ = insertParameter(lex_var_str, 300)/*;*/ : $$ = initializeInt(lex_var_str); }
| _double_ ID { FUN_SET ? $$ = insertParameter(lex_var_str, 301)/*;*/ : $$ = initializeFloat(lex_var_str); }
;

global_fun:
fun_id_and_retval '(' param_list ')' ';' { /* goto label of function that is declared after main*/ }
| fun_id_and_retval '(' param_list ')' {  --scopes_in_use; FUN_SET = 0; FLAG_FUN = 1; }'{' stmt_list '}' { endFun(); endOfScope(); }                                                             /* Think that a nested proc/fun is isolated from outer scope. for security sets and undefined internet accesses! This is why we go to '{' global '}' instead of using '{' stmt_list '}'   IF TOO COMPLICATED THEN JUST DO stmt_list */
;
/*
global_proc:
ID '(' param_list ')' ':' stmt_list ':'';' { / * a procedure is a macro, think inline functions in c * / }*/

global_job: 
global_fun
{/*| global_proc {/* no return type, thus macro * / }*/}
;

fun_id_and_retval:
_void_ ID {  FUN_SET = 1; insertFunction(lex_var_str, 0); }
| _int_ ID { FUN_SET = 1; insertFunction(lex_var_str, 300); }
| _double_ ID { FUN_SET = 1; insertFunction(lex_var_str, 301); }
;

main_set:
_main_ { fprintf( textSec, "\nmain:\n\n\tpush\trbp\n\tmov\trbp, rsp\n"); insertGlobalDeclarations(); }stmt_list
/* | fun '{' arg_list '}' { FUNCTIONS YAY! }*/
;

function_definitions: /* empty string until develeopment */ ;


stmt_list:
stmt_list stmt { ++num_lines_of_code; }
| stmt { ++num_lines_of_code; }
| error { yyerror("Castor thinks a semicolon may be missing", 1); yyerrok; } //untested error check
;

arg_list:
/* empty string */
| arg_list ',' expr { appendArg($3);/*this auto pushes to stack with expr code*//*I am storing the whole Type but do i just need the type number?!?!*/}
| expr { appendArg($1); }
;

stmt: 
if_stmt
| while_stmt
| expr '=' expr ';' { varAssignment($1, $3);   if(RET_VAL){--RET_VAL;}else{popStack();} }
| expr ';' { if( !FLAG_FUN ){popStack();} }
| _print_ expr ';'  { print_call($2); }
| _printl_ expr ';' { printl_call($2); }
| _scan_ ID ';'     { scan_call($2); /*free(lex_var_str);*/}
| stmt_block
| jump_stmt ';' { /*continue ';', break ';', _return_ ';', _return_ expr ';'*/ }
;
/*
fun:
ID { $$ = }
;
*/
stmt_block:
 '{' { newScope(); } stmt_list '}' { endOfScope(); }
;

jump_stmt:
_return_  { retValCheck( current_fun->s_head->param_type[0], 0 ); fprintf( textSec, "\n\tmov\trsp, rbp\n\tpop\trbp\n\tret\n");/* for void return --proj5 */ }
| _return_ expr { retValCheck( current_fun->s_head->param_type[0], $2->type);/*(1) ret expr needs to be checked against function ret type*/ RET_VAL = 1; fprintf( textSec, "\n\n\tmov\trsp, rbp\n\tpop\trbp\n\tret\n");/*(2) ret needs to be placed on stack at appropriate location --proj6*/}
| _break_     { if( !nestedWhiles ){ printError( "break", 9 ); } fprintf( textSec, ";break\n\tjmp\twhileExit%u\n", whileJumpArray[ nestedWhiles ] );   }
| _continue_  { if( !nestedWhiles ){ printError( "continue", 9 ); } fprintf( textSec, ";break\n\tjmp\twhileCond%u\n", whileJumpArray[ nestedWhiles ] );  }
;

/*selection_stmt:*/
if_stmt: 
matched_if_stmt 
| open_if_stmt 
;

if_part: 
_IF_ '(' expr ')' { fprintf( textSec, "\ttest\tQWORD [rsp], 1\n\tje\tifCond%lu\n", total_cond_labels); condLabels[++condLabelIndex] = total_cond_labels++; /* cmp $3, 0; jz ifCond%d , total_cond_labels ; condLabels[condLabelIndex++] = total_cond_labels++; */ }
;

if_part_then_matched_stmt:
if_part matched_if_stmt { fprintf( textSec, "\tjmp ifCond%lu\n", total_cond_labels); condLabels[++condLabelIndex] = total_cond_labels++; /* print skipElsefalse jump label */ }
;

matched_if_stmt:
if_part_then_matched_stmt _ELSE_ { fprintf( textSec, "ifCond%lu:\n", condLabels[condLabelIndex - 1]); }	matched_if_stmt	{ fprintf( textSec, "ifCond%lu:\n", condLabels[condLabelIndex]); condLabelIndex -= 2; }
| stmt_block { /* shortjump skipElseLabel;(THINK: if cannot fall through to else)*/ /* print false goto label*/ }
;

open_if_stmt:
if_part if_stmt { fprintf( textSec, "ifCond%lu:\n", condLabels[condLabelIndex--] ); }
| if_part_then_matched_stmt _ELSE_ { fprintf( textSec, "ifCond%lu:\n",  total_cond_labels); condLabels[condLabelIndex++] = total_cond_labels++; }	open_if_stmt 
;

while_stmt: { fprintf( textSec, "whileCond%lu:\n",  total_cond_labels); whileJumpArray[++nestedWhiles] = total_cond_labels; condLabels[++condLabelIndex] = total_cond_labels++; ++scopes_in_use; newScope(); SAVE_ID = 1; } _WHILE_ '(' expr ')' { fprintf( textSec, "\ttest\tQWORD [rsp], 1\n\tje\twhileExit%lu\n", condLabels[condLabelIndex]); --scopes_in_use; SAVE_ID = 0; } '{' stmt_list '}' { fprintf( textSec, "\tjmp\twhileCond%lu\nwhileExit%lu:\n", condLabels[condLabelIndex], condLabels[condLabelIndex] ); --condLabelIndex; endOfScope(); /* loop back to label */ --nestedWhiles; }
;
/*
logic_list:
/* empty string * /
| logic_expr logic_list { $$ = 
;
*/

logic_expr:
 expr AND { $$ = logicalANDfirst($1); } /*expr { $$ = logicalFollow($1, $4); }*/
| expr OR {  $$ = logicalORfirst($1); } /*expr { $$ = logicalFollow($1, $4); }*/
;
/* IMPORTANT: remember rule stated on line below */
/* always check expr->attr against NULL before freeing malloc!! */
expr:
_int_ ID				{ $$ = initializeInt(lex_var_str); /*free(lex_var_str);*/  /* simple adding identifier to symbol table*/ }
| _double_ ID			{ $$ = initializeFloat(lex_var_str); /*free(lex_var_str);*/ }
| logic_expr expr { $$ = logicalFollow($1, $2); }
| expr EQ expr		{ $$ = isEqual($1, $3); }
| expr NEQ expr		{ $$ = isNotEqual($1, $3); }
| expr LE expr		{ $$ = isLessThanOrEq($1, $3); }
| expr GE expr		{ $$ = isGreaterThanOrEq($1, $3); }
| expr '<' expr		{ $$ = isLessThan($1, $3); }
| expr '>' expr		{ $$ = isGreaterThan($1, $3); }
| expr '+' expr   { $$ = addition($1, $3); }
| expr '-' expr   { $$ = subtract($1, $3); }
| expr '*' expr   { $$ = multiply($1, $3); }
| expr '/' expr   { $$ = divide($1, $3); }
| expr '%' expr   { $$ = mod($1, $3); }
| '!' expr			{ $$ = logicalNOT($2); } 
| '-' expr        { $$ = complement($2); }
| expr POW expr   { $$ = power($1, $3); }
| INTEGER           { $$ = intNUM($1); }
| FLOAT			  { $$ = floatNUM($1); }
| STRING		  { $$ = strLiteral(lex_var_str); lex_var_str = NULL; }
| ID '(' arg_list ')' { $$ = getSignature($1); /*function call for proj5*/  num_args_to_match = 0; /*reset*/  }
| ID			  { $$ = getID($1); };
| '(' expr ')'    { $$ = $2; }

%%



void yyerror(const char *msg, int eType)
{ /* Future Application: perror() replaces fprintf( stderr, ...); */
	switch(eType){
		case 0:
			fprintf( stderr, "Lex Error: Traceback (Most recent call last):\n");
			fprintf( stderr, "%ld: %s at %s in this line:\n%s\n",
              num_lines, msg, yytext, linebuf);
			eType = 65535; //ALL OF THE ABOVE TESTED AND WORKS!!!
			break;
		case 1:
			fprintf( stderr, "Error(1): Traceback (Most recent call last):\n");
			fprintf( stderr, "%ld: %s at %s in this line:\n%s\n",
              num_lines, msg, yytext, linebuf);
			eType = 65535; //ALL OF THE ABOVE TESTED AND WORKS!!!
			break;
		case 2:
			fprintf( stderr, "Command Line Argument Error: %s\n", msg);
			eType = 65535; //ALL OF THE ABOVE TESTED AND WORKS!!!
			break;
		case 3:
			fprintf( stderr, "Error in reading from file : %s\n", msg);
			eType = 65535; //ALL OF THE ABOVE TESTED AND WORKS!!!
			break;
		case 65535:
			remove(global_file_output);
			remove(textPortion);
			exit(0); //error handled
		default:
			cerr << "Parser error:\n" << msg << endl;
	}
    exit(1);
	}


int main(int argc, char* argv[])
{
	
	if ( argc > 1 ){
		/*menuselection*/
		if( strlen(argv[1]) > 251)
			yyerror ("file name length exceeds max size\n", 2);
		//assign scanner to a file
		sprintf(global_file_output, "%s.asm", argv[1]);
		yyin = fopen( argv[1], "r" );
		/* check for successful file open */
		if( ferror(yyin) )
			yyerror( argv[1], 3);

	}
	else{
		strcpy(global_file_output, "a.asm");
		yyin = stdin;
		I_MODE = 1;
		/*changing ';' too '\n' ,  ';' by default */
		//s_apnd_sym = '\n';
	}
	
/* initialize currentScopeStr to scope0 i.e. global_scope */
	if ( (sprintf( currentScopeStr, "scope%d", scopes_in_use)) == -1){
		
		printError( "Malloc of sprintf failed", 3);
	}
	struct uthash_scope_struct *newScope;
	newScope = (struct uthash_scope_struct*)malloc(sizeof(uthash_scope_struct));
	/* Set parent location: scope_t ptr */
	newScope->parentScope = NULL; //end of the line, global_scope
	currentScope = newScope; 
	globalScope = currentScope; //global scope pointer for ret from function build
	//might need stpcpy ( , ) ...see scanner.l !!!cld be NULL @free(curScopeStr)
	strcpy( newScope->scope, currentScopeStr );

	/* Set FILE *fp *///set hashed scope fp and update curDataFile global var
	curDataFile = newScope->fp = fopen( currentScopeStr , "w+" );
	newScope->mapped_scope = NULL; //hash symtbl for this scope's ids 
	HASH_ADD_KEYPTR(sh, scope_symtbl, newScope->scope, strlen(newScope->scope), newScope ); 

	//THIS LIFO MAY REDUCE SIZE OF MALLOC in main OR for tracking FUNCTION CALLS
	stackForScopes[ currentScopeIndex ] = scopes_in_use;

	
/*start scanning*/
    return yyparse();
}


