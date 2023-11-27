#include <iostream>
#include "FileHandler.hpp"

void	otp_parse_argv(int argc, char *argv[], FileHandler *fileHandler)
{
	if (argc != 3)
        throw std::invalid_argument(
			"\033[31mexpecting ./ft_otp -<option> <key file>\033[0m");
	// If neither "-g" nor "-k" is found, that's an error
	if ((argv[1][0] != '-' && argv[2][0] != '-') ||
		(argv[1][1] != 'g' && argv[2][1] != 'k'))
        throw std::invalid_argument(
			"\033[31mexpecting ./ft_otp -<g/k> <key file>\033[0m");

	if (argv[1][0] == '-') {
		fileHandler->setFilename(argv[1]);
		argv[1][1] == 'g' ?
			fileHandler->setMode(OTP_MODE_SAVE_KEY) :
			fileHandler->setMode(OTP_MODE_GEN_PWD);
	} else {
		fileHandler->setFilename(argv[2]);
		argv[2][1] == 'g' ?
			fileHandler->setMode(OTP_MODE_SAVE_KEY) :
			fileHandler->setMode(OTP_MODE_GEN_PWD);
	}
}

void	otp_save_key(FileHandler *filehandler)
{
	try {
		filehandler->openFile();
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

int	main(int argc, char *argv[])
{
	FileHandler	fileHandler;

	try {
		otp_parse_argv(argc, argv, &fileHandler);
	} catch (std::exception &e) {
		std::cerr << "Invalid argument: " << e.what() << std::endl;
		return 1;
	}
	if (fileHandler.getMode() == OTP_MODE_SAVE_KEY)
		otp_save_key(&fileHandler);
	// else otp_generate_key();

	return 0;
}
