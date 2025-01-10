#include "FileHandler.hpp"

FileHandler::FileHandler() : _fileName(), _mode(0), _verbose() {}

FileHandler::~FileHandler() {}

void FileHandler::setFilename(const char *fileName) { _fileName = fileName; }
void FileHandler::setMode(uint8_t mode) { _mode ^= mode; }
void FileHandler::setVerbose(bool verbose) { _verbose = verbose; }

uint8_t FileHandler::getMode(void) const { return _mode; }

/**
 * @brief Read the key to be encrypted from the given filename.
 * 	- In `-g` mode, it will read from the original key file.
 * 
 * 	- In `-k` mode, it will open the file that has been encrypted
 * 	  by this program during `-g` mode.
 * 	  Then, the key will be recovered to get back its orginal format
 * 	  which corresponds to the original key read during `-k` mode. 
 *
 * @return 
 *  - In `-g` mode, the read key.
 *  - In `-k` mode, the recovered key.
 */

bool isRegularFile(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

std::string FileHandler::getKeyFromInFile()
{
    if (!isRegularFile(_fileName)) {
		std::cerr << FMT_ERROR " Argument is not a regular file." << std::endl;
		exit(1);
	}

	// Create a file stream object for reading from the file
	std::ifstream file(_fileName);
	if (_verbose)
		std::cout	<< FMT_INFO " Opening secret key file '"
					<< _fileName << "'..." << std::endl;
	;
	// Check if the file is open
	if (!file)
		throw OpenFileException();
	// Read the content of the file
	std::string key(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	file.close();

	std::string		recovered;
	TOTPGenerator	TOTPGenerator(_verbose);
	if (_mode == OTP_MODE_GEN_PWD)
	{
		try {
			recovered = TOTPGenerator.decryptAES(key);
		} catch (std::exception &e) {
			std::cerr << FMT_ERROR "Failed to decrypt key from file." << std::endl;
			return nullptr;
		}
	}
	else recovered = key;

	if (TOTPGenerator.isValidHexOrBase32(recovered))
		return recovered;
	else
		throw InvalidKeyFormatException();
}

void FileHandler::saveKeyToOutFile(std::string key)
{
	// Create a file stream object for writing in the file
	std::ofstream file(OTP_OUTFILENAME);
	// Check if the file is open
	if (!file)
		throw OpenFileException();
	// generate AES encrypted data to write to the file
	std::string cipher;
	try
	{
		TOTPGenerator	TOTPGenerator(_verbose);
		cipher = TOTPGenerator.encryptAES(key);
	}
	catch (std::exception &e)
	{
		std::cerr << FMT_ERROR " while encrypting: " << e.what() << std::endl;
	}
	if (_verbose)
		std::cout << "\n" << FMT_DONE " Key encrypted and saved." << std::endl;
	// Write the encrypted data in the file
	file << cipher;
	file.close();
}
