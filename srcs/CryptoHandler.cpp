#include "CryptoHandler.hpp"
using namespace CryptoPP;

CryptoHandler::CryptoHandler() {}
CryptoHandler::~CryptoHandler() {}


#include <stdio.h>
#include <string.h>
#include <openssl/hmac.h>
#include <string>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <cryptopp/base32.h>
#include <algorithm>
#include <iostream>
#include <iomanip>

using std::string;

string CryptoHandler::decodeBase32(string token) {
    string secret;
    int lookup[256];
    const CryptoPP::byte ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    CryptoPP::Base32Decoder::InitializeDecodingLookupArray(lookup, ALPHABET, 32, true);

    CryptoPP::Base32Decoder decoder;
    CryptoPP::AlgorithmParameters params = CryptoPP::MakeParameters(CryptoPP::Name::DecodingLookupArray(),(const int *)lookup);
    decoder.IsolatedInitialize(params);
    decoder.Put((CryptoPP::byte*)token.data(), token.length());
    decoder.MessageEnd();

    CryptoPP::word64 size = decoder.MaxRetrievable();
    if(size && size <= SIZE_MAX)
    {
        secret.resize(size);
        decoder.Get((CryptoPP::byte*)secret.data(), secret.length());
    }
    return secret;
}

bool CryptoHandler::isValidHexStr(const std::string str)
{
    return !str.empty() &&
		// Check if all characters are hex digits
		std::all_of(str.begin(), str.end(), ::isxdigit) &&
		// Check if the key has the minimum expected amount of characters
		str.size() >= OTP_MIN_KEY_STRENGTH;
}

SecByteBlock	convertStringToBytes(const char *str, int size)
{
    SecByteBlock	key(
		reinterpret_cast<const CryptoPP::byte*>(str), size
	);
	return key;
}

