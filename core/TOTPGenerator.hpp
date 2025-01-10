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
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/base32.h>

// For TOTP HMAC-SHA1 hash
#include <iomanip>
#include <chrono>
#include <openssl/hmac.h>

#include "ascii_format.hpp"

// Key used for outfile (where the key is stored) encryption
# define OTP_AES_KEY		"4a1c4b646cfd6740d738330d30019a62"
// Initialization vector used for the same AES encryption
# define OTP_AES_IV			"6FDBFFCAEF982021F4A02312ADC82CF3"

enum OTP
{
	OTP_MIN_KEY_STRENGTH	= 64,	// Minimum length of the key
	OTP_AES_KEY_LEN			= 32,
	OTP_AES_IV_LEN			= 32,
	OTP_TOTP_TIME			= 30,	// Time step used in TOTP
	OTP_TOTP_CODE_DIGIT		= 6		// Length of the TOTP code
};

// Values to identify the given key (secret) format.
// These values will be used in bitwise operations.
enum KeyFormat
{
	OTP_KEYFORMAT_HEX		= 1,
	OTP_KEYFORMAT_BASE32	= 2,
	OTP_KEYFORMAT_DEFAULT	= 3
};

using std::string;

class TOTPGenerator
{
private:
	bool	_verbose;

public:
	TOTPGenerator(bool verbose);
	~TOTPGenerator();

	uint8_t						isValidHexOrBase32(const std::string &str);
	std::string 				encryptAES(std::string plain);
	std::string					decryptAES(std::string &cipher);
	std::string					generateTOTPHmacSha1(
		const std::string &key, uint64_t timeStep = OTP_TOTP_TIME, int digits = OTP_TOTP_CODE_DIGIT);
	CryptoPP::SecByteBlock		DecodeKey(const std::string &key);
	CryptoPP::SecByteBlock		computeCounter(uint64_t timeStep);
};

class TOTPException : public std::exception
{
public:
	TOTPException() throw() {}
	const char *what() const throw() {
		return "Failed to  create TOTP.";
	}
	~TOTPException() throw() {}
};

#endif