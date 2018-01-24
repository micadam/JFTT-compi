#include "condition-types.hh"

Condition::Condition() {}
Condition::~Condition() {}

Eq::Eq(Value * val1, Value * val2) : val1(val1), val2(val2) {}

//A = B <=> A >= B && B <= A <=> A - B = 0 AND B - A = 0
std::vector<Command> * Eq::evaluate( VariableStorage& variableStorage) {
	std::vector<Command> * commands;

	if(!variableStorage.validate(val1->identifier) || !variableStorage.validate(val2->identifier)) {
		return new std::vector<Command>();
	}

	if(val1->num && val2->num) {
		commands = new std::vector<Command>();
		if(val1-> numericalValue == val2->numericalValue) {
			commands->push_back(Command("INC"));
		} else {
			commands->push_back(Command("ZERO"));
		}
	} else if(val1->num || val2->num) {
		Value * constValue = (val1->num) ? val1 : val2;
		Value * varValue = (val1->num) ? val2 : val1;

		std::string suffix1;
		long long addr1;

		if(varValue->identifier->iterName == "") {
			suffix1 = "";
			addr1 = variableStorage.getAddress(varValue->identifier);
			commands = new std::vector<Command>();
		} else {
			suffix1 = "I";
			addr1 = 8;
			commands = variableStorage.prepareArray(varValue->identifier, 8);
		}

		if(constValue->numericalValue == 1) {
			commands->push_back(Command("LOAD" + suffix1, addr1));
			commands->push_back(Command("JZERO", 6, true)); //NOPE
			commands->push_back(Command("DEC"));
			commands->push_back(Command("JZERO", 3, true)); //YUP
			commands->push_back(Command("ZERO"));
			commands->push_back(Command("JUMP", 2, true)); //NOPE
			commands->push_back(Command("INC"));
			return commands;
		} else if(constValue->numericalValue == 0) {
			commands->push_back(Command("LOAD" + suffix1, addr1));
			commands->push_back(Command("JZERO", 3, true)); //YUP
			commands->push_back(Command("ZERO"));
			commands->push_back(Command("JUMP", 2, true)); //NOPE
			commands->push_back(Command("INC"));
			return commands;
		} else {
			commands = variableStorage.generateConstant(constValue->numericalValue);
			commands->push_back(Command("INC"));
			commands->push_back(Command("SUB" + suffix1, addr1));
			commands->push_back(Command("JZERO", 6, true)); //NOPE
			commands->push_back(Command("DEC"));
			commands->push_back(Command("JZERO", 3, true)); //YUP
			commands->push_back(Command("ZERO"));
			commands->push_back(Command("JUMP", 2, true)); //NOPE
			commands->push_back(Command("INC"));
			return commands;
		}
	} else {
		commands = new std::vector<Command>();
		std::string suffix1, suffix2;
		long long addr1, addr2;

		if(val1->identifier->iterName != "") {
			suffix1 = "I";
			addr1 = 8;
			std::vector<Command> * commands2 = variableStorage.prepareArray(val1->identifier, addr1);
			commands->insert(commands->end(), commands2->begin(), commands2->end());
		} else {
			suffix1 = "";
			addr1 = variableStorage.getAddress(val1->identifier);
		}

		if(val2->identifier->iterName != "") {
			suffix2 = "I";
			addr2 = 9;
			std::vector<Command> * commands2 = variableStorage.prepareArray(val2->identifier, addr2);
			commands->insert(commands->end(), commands2->begin(), commands2->end());
		} else {
			suffix2 = "";
			addr2 = variableStorage.getAddress(val2->identifier);
		}

		commands->push_back(Command("LOAD" + suffix1, addr1));
		commands->push_back(Command("INC"));
		commands->push_back(Command("SUB" + suffix2, addr2));
		commands->push_back(Command("JZERO", 6, true)); //NOPE
		commands->push_back(Command("DEC"));
		commands->push_back(Command("JZERO", 3, true)); //YUP
		commands->push_back(Command("ZERO"));
		commands->push_back(Command("JUMP", 2, true)); //NOPE
		commands->push_back(Command("INC"));
	}
	return commands;
}

Neq::Neq(Value * val1, Value * val2) : val1(val1), val2(val2) {}

//A != B <=> A > B || A < B <=> A - B != 0 OR B - A != 0
std::vector<Command> * Neq::evaluate( VariableStorage& variableStorage) {
	std::vector<Command> * commands;

	if(!variableStorage.validate(val1->identifier) || !variableStorage.validate(val2->identifier)) {
		return new std::vector<Command>();
	}

	if(val1->num && val2->num) {
		commands = new std::vector<Command>();
		if(val1-> numericalValue != val2->numericalValue) {
			commands->push_back(Command("INC"));
		} else {
			commands->push_back(Command("ZERO"));
		}
	} else if(val1->num || val2->num) {
		Value * constValue = (val1->num) ? val1 : val2;
		Value * varValue = (val1->num) ? val2 : val1;

		std::string suffix1;
		long long addr1;

		if(varValue->identifier->iterName == "") {
			suffix1 = "";
			addr1 = variableStorage.getAddress(varValue->identifier);
			commands = new std::vector<Command>();
		} else {
			suffix1 = "I";
			addr1 = 8;
			commands = variableStorage.prepareArray(varValue->identifier, 8);
		}

		if(constValue->numericalValue == 1) {
			commands->push_back(Command("LOAD" + suffix1, addr1));
			commands->push_back(Command("JZERO", 3, true)); //YUP
			commands->push_back(Command("DEC"));
			commands->push_back(Command("JZERO", 2, true)); //NOPE
			commands->push_back(Command("INC"));
			return commands;
		} else if(constValue->numericalValue == 0) {
			commands->push_back(Command("LOAD" + suffix1, addr1));
			return commands;
		} else {
			commands = variableStorage.generateConstant(constValue->numericalValue);
			commands->push_back(Command("INC"));
			commands->push_back(Command("SUB" + suffix1, addr1));
			commands->push_back(Command("JZERO", 3, true)); //YUP
			commands->push_back(Command("DEC"));
			commands->push_back(Command("JZERO", 2, true)); //NOPE
			commands->push_back(Command("INC")); //YUP
			return commands;
		}
	} else {
		commands = new std::vector<Command>();
		std::string suffix1, suffix2;
		long long addr1, addr2;

		if(val1->identifier->iterName != "") {
			suffix1 = "I";
			addr1 = 8;
			std::vector<Command> * commands2 = variableStorage.prepareArray(val1->identifier, addr1);
			commands->insert(commands->end(), commands2->begin(), commands2->end());
		} else {
			suffix1 = "";
			addr1 = variableStorage.getAddress(val1->identifier);
		}

		if(val2->identifier->iterName != "") {
			suffix2 = "I";
			addr2 = 9;
			std::vector<Command> * commands2 = variableStorage.prepareArray(val2->identifier, addr2);
			commands->insert(commands->end(), commands2->begin(), commands2->end());
		} else {
			suffix2 = "";
			addr2 = variableStorage.getAddress(val2->identifier);
		}

		commands->push_back(Command("LOAD" + suffix1, addr1));
		commands->push_back(Command("INC"));
		commands->push_back(Command("SUB" + suffix2, addr2));
		commands->push_back(Command("JZERO", 3, true)); //YUP
		commands->push_back(Command("DEC"));
		commands->push_back(Command("JZERO", 2, true)); //NOPE
		commands->push_back(Command("INC")); //YUP
	}
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
