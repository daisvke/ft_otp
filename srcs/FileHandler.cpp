#include "FileHandler.hpp"

FileHandler::FileHandler(): _fileName(), _mode() {}

void FileHandler::openFile()
{
  // Open a file for reading and writing
	// Create a file stream object for both input and output
    _file.open(_fileName);
    // Check if the file is open
    if (!_file) throw OpenFileException();
    else std::cout << "INSIDE" << std::endl;
}

FileHandler::~FileHandler() { _file.close(); }

void  FileHandler::setFilename(const char *fileName) { _fileName = fileName; }
void  FileHandler::setMode(otp_e_modes mode) { _mode = mode; }

otp_e_modes  FileHandler::getMode(void) const { return _mode; }

void  FileHandler::saveKeyInFile()
{
  
}

void	FileHandler::searchKeyInFile()
{

}