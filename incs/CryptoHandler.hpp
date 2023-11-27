#ifndef CRYPTOHANDLER_HPP
# define CRYPTOHANDLER_HPP

# include <iostream>
# include <algorithm>
# include <cctype>
# include <string>

# include <cryptopp/cryptlib.h>
# include <cryptopp/modes.h>
# include <cryptopp/osrng.h>
# include <cryptopp/hex.h>
# include <cryptopp/rijndael.h>
# include <cryptopp/files.h>

// The provided key has to have at least 64 characters
# define OTP_MIN_KEY_STRENGTH	64
// Key used for outfile (where the key is stored) encryption
# define OTP_AES_KEY			"4a1c4b646cfd6740d738330d30019a62"
# define OTP_AES_KEY_LEN		32
// Initialization vector used for the same AES encryption
# define OTP_AES_IV				"6FDBFFCAEF982021F4A02312ADC82CF3"
# define OTP_AES_IV_LEN			32

class	CryptoHandler
{
	public:
		CryptoHandler();
		~CryptoHandler();

		bool static			isValidHexStr(const std::string	str);
		std::string	static	encryptAES(std::string plain);
		std::string	static	decryptAES(std::string &cipher);

	class InvalidHexException: public std::exception
	{
		public:
			InvalidHexException() throw() {}
			const char	*what() const throw() {
				return "\033[31mString is not a hexadecimal key " \
				"of at least 64 characters!"; }
			~InvalidHexException() throw() {}
	};
};

#endif