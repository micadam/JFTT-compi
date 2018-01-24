#include "helper-types.hh"

Command::Command(std::string oper)
	:oper(oper), arg(-1), offset(false) {

}

Command::Command(std::string oper, long long arg)
	:oper(oper), arg(arg), offset(false) {

}

Command::Command(std::string oper, long long arg, bool offset)
	:oper(oper), arg(arg), offset(offset) {

}

Identifier::Identifier() {

}

Identifier::~Identifier() {
	
}

Identifier::Identifier(std::string name) 
	: name(name), iter(false) {

}

Identifier::Identifier(std::string name, long long iterValue)
	: name(name), iterValue(iterValue), iterName(""), iter(true) {
}

Identifier::Identifier(std::string name, std::string iterName)
	: name(name), iterValue(-1), iterName(iterName), iter(true) {
}

Value::Value() {

}

Value::~Value() {

}

Value::Value(long long numericalValue)
	: numericalValue(numericalValue), identifier(NULL), num(true) {
}

Value::Value(Identifier * identifier)
	: numericalValue(0), identifier(identifier), num(false) {
}


Variable::Variable() {}

Variable::~Variable() {}


Variable::Variable(long long address, bool array)
	: address(address), array(array), iterator(false), initialized(false) {}

Variable::Variable(long long address, bool array, bool iterator)
	: address(address), array(array), iterator(iterator), initialized(false) {}


VariableStorage::VariableStorage() : first_free_address(10), isError(false) {
	specialCommands = new std::vector<Command>;
}

int VariableStorage::declareVariable(const std::string& name, long long len, bool array) {
	return declareVariable(name, len, array, false);
}

int VariableStorage::declareVariable(const std::string& name, long long len, bool array, bool iterator) {
	if(variables.find(name) == variables.end()) {
		variables[name] = Variable(first_free_address, array, iterator);
		first_free_address += len;
	} else {
		std::cerr<<"ERROR: Redeclaration of variable "<<name<<"!"<<std::endl;;
		isError = true;
	}
}

bool VariableStorage::isIterator(const std::string& name) {
	if(variables.find(name) == variables.end()) {
		std::cerr<<("ERROR: no such variable: " + name + "!")<<std::endl;;
		isError = true;
	} else {
		return variables[name].iterator;
	}
}

int VariableStorage::undeclareVariable(const std::string& name) {
	return variables.erase(name);
}

int VariableStorage::initializeVariable(const std::string& name) {
	if(variables.find(name) == variables.end()) {
		std::cerr<<("ERROR: no such variable: " + name + "!")<<std::endl;;
		isError = true;
		return -1;
	} else {
		variables[name].initialized = true;
	}
}



long long VariableStorage::getAddress(Identifier * identifier) {
	if(variables.find(identifier->name) == variables.end()) {
		std::cerr<<"Unknown variable: " + identifier->name + "!"<<std::endl;
		return -1;
	} else {
		if(!identifier->iter) {
			return variables[identifier->name].address;
		} else if(identifier->iterValue != -1) {
			return variables[identifier->name].address + identifier->iterValue;
		} else {
			return -1;
		}
	}
}

long long VariableStorage::getAddress(std::string name) {
	if(variables.find(name) == variables.end()) {
		std::cerr<<"Unknown variable: " + name + "!"<<std::endl;
		return -1;
	} else {
		return variables[name].address;
	}
}

long long VariableStorage::getDummyAddress() {
	return first_free_address++;
}


std::vector<Command> * VariableStorage::generateConstant(long long value) {


	if(value < 0) {
		value = 0;
	}

	std::vector<Command> * commands = new std::vector<Command>;

	bool expensive = false; //TODO expensive number detection

	if(constant_addresses.find(value) == constant_addresses.end()) {  //value is not a quick access value
		//construct a number and save it if it's expensive enough
		//SpecialCommands get inserted at the start of the code

		if(value == 0) {
			commands->push_back(Command("ZERO"));
			return commands;
		}

		std::vector<Command> * thisCommands = expensive ? specialCommands : commands;

		//VARIALBE GENERATION START
		thisCommands->push_back(Command("ZERO"));

		long long tmp = value;
		int msb = 0;

		while(tmp) { //find MSB of multiplier
			tmp = tmp >> 1;
			msb++;;
		} 
		
		long long currentMultiplier = (1L << (msb-1));

		while(currentMultiplier) {
			
			if(currentMultiplier & value) {
				thisCommands->push_back(Command("INC"));
			}

			if(currentMultiplier != 1) {
				thisCommands->push_back(Command("SHL"));
			}
			currentMultiplier = currentMultiplier >> 1;
		}
		//VARIABLE GENERATION END

		if(expensive) {
			specialCommands->push_back(Command("STORE", first_free_address));
			constant_addresses[value] = first_free_address++;
		}

	}

	if(expensive) {
		commands->push_back(Command("LOAD", constant_addresses[value]));
	}

	return commands;
}

std::vector<Command> * VariableStorage::prepareArray(Identifier * identifier, long long address) {
	if(identifier->iterName == "" || !validate(identifier)) {  //shouldn't happen
		std::cerr<<"ERROR while trying to access array"<<std::endl;
		return NULL;
	}

	std::vector<Command> * commands = generateConstant(variables[identifier->name].address);
	commands->push_back(Command("ADD", variables[identifier->iterName].address));
	commands->push_back(Command("STORE", address));

	return commands;
}


bool VariableStorage::validate(Identifier * identifier) {
	if(identifier == NULL) {
		return true;		//I hope it's only possible when this identifier is part of a num value
	}

	if(variables.find(identifier->name) == variables.end()) {
		std::cerr<<("ERROR: No such variable: " + identifier->name)<<std::endl;
		isError = true;
		return false;
	} else if(variables[identifier->name].array != identifier->iter) {
		std::cerr<<("ERROR: Bad variable usage: " + identifier->name + "!")<<std::endl;
		isError = true;
		return false;
	} else if(identifier->iterName != "" && variables.find(identifier->iterName) == variables.end()) {
		std::cerr<<("ERROR: Invalid array index: " + identifier->iterName)<<std::endl;;
		isError = true;
		return false;
	}


	return true;
}

IterIdentifier::IterIdentifier() {

}

IterIdentifier::~IterIdentifier() {
	
}

IterIdentifier::IterIdentifier(std::string name, Value * val1, Value * val2, bool reverse, VariableStorage& variableStorage)
	: name(name), val1(val1), val2(val2), reverse(reverse) {

	variableStorage.declareVariable(name, 1, false, true);
}
