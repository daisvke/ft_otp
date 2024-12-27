#include <iostream>
#include "FileHandler.hpp"

void parseArgv(int argc, char *argv[], FileHandler *fileHandler)
{
	if (argc != 3 ||
		// If neither "-g" nor "-k" is found, that's an error
		(argv[1][0] != '-' && argv[2][0] != '-') ||
		((argv[1][1] != 'g' && argv[2][1] != 'g') &&
		 (argv[1][1] != 'k' && argv[2][1] != 'k')))
		throw std::invalid_argument(
			"\033[31mexpecting ./ft_otp -<g/k> <key file>\033[0m");

	if (argv[1][0] == '-')
	{
		fileHandler->setFilename(argv[2]);
		argv[1][1] == 'g' ? fileHandler->setMode(OTP_MODE_SAVE_KEY) : fileHandler->setMode(OTP_MODE_GEN_PWD);
	}
	else
	{
		fileHandler->setFilename(argv[1]);
		argv[2][1] == 'g' ? fileHandler->setMode(OTP_MODE_SAVE_KEY) : fileHandler->setMode(OTP_MODE_GEN_PWD);
	}
}

void saveKeyToOutFile(FileHandler *filehandler)
{
	try
	{
		std::string key = filehandler->getKeyFromInFile();
		filehandler->saveKeyToOutFile(key);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void generateTOTPKey(FileHandler *filehandler)
{
	std::string TOTPKey;
	try
	{
		const std::string key = filehandler->getKeyFromInFile();
		TOTPKey = TOTPGenerator::generateTOTPHmacSha1(key);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
	std::cout << "\nGenerated TOTP key: " << std::endl;
	std::cout << TOTPKey << std::endl;
}

int main(int argc, char *argv[])
{
	FileHandler fileHandler;

	try
	{
		parseArgv(argc, argv, &fileHandler);
	}
	catch (std::exception &e)
	{
		std::cerr << "Invalid argument: " << e.what() << std::endl;
		return 1;
	}
	if (fileHandler.getMode() == OTP_MODE_SAVE_KEY)
		saveKeyToOutFile(&fileHandler);
	else
		generateTOTPKey(&fileHandler);

	return 0;
}
