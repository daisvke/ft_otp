#ifndef FILEHANDLER_HPP
# define FILEHANDLER_HPP

# include <iostream>
# include <fstream>
# include <stdexcept>

# include "ascii_format.hpp"
# include "TOTPGenerator.hpp"
 
# define OTP_OUTFILENAME "ft_otp.key"

enum otp_e_modes
{
	OTP_MODE_SAVE_KEY	= 1,
	OTP_MODE_GEN_PWD	= 2,
	OTP_MODE_GEN_QR		= 4
};

class FileHandler
{
public:
	FileHandler();
	~FileHandler();

	// Setters
	void		setFilename(const char *fileName);
	void		setMode(uint8_t mode);
	void		setVerbose(bool verbose);

	// Getters
	uint8_t		getMode(void) const;

	// Save key in outfile
	std::string	getKeyFromInFile();
	void		saveKeyToOutFile(std::string key);

private:
	const char *_fileName;
	uint8_t		_mode;
	bool		_verbose;

	class InvalidKeyFormatException: public std::exception
	{
	public:
		InvalidKeyFormatException() throw() {}
		const char *what() const throw()
		{
			return "The given string is not a hexadecimal key "
				   "or a Base32 key of at least 64 characters.";
		}
		~InvalidKeyFormatException() throw() {}
	};

	class OpenFileException : public std::exception
	{
	public:
		OpenFileException() throw() {}
		const char *what() const throw() {
			return "Failed to open the file.";
		}
		~OpenFileException() throw() {}
	};
};

#endif
