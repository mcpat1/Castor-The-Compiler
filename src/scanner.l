%{
#include <cstdlib>
#include "parser.tab.hh"
//char linebuf[500]; defined on line 459 in scanner.cc
/* look into this ^^^^ */
char linebuf[496];
unsigned long num_lines = 1;
unsigned long num_chars = 0;
int scopes_in_use = 0;
char* lex_var_str;
%}
%x comment
%%
"/*"	BEGIN(comment);
 <comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             ++num_lines;
<comment>"*"+"/"        BEGIN(INITIAL);

<<EOF>> {
	/* end of file */
	return (int)(*yytext);
}

\n.*  { 
	++num_lines; ++num_chars;
	/* save the next line */ 
	strncpy(linebuf, yytext+1, sizeof(linebuf));
	//printf( "%s\n", linebuf );
	/* give back all but \n to rescan */ 
	yyless(1);
}

(([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?) {
		/* Floats */
	yylval.dubval = atof(yytext); /* attribute */
	num_chars += yyleng;  
	return FLOAT; /* name */
	}

[[:digit:]]+ {
    /* Integers */
    yylval.longval = atol(yytext);
	num_chars += yyleng; 
    return INTEGER;
}

"{"   { 
		/* entering new scope */
		++scopes_in_use;
		return (int)(*yytext);
}
"}"	{ 
		/* leaving scope */
		/* call function to remove scope from symbol table */
		//++scopes_in_use;
		return (int)(*yytext);
}
"main" {
		return _main_;
}

"print" {
		/* keyword for print, NOTE: no return appended */
		 ++num_chars;
		return _print_;
}
"printl" { 
		/* keyword for printline */
		 ++num_chars;
		return _printl_;
	
}

"break"  {
		return _break_;
}

"continue" {
		return _continue_;
}
"return" {
		return _return_;
}

"scan" {
		/* keyword for scanf */
		return _scan_;
}

"int" {
		/* type INTEGER identifier */
		 ++num_chars;
		return _int_;
}

"double" {
		/* type FLOAT identifier */
		 ++num_chars;
		return _double_;
}

"if" {
		return _IF_;
}

"else" {
		return _ELSE_;
}
"void" {
		return _void_;
}

"while" {
		return _WHILE_;
}

([Ww][Ee][Tt]) {
		/* true value */
		/*yank the identifier and putback a 1 */
		yylval.longval = 1;
		num_chars += yyleng; 
		return INTEGER;
}

([Dd][Rr][Yy]) {
		/* false value */
		/*yank the id and putback a 0 */
		yylval.longval = 0;
		num_chars += yyleng; 
		return INTEGER;
}

([A-Za-z_][A-Za-z_0-9]*) {
	/* dont forget to add keywords above this */
	/* identifiers */
	num_chars += yyleng; 
	lex_var_str = (char*)malloc(yyleng);
	stpcpy(lex_var_str, yytext);
	yylval.strval = lex_var_str;
	 return ID;
}

\"(\\.|[^"])*\" {
	/* \"(\\(.|\n)|[^"])*\" { */
	/* char * type (string literal) */
	num_chars += yyleng; 
	yylval.strval = yytext;
	lex_var_str = (char*)malloc(yyleng);
	stpcpy(lex_var_str, yytext);
	return STRING;
}


"**" {
	num_chars += yyleng; 
    /* Exponentiation operator */
    return POW;
	/*
	"++" {
	/* Increment operator * /
	return IDPP;
}

"--" {
	/* Decrement operator * /
	return IDMM;
}
	*/
}

"<=" {
	return LE;
}

">=" {
	return GE;
}

"==" {
	return EQ;
}

"!=" {
	return NEQ;
}

"&&" {
	return AND;
}

"||" {
	return OR;
}

[ \t]+ {
	num_chars += yyleng; 
    /* Eat whitespace */
}

. { ++num_chars;
	/*can remove |\n because it is yanked earlier in lex*/
	/* Potentially want to catch errors here and define all ^^^*/
    /* Return any unmatched character as is */
    return (int)(*yytext);
}

%%
