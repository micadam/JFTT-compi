#ifndef CONDITION_TYPES_HH
#define CONDITION_TYPES_HH
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "helper-types.hh"
#include "expression-types.hh"

class Condition {

public:
	Condition();
	~Condition();
	Condition(Value * val1, Value *val2);
	virtual std::vector<Command> * evaluate(VariableStorage& variableStorage) = 0;
};

class Eq : public Condition {
	Value * val1, *val2;
public:
	Eq(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

class Neq : public Condition {
	Value * val1, *val2;

public:
	Neq(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

class Lt : public Condition {
	Value * val1, *val2;

public:
	Lt(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

class Gt : public Condition {
	Value * val1, *val2;

public:
	Gt(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

class Leq : public Condition {
	Value * val1, *val2;

public:
	Leq(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

class Geq : public Condition {
	Value * val1, *val2;

public:
	Geq(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

#endif // ! EXPRESSION_TYPES_HH