#include "compi-driver.hh"
#include "compi-parser.tab.hh"


compi_driver::compi_driver ()
	: trace_scanning (false), trace_parsing (false), isError(false) {}

compi_driver::~compi_driver () {}

int compi_driver::parse (const std::string &f) {
	file = f;
	scan_begin();
	yy::compi_parser parser(*this);
	parser.set_debug_level(trace_parsing);
	int res = parser.parse();
	scan_end();

	return res;
}

void compi_driver::error(const yy::location& l, const std::string& m) {
	std::cerr << l << ": " << m << std::endl;
	isError = true;
	result = 1;
}

void compi_driver::error(const std::string& m) {
	std::cerr << m << std::endl;
	result = 1;
}

int compi_driver::finish(std::vector<Command> * commands) {
	if(!isError && !variableStorage.isError) {
		commands->insert(commands->begin(), variableStorage.specialCommands->begin(), variableStorage.specialCommands->end());
		for(int i = 0; i < commands->size(); i++) {
			if(commands->at(i).arg != -1) {
				long long realArg = commands->at(i).arg;
				if(commands->at(i).offset) {
					realArg += i;
				}
				std::cout<<commands->at(i).oper<<" "<<realArg<<std::endl;;

			} else {		
				std::cout<<commands->at(i).oper<<std::endl;;
			}
		}

		//last command
		std::cout<<"HALT"<<std::endl;
	} else {
		result = 1;
	}

	return result;
}


int compi_driver::declareVariable(const std::string& name) {
	variableStorage.declareVariable(name, 1, false);
}

int compi_driver::declareVariable(const std::string& name, long long len) {
	variableStorage.declareVariable(name, len, true);
}



std::vector<Command> * compi_driver::assign(Identifier * identifier, Expression * expression) {
	if(!variableStorage.validate(identifier)) {
		isError = true;
		return new std::vector<Command>();
	}

	if(variableStorage.isIterator(identifier->name)) {
		isError = true;
		std::cerr<<"Invalid assigment of an operator!"<<std::endl;
		return new std::vector<Command>();
	}

	std::vector<Command> * commands;

	if(identifier->iter && identifier->iterName != "") {
		commands = variableStorage.prepareArray(identifier, 7);
	} else {
		commands = new std::vector<Command>();
	}

	std::vector<Command> * commands2 = expression->evaluate(variableStorage);
	commands->insert(commands->end(), commands2->begin(), commands2->end());

	if(!identifier->iter || identifier->iterValue != -1) {
		commands->push_back(Command("STORE", variableStorage.getAddress(identifier)));
	} else {
		commands->push_back(Command("STOREI", 7));
	}

	if(!identifier->iter) {
		variableStorage.initializeVariable(identifier->name);
	}

	return commands;

}

std::vector<Command> * compi_driver::ifelse(Condition * condition, std::vector<Command> * thencommands,
														std::vector<Command> * elsecommands) {

		std::vector<Command> * commands = condition->evaluate(variableStorage);
		commands->push_back(Command("JZERO", thencommands->size() + 2, true)); //condition not fullfilled. +2 because there's an extra jump at the end
		commands->insert(commands->end(), thencommands->begin(), thencommands->end());
		commands->push_back(Command("JUMP", elsecommands->size() + 1, true)); //jump  over the else block
		commands->insert(commands->end(), elsecommands->begin(), elsecommands->end());
		return commands;
}

std::vector<Command> * compi_driver::ifthen(Condition * condition, std::vector<Command> * thencommands) {
		std::vector<Command> * commands = condition->evaluate(variableStorage);
		commands->push_back(Command("JZERO", thencommands->size() + 1, true));
		commands->insert(commands->end(), thencommands->begin(), thencommands->end());
		return commands;
}

std::vector<Command> * compi_driver::whiledo(Condition * condition, std::vector<Command> * docommands) {

		std::vector<Command> * commands = condition->evaluate(variableStorage);
		long long conditionBlockSize = commands->size();
		commands->push_back(Command("JZERO", docommands->size() + 2, true));
		commands->insert(commands->end(), docommands->begin(), docommands->end());

		commands->push_back(Command("JUMP", -(conditionBlockSize + 1 + docommands->size()), true));
		return commands;
}

