#ifndef QRGENERATOR_HPP
# define QRGENERATOR_HPP

# include <qrencode.h>
# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <cctype>
# include <png.h>
# include <cstdio>
# include <cstdlib>

# define OTP_QRCODE_FILE	"qrcode"	// Name of the PNG outfile for the QR code
# define OTP_QRCODE_SCALE_TERM	1		// The scale of the printed QR code on the terminal
# define OTP_QRCODE_SCALE_PNG	10		// The scale of the QR code's PNG image (if too
										// small it will be difficult to read it)

void	printQRCode(QRcode* &qrcode, const int scale = OTP_QRCODE_SCALE_TERM);
void	saveQRCodeAsPNG(QRcode* &qrcode, const char* filename, const int scale = OTP_QRCODE_SCALE_PNG);
void	generateQRCode(const std::string& totpURI, const std::string& filename);
QRcode *generateQRCodeFromURI(const std::string secret, bool verbose);
void	generateQRcodePNGFromSecret(const std::string secret, bool verbose);


// Exceptions

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
