#include "expression-types.hh"

Expression::Expression() {}
Expression::~Expression() {}

ConstExpr::ConstExpr(Value * val1) : val1(val1) {}

std::vector<Command> * ConstExpr::evaluate(VariableStorage& variableStorage) {

	if(!variableStorage.validate(val1->identifier)) {
		return new std::vector<Command>();
	}

	std::vector<Command> * commands;
	if(val1->num) {
		if(val1->numericalValue == 0) {
			commands = new std::vector<Command>();
			commands->push_back(Command("ZERO"));
		} else if (val1->numericalValue == 1) {
			commands = new std::vector<Command>();
			commands->push_back(Command("ZERO"));
			commands->push_back(Command("INC"));
		} else {
			commands = variableStorage.generateConstant(val1->numericalValue);
		}
	} else if (val1->identifier->iterName == "") {
		commands = new std::vector<Command>();
		commands->push_back(Command("LOAD", variableStorage.getAddress(val1->identifier)));
	} else {
		commands = variableStorage.prepareArray(val1->identifier, 8);
		commands->push_back(Command("LOADI", 8));

	}

	return commands;
}


Add::Add(Value * val1, Value * val2) : val1(val1), val2(val2) {}

std::vector<Command> * Add::evaluate(VariableStorage& variableStorage) {
	std::vector<Command> * commands;

	if(!variableStorage.validate(val1->identifier) || !variableStorage.validate(val2->identifier)) {
		return new std::vector<Command>();
	}

	if(val1->num && val2->num) {
		commands = variableStorage.generateConstant(val1->numericalValue + val2->numericalValue);
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
			commands->push_back(Command("INC"));
			return commands;
		} else if(constValue->numericalValue == 0) {
			commands->push_back(Command("LOAD" + suffix1, addr1));
			return commands;
		} else {
			commands = variableStorage.generateConstant(constValue->numericalValue);
			commands->push_back(Command("ADD" + suffix1, addr1));
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
		commands->push_back(Command("ADD" + suffix2, addr2));
	}

	return commands;
}

Sub::Sub(Value * val1, Value * val2) : val1(val1), val2(val2) {}

