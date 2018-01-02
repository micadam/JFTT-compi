#include "condition-types.hh"

Condition::Condition() {}
Condition::~Condition() {}

Eq::Eq(Value * val1, Value * val2) : val1(val1), val2(val2) {}

//A = B <=> A >= B && B <= A <=> A - B = 0 AND B - A = 0
std::vector<Command> * Eq::evaluate( VariableStorage& variableStorage) {

	std::cerr<<val1->identifier->name<<std::endl;

	Sub * sub = new Sub(val1, val2);
	std::vector<Command> * commands = sub->evaluate(variableStorage);
	delete sub;
	commands->push_back(Command("STORE", 1));
	sub = new Sub(val2, val1);
	std::vector<Command> * commands2 = sub->evaluate(variableStorage);
	delete sub;
	commands->insert(commands->end(), commands2->begin(), commands2->end());
	commands->push_back(Command("ADD", 1));

	commands->push_back(Command("JZERO", 3, true));
	commands->push_back(Command("ZERO"));
	commands->push_back(Command("JUMP", 2, true));
	commands->push_back(Command("INC"));
	return commands;

}

Neq::Neq(Value * val1, Value * val2) : val1(val1), val2(val2) {}

//A != B <=> A > B || A < B <=> A - B != 0 OR B - A != 0
std::vector<Command> * Neq::evaluate( VariableStorage& variableStorage) {
	Sub * sub = new Sub(val1, val2);
	std::vector<Command> * commands = sub->evaluate(variableStorage);
	delete sub;
	commands->push_back(Command("STORE", 1));
	sub = new Sub(val2, val1);
	std::vector<Command> * commands2 = sub->evaluate(variableStorage);
	delete sub;
	commands->insert(commands->end(), commands2->begin(), commands2->end());
	commands->push_back(Command("ADD", 1));
	return commands;	
}

Lt::Lt(Value * val1, Value * val2) : val1(val1), val2(val2) {}

//B - A != 0 <=> A < B
std::vector<Command> * Lt::evaluate( VariableStorage& variableStorage) {
	Sub * sub = new Sub(val2, val1);
	std::vector<Command> * commands = sub->evaluate(variableStorage);
	delete sub;

	return commands;
}

Gt::Gt(Value * val1, Value * val2) : val1(val1), val2(val2) {}

//A - B != 0 <=> A > B
std::vector<Command> * Gt::evaluate( VariableStorage& variableStorage) {
	Sub * sub = new Sub(val1, val2);
	std::vector<Command> * commands = sub->evaluate(variableStorage);
	delete sub;

	return commands;
}

Leq::Leq(Value * val1, Value * val2) : val1(val1), val2(val2) {}

//A <= B  <=> !(A > B) <=> A - B = 0
std::vector<Command> * Leq::evaluate( VariableStorage& variableStorage) {
	Sub * sub = new Sub(val1, val2);
	std::vector<Command> * commands = sub->evaluate(variableStorage);
	delete sub;

	commands->push_back(Command("JZERO", 3, true));
	commands->push_back(Command("ZERO"));
	commands->push_back(Command("JUMP", 2, true));
	commands->push_back(Command("INC"));

	return commands;
}

Geq::Geq(Value * val1, Value * val2) : val1(val1), val2(val2) {}


//A >= B <=> !(A < B) <=> B - A = 0
std::vector<Command> * Geq::evaluate( VariableStorage& variableStorage) {
	Sub * sub = new Sub(val2, val1);
	std::vector<Command> * commands = sub->evaluate(variableStorage);
	delete sub;

	commands->push_back(Command("JZERO", 3, true));
	commands->push_back(Command("ZERO"));
	commands->push_back(Command("JUMP", 2, true));
	commands->push_back(Command("INC"));

	return commands;	
}
