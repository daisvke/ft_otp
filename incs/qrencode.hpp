#ifndef QRENCODE_HPP
# define QRENCODE_HPP

# include <qrencode.h>
# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <png.h>
# include <cstdio>
# include <cstdlib>

# include "ascii_format.hpp"

# define OTP_PROJECT_NAME	"ft_otp"
# define OTP_QRCODE_FILE	"qrcode"	// Name of the PNG outfile for the QR code
# define OTP_QRCODE_SCALE	5			// The scale of the QR code's PNG image (if too
										// small it will be difficult to read it)

void	generateQRCode(const std::string& totpURI, const std::string& filename);
void	generateQRcodePNGFromSecret(const std::string secret, bool verbose);

class QRCodeGenerationException: public std::exception
{
public:
	QRCodeGenerationException() throw() {}
	const char	*what() const throw() {
			return "Failed to generate the QR code.";
	}
	~QRCodeGenerationException() throw() {}
};

class OpenFileException : public std::exception
{
public:
	OpenFileException() throw() {}
	const char *what() const throw() {
		return "Failed to open the file for writing.";
	}
	~OpenFileException() throw() {}
};

class LibpngInitException : public std::exception
{
public:
	LibpngInitException() throw() {}
	const char *what() const throw() {
		return "Unable to initialize libpng.";
	}
	~LibpngInitException() throw() {}
};

class PNGCreationException : public std::exception
{
public:
	PNGCreationException() throw() {}
	const char *what() const throw() {
		return "Failed to create PNG image.";
	}
	~PNGCreationException() throw() {}
};


class PNGRowMemAllocationException : public std::exception
{
public:
	PNGRowMemAllocationException() throw() {}
	const char *what() const throw() {
		return "Unable to allocate memory for row.";
	}
	~PNGRowMemAllocationException() throw() {}
};

#endif