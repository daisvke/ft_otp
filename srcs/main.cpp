#include "FileHandler.hpp"
#include "parse_args.hpp"

// Encrypt and save the key to an external file (-g)
void saveKeyToOutFile(FileHandler *fileHandler, bool verbose)
{
	try
	{
		fileHandler->setVerbose(verbose);
		// Get the original secret from the given file
		std::string key = fileHandler->getKeyFromInFile();
		// Encrypt and save the key to the outfile
		if (key.empty() == false)
			fileHandler->saveKeyToOutFile(key);
	}
	catch (std::exception &e)
	{
		std::cerr << FMT_ERROR " " << e.what() << std::endl;
		exit(1);
	}
}

// Generate the TOTP key from the given secret key (-k)
void generateTOTPKey(FileHandler *filehandler, bool verbose)
{
	std::string TOTPKey;
	try
	{
		// Retrieve and decrypt the saved key from the file
		const std::string	key = filehandler->getKeyFromInFile();
		// Generate the TOTP code
		TOTPGenerator		TOTPGenerator(verbose);
		TOTPKey = TOTPGenerator.generateTOTPHmacSha1(key);
	}
	catch (std::exception &e)
	{
		std::cerr << FMT_ERROR " " << e.what() << std::endl;
		exit(1);
	}
	if (verbose)
		std::cout << "\n" FMT_DONE " Generated TOTP key: " << std::endl;
	std::cout << TOTPKey << std::endl;
}

int main(int argc, char *argv[])
{
	FileHandler fileHandler;
	bool		verbose;

	try
	{ // Parse the given arguments
		parseArgv(argc, argv, &fileHandler, verbose);
	} catch (std::exception &e)
	{
		std::cerr << FMT_ERROR " Invalid argument: " << e.what() << std::endl;
		return 1;
	}

	// If we are in '-g' mode, we will encrypt and save the key
	if (fileHandler.getMode() == OTP_MODE_SAVE_KEY)
		saveKeyToOutFile(&fileHandler, verbose);
	else
	{ // If we are in '-k' mode, we will retrieve that key and produce a TOTP code
		generateTOTPKey(&fileHandler, verbose);
	}

	return 0;
}
