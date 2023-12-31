#include "FileHandler.hpp"

FileHandler::FileHandler(): _fileName(), _mode() {}

FileHandler::~FileHandler() {}

void 	FileHandler::setFilename(const char *fileName) { _fileName = fileName; }
void 	FileHandler::setMode(otp_e_modes mode) { _mode = mode; }

otp_e_modes 	FileHandler::getMode(void) const { return _mode; }

std::string 	FileHandler::getKeyFromInFile()
{
	// Create a file stream object for reading from the file
    std::ifstream	file(_fileName);
	std::cout << _fileName << "=> file" << std::endl;;
    // Check if the file is open
    if (!file) throw OpenFileException();
	// Read the content of the file
    std::string key(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);
    file.close();

	std::string	recovered;
	if (_mode == OTP_MODE_GEN_PWD) {
		try { recovered = CryptoHandler::decryptAES(key); }
		catch(std::exception &e)
		{
			std::cerr << "Failed to decrypt key from file." << std::endl;
			return 0;
		}
	} else recovered = key;

	if (CryptoHandler::isValidHexStr(recovered))
		return recovered;
	else throw CryptoHandler::InvalidHexException();
}

void 	FileHandler::saveKeyToOutFile(std::string key)
{
	// Create a file stream object for writing in the file
    std::ofstream	file(OTP_OUTFILENAME);
    // Check if the file is open
    if (!file) throw OpenFileException();
	// generate AES encrypted data to write to the file
	std::string		cipher;
	try {
		cipher = CryptoHandler::encryptAES(key);
		// CryptoHandler::decryptAES(cipher);

	} catch (std::exception &e) {
		std::cerr << "Error: while encrypting: " << e.what() << std::endl;
	}
	std::cout << " \nKEY encrypted: " << cipher << std::endl;
	// Write the encrypted data in the file
	file << cipher;
    file.close();
}