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
# include "qrgenerator.hpp"

# define OTP_PROJECT_NAME	"ft_otp"
# define OTP_QRCODE_FILE	"qrcode"	// Name of the PNG outfile for the QR code
# define OTP_QRCODE_SCALE	5			// The scale of the QR code's PNG image (if too
										// small it will be difficult to read it)

void	generateQRCode(const std::string& totpURI, const std::string& filename);
QRcode *generateQRCodeFromURI(const std::string secret, bool verbose);
void	generateQRcodePNGFromSecret(const std::string secret, bool verbose);

#endif
