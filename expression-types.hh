#ifndef EXPRESSION_TYPES_HH
#define EXPRESSION_TYPES_HH
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "helper-types.hh"

class Expression {

public:
	Expression();
	~Expression();

	//evaluates expression and stores it inside the accumulator
	virtual std::vector<Command> * evaluate(VariableStorage& variableStorage)=0;
};

class ConstExpr : public Expression {
	Value * val1;

public:
	ConstExpr(Value * val);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

class Add : public Expression {
	Value * val1, *val2;
public:
	Add(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);
};

class Sub : public Expression {
	Value * val1, *val2;
public:
	Sub(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);
};

class Mul : public Expression {
	Value * val1, *val2;
public:
	Mul(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

class Divi : public Expression {
	Value * val1, *val2;
public:
	Divi(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

class Mod : public Expression {
	Value * val1, *val2;
public:
	Mod(Value * val1, Value *val2);
	std::vector<Command> * evaluate(VariableStorage& variableStorage);

};

#endif // ! EXPRESSION_TYPES_HH