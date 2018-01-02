all: 
	bison compi-parser.yy
	flex compi-scanner.ll
	g++ lex.yy.c compi-parser.tab.cc compi-driver.cc compi.cc helper-types.cc expression-types.cc condition-types.cc -o compi.out