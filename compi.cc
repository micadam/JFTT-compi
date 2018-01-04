#include <iostream>
#include "compi-driver.hh"

int main (int argc, char *argv[]) {
  int res = 0;
  compi_driver driver;
  if(argc == 1) {
    
    if(driver.parse("-")) {
      std::cerr<< driver.result << std::endl;
      res = driver.result;
    };
  }
  for (int i = 1; i < argc; ++i) {
    if (argv[i] == std::string ("-p")) {
      driver.trace_parsing = true;
    } else if (argv[i] == std::string ("-s")) {
      driver.trace_scanning = true;
    } else if (driver.parse (argv[i])) {
      std::cerr << driver.result << std::endl;
      res = res | 1;
    } else {
      res = res | 0;
    }
  }

  return res;
}
