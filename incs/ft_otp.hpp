#ifndef FT_OTP_HPP
# define FT_OTP_HPP

# include <iostream>
# include <stdexcept>

# include "FileHandler.hpp"

void printHelp();
void parseArgv(int argc, char *argv[], FileHandler *fileHandler, bool &verbose);

#endif
