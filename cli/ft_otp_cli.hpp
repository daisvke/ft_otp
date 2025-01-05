#ifndef FT_OTP_CLI_HPP
# define FT_OTP_CLI_HPP

# include <iostream>
# include <stdexcept>

# include "../core/FileHandler.hpp"
# include "../core/qrencode.hpp"

void printHelp();
void parseArgv(int argc, char *argv[], FileHandler *fileHandler, bool &verbose);

#endif
