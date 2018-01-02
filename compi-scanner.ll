%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <string>
# include "compi-driver.hh"
# include "compi-parser.tab.hh"

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
# undef yywrap
# define yywrap() 1

// The location of the current token.
static yy::location loc;
%}

%option noyywrap nounput batch debug noinput

PIDENTIFIER   [_a-z]+
NUM   [0-9]+
blank [ \t]

%{
	// Code run each time a pattern is matched.
	# define YY_USER_ACTION  loc.columns (yyleng);
%}

%x COMMENT
%%
%{
  // Code run each time yylex is called.
  loc.step ();
%}

"("				{ BEGIN(COMMENT); }
<COMMENT>")" 	{ BEGIN(INITIAL); }
<COMMENT>.

{blank}+   loc.step ();
[\n]+      loc.lines (yyleng); loc.step ();

"VAR"		{ return yy::compi_parser::make_VAR(loc); }
"BEGIN"		{ return yy::compi_parser::make_mBEGIN(loc); }
"END"		{ return yy::compi_parser::make_END(loc); }
"["			{ return yy::compi_parser::make_LBR(loc); }
"]"			{ return yy::compi_parser::make_RBR(loc); }
":="		{ return yy::compi_parser::make_ASSIGN(loc); }
"IF"		{ return yy::compi_parser::make_IF(loc); }
"THEN"		{ return yy::compi_parser::make_THEN(loc); }
"ELSE"		{ return yy::compi_parser::make_ELSE(loc); }
"ENDIF"		{ return yy::compi_parser::make_ENDIF(loc); }
"WHILE"		{ return yy::compi_parser::make_WHILE(loc); }
"DO"		{ return yy::compi_parser::make_DO(loc); }
"ENDWHILE"	{ return yy::compi_parser::make_ENDWHILE(loc); }
"FOR"		{ return yy::compi_parser::make_FOR(loc); }
"FROM"		{ return yy::compi_parser::make_FROM(loc); }
"TO"		{ return yy::compi_parser::make_TO(loc); }
"DOWNTO"	{ return yy::compi_parser::make_DOWNTO(loc); }
"ENDFOR"	{ return yy::compi_parser::make_ENDFOR(loc); }
"READ"		{ return yy::compi_parser::make_READ(loc); }
"WRITE"		{ return yy::compi_parser::make_WRITE(loc); }
"+"			{ return yy::compi_parser::make_ADD(loc); }
"-"			{ return yy::compi_parser::make_SUB(loc); }
"*"			{ return yy::compi_parser::make_MUL(loc); }
"/"			{ return yy::compi_parser::make_DIVI(loc); }
"%"			{ return yy::compi_parser::make_MOD(loc); }
"="			{ return yy::compi_parser::make_EQ(loc); }
"<>"		{ return yy::compi_parser::make_NEQ(loc); }
"<"			{ return yy::compi_parser::make_LT(loc); }
">"			{ return yy::compi_parser::make_GT(loc); }
"<="		{ return yy::compi_parser::make_LEQ(loc); }
">="		{ return yy::compi_parser::make_GEQ(loc); }
";"			{ return yy::compi_parser::make_TERM(loc); }

{NUM} {
	errno = 0;
	long n = strtol (yytext, NULL, 10);
 	return yy::compi_parser::make_NUM(n, loc);
}

{PIDENTIFIER}       return yy::compi_parser::make_PIDENTIFIER(yytext, loc);
.          driver.error (loc, "invalid character");
<<EOF>>    return yy::compi_parser::make_ENDFILE(loc);
%%

void
compi_driver::scan_begin () {
 	yy_flex_debug = trace_scanning;
 	if (file.empty () || file == "-")
    	yyin = stdin;
    else if (!(yyin = fopen (file.c_str (), "r"))) {
    	error ("cannot open " + file + ": " + strerror(errno));
    	exit (EXIT_FAILURE);
    }
}

void
compi_driver::scan_end ()
{
	fclose (yyin);
}