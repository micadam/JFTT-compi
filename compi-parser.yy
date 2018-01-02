%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define parser_class_name {compi_parser}
%define parse.assert
%define api.token.constructor
%define api.value.type variant

%code requires
{
#include <string>
#include "helper-types.hh"
#include "condition-types.hh"
#include "expression-types.hh"
class compi_driver;
}

// The parsing context.
%param { compi_driver& driver }

%locations
%initial-action
{
	// Initialize the initial location.
	@$.begin.filename = @$.end.filename = &driver.file;
};

%define parse.trace
%define parse.error verbose

%code
{
#include "compi-driver.hh"
}

%define api.token.prefix {TOK_}
%token
	ENDFILE  0	"end of file"
	VAR			"VAR"		
	mBEGIN		"BEGIN"		
	END			"END"		
	LBR			"["			
	RBR			"]"			
	ASSIGN 		":="		
	IF			"IF"		
	THEN		"THEN"		
	ELSE		"ELSE"		
	ENDIF		"ENDIF"		
	WHILE		"WHILE"		
	DO			"DO"		
	ENDWHILE	"ENDWHILE"	
	FOR			"FOR"		
	FROM		"FROM"		
	TO			"TO"		
	DOWNTO		"DOWNTO"	
	ENDFOR		"ENDFOR"	
	READ		"READ"		
	WRITE		"WRITE"		
	ADD			"+"			
	SUB			"-"			
	MUL			"*"			
	DIVI		"/"			
	MOD			"%"			
	EQ			"="			
	NEQ			"<>"		
	LT			"<"			
	GT			">"			
	LEQ			"<="		
	GEQ			">="		
	TERM		";"			
;


//typed tokens declarations
%token <std::string> PIDENTIFIER "identifier"
%token <long long> NUM "numerical value"

%type <Identifier *> identifier
%type <Value *> value
%type <std::vector<Command>*> commands
%type <std::vector<Command>*> command
%type <Condition *> condition
%type <Expression *> expression
%type <IterIdentifier *> iterIdentifier
%%
%start program;
program	: 		VAR vdeclarations mBEGIN commands END 							{return driver.finish($4);}
;
vdeclarations :	vdeclarations PIDENTIFIER										{driver.declareVariable($2);}
				| vdeclarations PIDENTIFIER LBR NUM RBR							{driver.declareVariable($2, $4);}
				| %empty
;
commands :		commands command 												{$1->insert($1->end(), $2->begin(), $2->end());
																				$$ = $1;}
				| command 														{$$ = $1;}
;
command :		identifier ASSIGN expression TERM								{$$ = driver.assign($1, $3);}
				| IF condition THEN commands ELSE commands ENDIF 				{$$ = driver.ifelse($2, $4, $6);}
				| IF condition THEN commands ENDIF 								{$$ = driver.ifthen($2, $4);}
				| WHILE condition DO commands ENDWHILE 							{$$ = driver.whiledo($2, $4);}
				| FOR iterIdentifier DO commands ENDFOR						 	{$$ = driver.forto($2, $4);}
				| READ identifier TERM											{$$ = driver.read($2);}
				| WRITE value TERM												{$$ = driver.write($2);}
;

iterIdentifier:	PIDENTIFIER FROM value TO value 								{$$ = new IterIdentifier($1, $3, $5, false, driver.variableStorage);}
				| PIDENTIFIER FROM value DOWNTO value 							{$$ = new IterIdentifier($1, $3, $5, true, driver.variableStorage);}
;

expression :	value 															{$$ = new ConstExpr($1);}
				| value ADD value 												{$$ = new Add($1, $3);}
				| value SUB value 												{$$ = new Sub($1, $3);}
				| value MUL value 												{$$ = new Mul($1, $3);}
				| value DIVI value 												{$$ = new Divi($1, $3);}
				| value MOD value 												{$$ = new Mod($1, $3);}
;
condition :		value EQ value 													{$$ = new Eq($1, $3);}
				| value NEQ value 												{$$ = new Neq($1, $3);}
				| value LT value 												{$$ = new Lt($1, $3);}
				| value GT value 												{$$ = new Gt($1, $3);}
				| value LEQ value 												{$$ = new Leq($1, $3);}
				| value GEQ value 												{$$ = new Geq($1, $3);}
;
value :			NUM																{$$ = new Value($1); }
				| identifier													{$$ = new Value($1); }
;
identifier :	PIDENTIFIER														{$$ = new Identifier($1); } //string
				| PIDENTIFIER LBR PIDENTIFIER RBR								{$$ = new Identifier($1, $3); } //string, string
				| PIDENTIFIER LBR NUM RBR										{$$ = new Identifier($1, $3); } //string, int
;
%%

void yy::compi_parser::error (const location_type& l, const std::string& m) {
	driver.error (l, m);
}