std::string	CryptoHandler::encryptAES(std::string plain)
{
	// Convert macro key and initialization vector to the approriate data type
    SecByteBlock	key = convertStringToBytes(OTP_AES_KEY, OTP_AES_KEY_LEN);
    SecByteBlock	iv = convertStringToBytes(OTP_AES_IV, OTP_AES_IV_LEN);
    HexEncoder		encoder(new FileSink(std::cout));
    std::string		cipher;

    std::cout << "plain text: " << plain << std::endl;

    try
    {
        CBC_Mode< AES >::Encryption e;
        e.SetKeyWithIV(key, key.size(), iv);

        StringSource s(plain, true, 
            new StreamTransformationFilter(e,
                new StringSink(cipher)
            ) // StreamTransformationFilter
        ); // StringSource
    } catch(const Exception& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
	std::cout << "CIPHER: " << std::hex << cipher << std::endl;

    std::cout << "key: ";
    encoder.Put(key, key.size());
    encoder.MessageEnd();
    std::cout << std::endl;

    std::cout << "iv: ";
    encoder.Put(iv, iv.size());
    encoder.MessageEnd();
    std::cout << std::endl;

    std::cout << "cipher text: ";
    encoder.Put((const byte*)&cipher[0], cipher.size());
    encoder.MessageEnd();
    std::cout << std::endl;

	return cipher;
}

// Function to perform AES decryption
std::string CryptoHandler::decryptAES(std::string &cipher)
{
	// Convert macro key and initialization vector to the approriate data type
    SecByteBlock	key = convertStringToBytes(OTP_AES_KEY, OTP_AES_KEY_LEN);
    SecByteBlock	iv = convertStringToBytes(OTP_AES_IV, OTP_AES_IV_LEN);
    std::string		recovered;

    try
    {
        CBC_Mode< AES >::Decryption d;
        d.SetKeyWithIV(key, key.size(), iv);

        StringSource s(cipher, true, 
            new StreamTransformationFilter(d,
                new StringSink(recovered)
            ) // StreamTransformationFilter
        ); // StringSource

        std::cout << "recovered text: " << recovered << std::endl;
    } catch(const Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
	return recovered;
}

std::string hexToBase32(const std::string& hexKey) {
    // Define the Base32 encoding table
    const char base32Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

    // Convert hex to binary
    std::string binaryKey;
    for (char hexChar : hexKey) {
        if (hexChar == '\0') {
            // Handle null character as any other character
            binaryKey.push_back('0');
            binaryKey.push_back('0');
            binaryKey.push_back('0');
            binaryKey.push_back('0');
        } else {
            int hexValue;
            std::sscanf(&hexChar, "%1X", &hexValue);  // Convert hex char to integer
            for (int i = 3; i >= 0; --i) {
                binaryKey.push_back(((hexValue >> i) & 1) ? '1' : '0');
            }
        }
    }

    // Add padding to make the length a multiple of 40 (8 characters)
    while (binaryKey.length() % 40 != 0) {
        binaryKey.push_back('0');
    }

    // Convert binary to Base32
    std::string base32Key;
    for (size_t i = 0; i < binaryKey.length(); i += 5) {
        int index = 0;
        for (size_t j = 0; j < 5; ++j) {
            index = (index << 1) + (binaryKey[i + j] - '0');
        }
        base32Key.push_back(base32Chars[index]);
    }
    for (size_t i = base32Key.length() - 1; i > 0; --i) {
        if (base32Key[i] == 'A') base32Key[i] = '=';
    }

    return base32Key;
}

std::string	CryptoHandler::generateTOTPHmacSha1(
	const std::string &hexKey, uint64_t timeStep)
{
    // Calculate the current time in seconds
	int64_t currentTime =
		std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()
			.time_since_epoch()).count();

	// Calculate the counter based on the time step
	uint64_t counter = currentTime / timeStep;
	std::cout << "counter: " << counter << std::endl;

	// Convert the counter to bytes (big-endian)
	CryptoPP::SecByteBlock counterBytes(8);
	for (int i = 7; i >= 0; --i) {
		counterBytes[i] = static_cast<byte>(counter & 0xFF);
		counter >>= 8;
	}

	std::string	base32Key = hexToBase32(hexKey);
	std::cout << "Base32 key: " << base32Key << std::endl;

	// Use the provided hex-encoded secret key
	CryptoPP::SecByteBlock hmacKey;
	CryptoPP::Base32Encoder base32Encoder(new CryptoPP::ArraySink(hmacKey, hmacKey.size()));
	base32Encoder.Put(reinterpret_cast<const byte*>(hexKey.data()), hexKey.size());
	base32Encoder.MessageEnd();

    // // Hex decoding
	// CryptoPP::SecByteBlock hmacKey;
    // CryptoPP::StringSource(hexKey, true,
    //     new CryptoPP::HexDecoder(
    //         new CryptoPP::ArraySink(hmacKey, hmacKey.size())
    //     )
    // );

	// Calculate the HMAC-SHA1
	CryptoPP::HMAC<CryptoPP::SHA1> hmac(hmacKey, hmacKey.size());
	std::string otp(20, '\0');  // Allocate 20 characters for HMAC result

	CryptoPP::StringSource(counterBytes, counterBytes.size(), true,
		new CryptoPP::HashFilter(
			hmac, new CryptoPP::ArraySink(reinterpret_cast<byte*>(&otp[0]), otp.size())
		)
	);

	// Extract the lower 31 bits as an integer
	int offset = otp[19] & 0xf;
	int binCode = (otp[offset] & 0x7f) << 24
		| (otp[offset + 1] & 0xff) << 16
		| (otp[offset + 2] & 0xff) << 8
		| (otp[offset + 3] & 0xff);

	// HOTP = binCode modulo 10^6
	binCode %= 1000000;

	// Convert to string with leading zeros if necessary
	std::ostringstream oss;
	oss << std::setw(6) << std::setfill('0') << binCode;

	return oss.str();
}