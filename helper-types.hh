#ifndef HELPER_TYPES_HH
#define HELPER_TYPES_HH
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

struct Command {
	std::string oper;
	long long arg;
	bool offset;

	Command(std::string oper);
	Command(std::string oper, long long arg);
	Command(std::string oper, long long arg, bool offset);

};

class Identifier {
public:
	Identifier();
	~Identifier();

	bool iter;
	std::string name;
	long long iterValue;
	std::string iterName;

	Identifier(std::string name);
	Identifier(std::string name, long long iterValue);
	Identifier(std::string name, std::string iterName);
};

class Value {
public:
	Value();
	~Value();

	bool num;
	long long numericalValue;
	Identifier * identifier;

	Value(long long numericalValue);
	Value(Identifier * identifier);
};

class Variable {
public:
	Variable();
	~Variable();

	long long address;
	bool array;
	bool initialized;
	bool iterator;

	Variable(long long address, bool array);
	Variable(long long address, bool array, bool iterator);
};

class VariableStorage {
	int first_free_address;
	std::unordered_map<std::string, Variable> variables;
	std::unordered_map<long long, long long> constant_addresses;

public:
	std::vector<Command> * specialCommands;

	VariableStorage();

	bool isError;
	int declareVariable(const std::string& name, long long len, bool array);
	int declareVariable(const std::string& name, long long len, bool array, bool iterator);
	int initializeVariable(const std::string& name);
	int undeclareVariable(const std::string& name);
	long long getAddress(Identifier * identifier);
	long long getAddress(std::string name);
	long long getDummyAddress();
	bool isIterator(const std::string& name);
	bool validate(Identifier * identifier);
	std::vector<Command> * generateConstant(long long val);
	std::vector<Command> * prepareArray(Identifier * identifier, long long address);
};

class IterIdentifier {
public:
	IterIdentifier();
	~IterIdentifier();

	std::string name;
	Value * val1;
	Value * val2;
	bool reverse;

	IterIdentifier(std::string name, Value * val1, Value * val2, bool reverse, VariableStorage& variableStorage);
};

#endif // ! HELPER_TYPES_HH