#ifndef PARSE_ARGS_HPP
#define PARSE_ARGS_HPP

#include <iostream>
#include "FileHandler.hpp"

void printHelp();
void parseArgv(int argc, char *argv[], FileHandler *fileHandler, bool &verbose);

#endif
