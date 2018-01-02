#ifndef COMPI_DRIVER_HH
#define COMPI_DRIVER_HH
#include <string>
#include <unordered_map>
#include <vector>
#include "compi-parser.tab.hh"

// Tell Flex the lexer's prototype ...
# define YY_DECL \
	yy::compi_parser::symbol_type yylex (compi_driver& driver)
// ... and declare it for the parser's sake.
YY_DECL;



enum Var_Type {VAR_NUM, VAR_ARRAY, VAR_CONST};

class Identifier;
class Value;

// Conducting the whole scanning and parsing
class compi_driver {
	bool isError;
public:
	compi_driver();
	virtual ~compi_driver();

	int result;

	// Handling the scanner.
	void scan_begin ();
	void scan_end ();
	bool trace_scanning;

	// Run the parser on file F.
	// Return 0 on success.
	int parse (const std::string& f);
	// The name of the file being parsed.
	// Used later to pass the file name to the location tracker.
	std::string file;
	// Whether parser traces should be generated.
	bool trace_parsing;

	VariableStorage variableStorage;

	// Error handling.
	void error (const yy::location& l, const std::string& m);
	void error (const std::string& m);

	int finish(std::vector<Command> * commands);
	int declareVariable(const std::string& name);
	int declareVariable(const std::string& name, long long le);

	std::vector<Command> * assign(Identifier * identifier, Expression * expression);
	std::vector<Command> * ifelse(Condition * condition, std::vector<Command> * thencommands,
															std::vector<Command> * elsecommands);
	std::vector<Command> * ifthen(Condition * condition, std::vector<Command> * thencommands);
	std::vector<Command> * whiledo(Condition * condition, std::vector<Command> * docommands);
	std::vector<Command> * forto(IterIdentifier * iterIdentifier, std::vector<Command> * docommands);
	std::vector<Command> * read(Identifier * identifier);
	std::vector<Command> * write(Value * value);

};
#endif // ! COMPI_DRIVER_HH

