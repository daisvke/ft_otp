#ifndef TOTPGENERATOR_HPP
#define TOTPGENERATOR_HPP

#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <iomanip>

// Crypto++
#include <../cryptopp/cryptlib.h>
#include <../cryptopp/modes.h>
#include <../cryptopp/osrng.h>
#include <../cryptopp/hex.h>
#include <../cryptopp/files.h>
#include <../cryptopp/hmac.h>
#include <../cryptopp/sha.h>
#include <../cryptopp/filters.h>
#include <../cryptopp/base32.h>

// For TOTP HMAC-SHA1 hash
#include <iomanip>
#include <chrono>
#include <sstream>

#include <openssl/hmac.h>

// The provided key has to have at least 64 characters
#define OTP_MIN_KEY_STRENGTH 64
// Key used for outfile (where the key is stored) encryption
#define OTP_AES_KEY "4a1c4b646cfd6740d738330d30019a62"
#define OTP_AES_KEY_LEN 32
// Initialization vector used for the same AES encryption
#define OTP_AES_IV "6FDBFFCAEF982021F4A02312ADC82CF3"
#define OTP_AES_IV_LEN 32
// Time step used in TOTP
#define OTP_TOTP_TIME 30
// Length of the TOTP code
#define OTP_TOTP_CODE_DIGIT 6

// Values to identify the given key (secret) format.
// These values will be used in bitwise operations.
enum KeyFormat
{
	OTP_KEYFORMAT_HEX = 1,
	OTP_KEYFORMAT_BASE32 = 2,
	OTP_KEYFORMAT_DEFAULT = 3,
};

using std::string;

class TOTPGenerator
{
public:
	TOTPGenerator();
	~TOTPGenerator();

	uint8_t static isValidHexOrBase32(const std::string &str);
	std::string static encryptAES(std::string plain);
	std::string static decryptAES(std::string &cipher);
	// SecByteBlock static	DecodeKey(const std::string& key);
	std::string static generateTOTPHmacSha1(
		const std::string &key, uint64_t timeStep = OTP_TOTP_TIME, int digits = OTP_TOTP_CODE_DIGIT);

	class InvalidKeyFormatException : public std::exception
	{
	public:
		InvalidKeyFormatException() throw() {}
		const char *what() const throw()
		{
			return "\033[31mThe given string is not a hexadecimal key "
				   "or a Base32 key of at least 64 characters.";
		}
		~InvalidKeyFormatException() throw() {}
	};
};

#endif