std::vector<Command> * compi_driver::forto(IterIdentifier * iterIdentifier, std::vector<Command> * docommands) {
	std::vector<Command> * commands = new std::vector<Command>();
	Value * val1 = iterIdentifier->val1;
	Value * val2 = iterIdentifier->val2;


	long long iterAddr = variableStorage.getAddress(iterIdentifier->name);
	long long limitAddr = variableStorage.getDummyAddress();

	
	//initialize iterator variable
	if(val1->num) {
		std::vector<Command> * commands2 = variableStorage.generateConstant(val1->numericalValue);
		commands->insert(commands->end(), commands2->begin(), commands2->end());
		commands->push_back(Command("STORE", iterAddr));
	} else if(val1->identifier->iterName != "") {
		std::vector<Command> * commands2 = variableStorage.prepareArray(val1->identifier, 6);
		commands->insert(commands->end(), commands2->begin(), commands2->end());

		commands->push_back(Command("LOADI", 6));
		commands->push_back(Command("STORE", iterAddr));
	} else {
		commands->push_back(Command("LOAD", variableStorage.getAddress(val1->identifier)));
		commands->push_back(Command("STORE", iterAddr));
	}

	//specify iterator variable limit
	if(val2->num) {
		std::vector<Command> * commands2 = variableStorage.generateConstant(val2->numericalValue);
		commands->insert(commands->end(), commands2->begin(), commands2->end());
		commands->push_back(Command("STORE", limitAddr));
	} else if(val2->identifier->iterName != "") {
		std::vector<Command> * commands2 = variableStorage.prepareArray(val2->identifier, 6);
		commands->insert(commands->end(), commands2->begin(), commands2->end());

		commands->push_back(Command("LOADI", 6));
		commands->push_back(Command("STORE", limitAddr));
	} else {
		commands->push_back(Command("LOAD", variableStorage.getAddress(val2->identifier)));
		commands->push_back(Command("STORE", limitAddr));
	}

	if(!iterIdentifier->reverse) { //need iter <= limit
		commands->push_back(Command("LOAD", iterAddr));
		commands->push_back(Command("SUB", limitAddr));
		commands->push_back(Command("JZERO", 2, true)); // iter <= limit
		commands->push_back(Command("JUMP", docommands->size() + 5, true)); //iter > limit

		commands->insert(commands->end(), docommands->begin(), docommands->end());

		commands->push_back(Command("LOAD", iterAddr));
		commands->push_back(Command("INC"));
		commands->push_back(Command("STORE", iterAddr));

		commands->push_back(Command("JUMP", -(docommands->size() + 6), true));		
	} else { //DOWNTO
			 //need iter >= limit <=> iter + 1 > limit
		commands->push_back(Command("LOAD", iterAddr));
		commands->push_back(Command("INC"));
		commands->push_back(Command("SUB", limitAddr));
		commands->push_back(Command("JZERO", docommands->size() + 8, true)); // iter < limit

		commands->insert(commands->end(), docommands->begin(), docommands->end());

		commands->push_back(Command("LOAD", iterAddr));
		commands->push_back(Command("SUB", limitAddr));
		commands->push_back(Command("JZERO", 5, true)); // iter <= limit

		commands->push_back(Command("LOAD", iterAddr));
		commands->push_back(Command("DEC"));
		commands->push_back(Command("STORE", iterAddr));


		commands->push_back(Command("JUMP", -(docommands->size() + 6), true));
	}

	variableStorage.undeclareVariable(iterIdentifier->name);
	return commands; 
}

std::vector<Command> * compi_driver::read(Identifier * identifier) {
	std::vector<Command> * commands;
	if(!variableStorage.validate(identifier)) {
		isError = true;
		return new std::vector<Command>();
	}

	if(!identifier->iter || identifier->iterValue != -1) {
		commands = new std::vector<Command>();
		commands->push_back(Command("GET"));
		commands->push_back(Command("STORE", variableStorage.getAddress(identifier)));
	} else {
		commands = variableStorage.prepareArray(identifier, 7);
		commands->push_back(Command("GET"));
		commands->push_back(Command("STOREI",7));

	}
	return commands;
}

std::vector<Command> * compi_driver::write(Value * value) {
	std::vector<Command> * commands;
	if(value->num) {
		commands = variableStorage.generateConstant(value->numericalValue);
		commands->push_back(Command("PUT"));
	} else {
		commands = new std::vector<Command>();
		Identifier * identifier = value->identifier;
		if(!variableStorage.validate(identifier)) {
			isError = true;
			return commands;
		}

		if(!identifier->iter || identifier->iterValue != -1) {
			commands->push_back(Command("LOAD", variableStorage.getAddress(identifier)));
			commands->push_back(Command("PUT"));
		} else {
			commands = variableStorage.prepareArray(identifier, 7);
			commands->push_back(Command("LOADI",7));
			commands->push_back(Command("PUT"));
		}
	}
	return commands;
}