std::vector<Command> * Sub::evaluate(VariableStorage& variableStorage) {
	std::vector<Command> * commands;

	if(!variableStorage.validate(val1->identifier) || !variableStorage.validate(val2->identifier)) {
		return new std::vector<Command>();
	}

	if(val1->num && val2->num) {
		commands = variableStorage.generateConstant(val1->numericalValue - val2->numericalValue);
	} else if(val1->num) {
		if(val1->numericalValue == 0) {
			commands = new std::vector<Command>();
			commands->push_back(Command("ZERO"));
			return commands;
		}

		std::string suffix2;
		long long addr2;
		if(val2->identifier->iterName != "") {
			suffix2 = "I";
			addr2 = 9;
			commands = variableStorage.prepareArray(val2->identifier, addr2);
		} else {
			suffix2 = "";
			addr2 = variableStorage.getAddress(val2->identifier);
			commands = new std::vector<Command>();
		}

		std::vector<Command> * commands2 = variableStorage.generateConstant(val1->numericalValue);
		commands->insert(commands->end(), commands2->begin(), commands2->end());

		commands->push_back(Command("SUB" + suffix2, addr2));
	} else if(val2->num) {
		std::string suffix1;
		long long addr1; 
		if(val1->identifier->iterName != "") {
			suffix1 = "I";
			addr1 = 8;
			commands = variableStorage.prepareArray(val1->identifier, addr1);
		} else {
			suffix1 = "";
			addr1 = variableStorage.getAddress(val1->identifier);
			commands = new std::vector<Command>();
		}

		if(val2->numericalValue == 0) {
			commands->push_back(Command("LOAD" + suffix1, addr1));
		} else if(val2->numericalValue == 1) {
			commands->push_back(Command("LOAD" + suffix1, addr1));
			commands->push_back(Command("DEC"));
		} else {
			std::vector<Command> * commands2 = variableStorage.generateConstant(val2->numericalValue);
			commands->insert(commands->end(), commands2->begin(), commands2->end());
			commands->push_back(Command("STORE", 0));
			commands->push_back(Command("LOAD" + suffix1, addr1));
			commands->push_back(Command("SUB", 0));
		}

	} else {
		std::string suffix1, suffix2;
		long long addr1, addr2;

		if(val1->identifier->iterName != "") {
			suffix1 = "I";
			addr1 = 8;
			commands = variableStorage.prepareArray(val1->identifier, addr1);
		} else {
			suffix1 = "";
			addr1 = variableStorage.getAddress(val1->identifier);
			commands = new std::vector<Command>();
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
		commands->push_back(Command("SUB" + suffix2, addr2));
	}

	return commands;
}

Mul::Mul(Value * val1, Value * val2) : val1(val1), val2(val2) {}

std::vector<Command> * Mul::evaluate(VariableStorage& variableStorage) {
	std::vector<Command> * commands;
	if(!variableStorage.validate(val1->identifier) || !variableStorage.validate(val2->identifier)) {
		return new std::vector<Command>();
	}

	if(val1->num && val2->num) {
		commands = variableStorage.generateConstant(val1->numericalValue * val2->numericalValue);
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

		long long multiplier = constValue->numericalValue;
		commands->push_back(Command("ZERO"));

		long long tmp = multiplier;
		int msb = 0;
		while(tmp) { //find MSB of multiplier
			tmp = (tmp >> 1);
			msb++;
		} 
		long long currentMultiplier = (1L << (msb - 1));

		while(currentMultiplier) {
			
			if(currentMultiplier & multiplier) {
				commands->push_back(Command("ADD" + suffix1, addr1));
			}
			if(currentMultiplier != 1) {
				commands->push_back(Command("SHL"));
			}
			currentMultiplier /= 2;
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

		//1 - smaller variable (shifted right)
		//2 - bigger variable (shifted left)
		//3 - result
		commands->push_back(Command("ZERO"));
		commands->push_back(Command("STORE", 3));
		commands->push_back(Command("LOAD" + suffix1, addr1));
		commands->push_back(Command("SUB" + suffix2, addr2));
		commands->push_back(Command("JZERO", 6, true));
		//val1 - val2 > 0, so val1 > val2, val2 should be the multiplier
		commands->push_back(Command("LOAD" + suffix1, addr1));
		commands->push_back(Command("STORE", 2));
		commands->push_back(Command("LOAD" + suffix2, addr2));
		commands->push_back(Command("STORE", 1));
		commands->push_back(Command("JUMP", 7, true)); //goto GO + 1
		//val1 - val2 <= 0, so val1 <= val2, val1 should be the multiplier
		commands->push_back(Command("LOAD" + suffix2, addr2));
		commands->push_back(Command("STORE", 2));
		commands->push_back(Command("LOAD" + suffix1, addr1));
		commands->push_back(Command("STORE", 1));
		commands->push_back(Command("JUMP", 2, true)); //goto GO + 1

		//proper multiplication (real men only):
		commands->push_back(Command("LOAD", 1)); //label GO
		commands->push_back(Command("JZERO", 16, true)); //goto DONE
		commands->push_back(Command("JODD", 7, true)); //goto DOSTUFF add B to result, SHR A, SHL B

		commands->push_back(Command("SHR")); //label DONTDOSTUFF
		commands->push_back(Command("STORE", 1));
		commands->push_back(Command("LOAD", 2));
		commands->push_back(Command("SHL"));
		commands->push_back(Command("STORE", 2));
		commands->push_back(Command("JUMP", -8, true)); //goto GO

		commands->push_back(Command("SHR")); //label DOSTUFF
		commands->push_back(Command("STORE", 1));
		commands->push_back(Command("LOAD", 2));
		commands->push_back(Command("SHL"));
		commands->push_back(Command("STORE", 2));
		commands->push_back(Command("ADD", 3));
		commands->push_back(Command("STORE", 3));
		commands->push_back(Command("JUMP", -16, true)); //goto GO

		commands->push_back(Command("LOAD", 3)); //label DONE
		commands->push_back(Command("SHR"));		
	}

	return commands;
}

Divi::Divi(Value * val1, Value * val2) : val1(val1), val2(val2) {}

std::vector<Command> * Divi::evaluate(VariableStorage& variableStorage) {
	std::vector<Command> * commands;

	if(!variableStorage.validate(val1->identifier) || !variableStorage.validate(val2->identifier)) {
		return new std::vector<Command>();
	}

	if(val1->num && val2->num) {
		if(val2->numericalValue == 0) {
			commands = new std::vector<Command>();
			commands->push_back(Command("ZERO"));
		} else {
			commands = variableStorage.generateConstant(val1->numericalValue / val2->numericalValue);
		}

		return commands;
	} else if(val2->num) {
		if(val2->numericalValue == 0) {
			commands = new std::vector<Command>();
			commands->push_back(Command("ZERO"));
			return commands;
		} else if((val2->numericalValue & (val2->numericalValue - 1)) == 0)  { //val2 is a power of 2, just shift val1 to the right
			int msb = 0;
			long long val = val2->numericalValue;
			while(val) {
				val /= 2;
				msb++;
			}


			std::string suffix1;
			long long addr1;
			if(val1->identifier->iterName != "") {
				suffix1 = "I";
				addr1 = 8;
				commands = variableStorage.prepareArray(val1->identifier, addr1);
			} else {
				suffix1 = "";
				addr1 = variableStorage.getAddress(val1->identifier);
				commands = new std::vector<Command>();
			}

			commands->push_back(Command("LOAD" + suffix1, addr1));

			for(int i = 0; i < msb - 1; i++) {
				commands->push_back(Command("SHR"));
			}
			return commands;
		}
	}

	//Load the values into their registers
	commands = new std::vector<Command>();
	commands->push_back(Command("ZERO"));
	commands->push_back(Command("STORE", 3));
	commands->push_back(Command("INC"));
	commands->push_back(Command("STORE", 4));
	if(val1->num) {
		std::vector<Command> * commands2 = variableStorage.generateConstant(val1->numericalValue);
		commands->insert(commands->end(), commands2->begin(), commands2->end());
		commands->push_back(Command("STORE", 1));
	} else {
		std::string suffix1;
		long long addr1;
		if(val1->identifier->iterName != "") {
			suffix1 = "I";
			addr1 = 8;
			std::vector<Command> * commands2 = variableStorage.prepareArray(val1->identifier, addr1);
			commands->insert(commands->end(), commands2->begin(), commands2->end());
		} else {
			suffix1 = "";
			addr1 = variableStorage.getAddress(val1->identifier);
		}
		commands->push_back(Command("LOAD" + suffix1, addr1));
		commands->push_back(Command("STORE", 1));
	}

	if(val2-> num) {
		std::vector<Command> * commands2 = variableStorage.generateConstant(val2->numericalValue);
		commands->insert(commands->end(), commands2->begin(), commands2->end());
		commands->push_back(Command("STORE", 2));
	} else {
		std::string suffix2;
		long long addr2;
		if(val2->identifier->iterName != "") {
			suffix2 = "I";
			addr2 = 9;
			std::vector<Command> * commands2 = variableStorage.prepareArray(val2->identifier, addr2);
			commands->insert(commands->end(), commands2->begin(), commands2->end());
		} else {
			suffix2 = "";
			addr2 = variableStorage.getAddress(val2->identifier);
		}

		commands->push_back(Command("LOAD" + suffix2, addr2));
		commands->push_back(Command("STORE", 2));	
	}
	

	//1 - remainder
	//2 - divider
	//3 - result
	//4 - multiplier

	//zero disivion check
	commands->push_back(Command("JZERO", 30, true)); //done + 1

	//align a and b
	commands->push_back(Command("SHL")); //LABEL start1
	commands->push_back(Command("SUB", 1));
	commands->push_back(Command("JZERO", 2, true)); //ok
	commands->push_back(Command("JUMP", 8, true)); //next1
	commands->push_back(Command("LOAD", 4)); //LABEL ok
	commands->push_back(Command("SHL"));
	commands->push_back(Command("STORE", 4));
	commands->push_back(Command("LOAD", 2));
	commands->push_back(Command("SHL"));
	commands->push_back(Command("STORE", 2));
	commands->push_back(Command("JUMP", -10, true)); //start1

	//proceed with division
	commands->push_back(Command("LOAD", 1)); //LABEL next1
	commands->push_back(Command("INC"));
	commands->push_back(Command("SUB", 2));
	commands->push_back(Command("JZERO", 6, true)); //no     a + 1 - b <= 0, a < b
	commands->push_back(Command("DEC"));
	commands->push_back(Command("STORE", 1));
	commands->push_back(Command("LOAD", 3));
	commands->push_back(Command("ADD", 4));
	commands->push_back(Command("STORE", 3));
	commands->push_back(Command("LOAD", 2)); //LABEL no
	commands->push_back(Command("SHR"));
	commands->push_back(Command("STORE", 2));
	commands->push_back(Command("LOAD", 4));
	commands->push_back(Command("SHR"));
	commands->push_back(Command("JZERO", 3, true)); //done 
	commands->push_back(Command("STORE", 4));
	commands->push_back(Command("JUMP", -16, true)); //next1
	commands->push_back(Command("LOAD", 3)); //LABEL done
		

	return commands;
}

Mod::Mod(Value * val1, Value * val2) : val1(val1), val2(val2) {}

std::vector<Command> * Mod::evaluate(VariableStorage& variableStorage) {
	std::vector<Command> * commands;

	if(!variableStorage.validate(val1->identifier) || !variableStorage.validate(val2->identifier)) {
		return new std::vector<Command>();
	}

	if(val1->num && val2->num) {
		if(val2->numericalValue == 0) {
			commands = new std::vector<Command>();
			commands->push_back(Command("ZERO"));
		} else {
			commands = variableStorage.generateConstant(val1->numericalValue % val2->numericalValue);
		}

		return commands;
	} else if(val2->num) {
		if(val2->numericalValue == 0) {
			commands = new std::vector<Command>();
			commands->push_back(Command("ZERO"));
			return commands;
		} else if((val2->numericalValue & (val2->numericalValue - 1)) == 0)  { //val2 is a power of 2, trim val1 to the last k bits
			int msb = 0;
			long long val = val2->numericalValue;
			while(val) {
				val /= 2;
				msb++;
			}

			std::string suffix1;
			long long addr1;
			if(val1->identifier->iterName != "") {
				suffix1 = "I";
				addr1 = 8;
				commands = variableStorage.prepareArray(val1->identifier, addr1);
			} else {
				suffix1 = "";
				addr1 = variableStorage.getAddress(val1->identifier);
				commands = new std::vector<Command>();
			}

			commands->push_back(Command("LOAD" + suffix1, addr1));

			for(int i = 0; i < msb - 1; i++) {
				commands->push_back(Command("SHR"));
			}
			for(int i = 0; i < msb - 1; i++) {
				commands->push_back(Command("SHL"));
			}
			commands->push_back(Command("STORE", 0));
			commands->push_back(Command("LOAD" + suffix1, addr1));
			commands->push_back(Command("SUB", 0));

			return commands;
		}
	}


	//Load the values into their registers
	commands = new std::vector<Command>();
	if(val1->num) {
		std::vector<Command> * commands2 = variableStorage.generateConstant(val1->numericalValue);
		commands->insert(commands->end(), commands2->begin(), commands2->end());
		commands->push_back(Command("STORE", 1));
	} else {
		std::string suffix1;
		long long addr1;
		if(val1->identifier->iterName != "") {
			suffix1 = "I";
			addr1 = 8;
			std::vector<Command> * commands2 = variableStorage.prepareArray(val1->identifier, addr1);
			commands->insert(commands->end(), commands2->begin(), commands2->end());
		} else {
			suffix1 = "";
			addr1 = variableStorage.getAddress(val1->identifier);
		}
		commands->push_back(Command("LOAD" + suffix1, addr1));
		commands->push_back(Command("STORE", 1));
	}


	if(val2-> num) {
		std::vector<Command> * commands2 = variableStorage.generateConstant(val2->numericalValue);
		commands->insert(commands->end(), commands2->begin(), commands2->end());
		commands->push_back(Command("STORE", 2));
	} else {
		std::string suffix2;
		long long addr2;
		if(val2->identifier->iterName != "") {
			suffix2 = "I";
			addr2 = 9;
			std::vector<Command> * commands2 = variableStorage.prepareArray(val2->identifier, addr2);
			commands->insert(commands->end(), commands2->begin(), commands2->end());
		} else {
			suffix2 = "";
			addr2 = variableStorage.getAddress(val2->identifier);
		}

		commands->push_back(Command("LOAD" + suffix2, addr2));
		commands->push_back(Command("STORE", 2));	
	}
	



	//1 - remainder
	//2 - divider
	//3 - limit

	//zero disivion check
	commands->push_back(Command("JZERO", 29, true)); //done + 1
	commands->push_back(Command("STORE", 3));
	//align a and b
	commands->push_back(Command("SHL")); //LABEL start1
	commands->push_back(Command("SUB", 1));
	commands->push_back(Command("JZERO", 2, true)); //ok
	commands->push_back(Command("JUMP", 5, true)); //next1
	commands->push_back(Command("LOAD", 2)); //LABEL ok
	commands->push_back(Command("SHL"));
	commands->push_back(Command("STORE", 2));
	commands->push_back(Command("JUMP", -7, true)); //start1
	//proceed with division
	//if a < b at start, don't do anything
	commands->push_back(Command("LOAD", 1));   //LABEL next1
	commands->push_back(Command("INC"));
	commands->push_back(Command("SUB", 2));
	commands->push_back(Command("JZERO", 15, true)); //done
	commands->push_back(Command("JUMP", 5, true));
	//proper modulo loop
	commands->push_back(Command("LOAD", 1));
	commands->push_back(Command("INC"));
	commands->push_back(Command("SUB", 2));
	commands->push_back(Command("JZERO", 6, true)); //no			b > a, don't subtract
	commands->push_back(Command("DEC"));
	commands->push_back(Command("STORE", 1));
	commands->push_back(Command("INC"));
	commands->push_back(Command("SUB", 3));
	commands->push_back(Command("JZERO", 5, true)); //done
	commands->push_back(Command("LOAD", 2)); //LABEL no
	commands->push_back(Command("SHR"));
	commands->push_back(Command("STORE", 2));
	commands->push_back(Command("JUMP", -12, true)); //next1
	commands->push_back(Command("LOAD", 1)); //LABEL done
	
	return commands;
}
