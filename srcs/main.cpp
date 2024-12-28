#include "FileHandler.hpp"

void parseArgv(int argc, char *argv[], FileHandler *fileHandler)
{
	if (argc != 3 ||
		// If neither "-g" nor "-k" is found, that's an error
		(argv[1][0] != '-' && argv[2][0] != '-') ||
		((argv[1][1] != 'g' && argv[2][1] != 'g') &&
		 (argv[1][1] != 'k' && argv[2][1] != 'k')))
		throw std::invalid_argument("expecting ./ft_otp -<g/k> <key file>");

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

// Encrypt and save the key to an external file (-g)
void saveKeyToOutFile(FileHandler *filehandler)
{
	try
	{
		// Get the original secret from the given file
		std::string key = filehandler->getKeyFromInFile();
		// Encrypt and save the key to the outfile
		filehandler->saveKeyToOutFile(key);
	}
	catch (std::exception &e)
	{
		std::cerr << FMT_ERROR " " << e.what() << std::endl;
		exit(1);
	}
}

// Generate the TOTP key from the given secret key (-k)
void generateTOTPKey(FileHandler *filehandler)
{
	std::string TOTPKey;
	try
	{
		// Retrieve and decrypt the saved key from the file
		const std::string key = filehandler->getKeyFromInFile();
		// Generate the TOTP code
		TOTPKey = TOTPGenerator::generateTOTPHmacSha1(key);
	}
	catch (std::exception &e)
	{
		std::cerr << FMT_ERROR " " << e.what() << std::endl;
		exit(1);
	}
	std::cout << "\n" FMT_DONE " Generated TOTP key: " << std::endl;
	std::cout << TOTPKey << std::endl;
}

int main(int argc, char *argv[])
{
	FileHandler fileHandler;

	try
	{ // Parse the given arguments
		parseArgv(argc, argv, &fileHandler);
	} catch (std::exception &e)
	{
		std::cerr << FMT_ERROR " Invalid argument: " << e.what() << std::endl;
		return 1;
	}

	// If we are in '-g' mode, we will encrypt and save the key
	if (fileHandler.getMode() == OTP_MODE_SAVE_KEY)
		saveKeyToOutFile(&fileHandler);
	else
	{ // If we are in '-k' mode, we will retrieve that key and produce a TOTP code
		generateTOTPKey(&fileHandler);
	}

	return 0;
}
