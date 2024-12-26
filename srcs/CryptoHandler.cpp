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

bool CryptoHandler::isValidHexStr(const std::string str)
{
    return !str.empty() &&
		// Check if all characters are hex digits
		std::all_of(str.begin(), str.end(), ::isxdigit) &&
		// Check if the key has the minimum expected amount of characters
		str.size() >= OTP_MIN_KEY_STRENGTH;
}

// bool isValidHexBase32(const std::string &str) {
//     bool            isHex = true, isBase32 = true;

//     // Check the format of the key
//     for (char c : str) {
//         // Check for valid hex characters
//         if (!std::isxdigit(c)) isHex = false;
//         // Check for valid Base32 characters
//         if (!std::isalnum(c)
//                 || (std::toupper(c) > '7' && std::toupper(c) < 'A')
//             )
//             isBase32 = false;
//     }

    
// }

static SecByteBlock	convertStringToBytes(const char *str, int size)
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

        StringSource s(
            cipher,
            true, 
            new StreamTransformationFilter(
                d,
                new StringSink(recovered)
            )
        );

        std::cout << "Hex secret: " << recovered << std::endl;
    } catch(const Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
	return recovered;
}

/**
 * @brief A function to detect and decode the key (Base32 or Hex)
 *
 * It will convert a human-readable string representation of data
 * back into its raw binary form.
 * 
 * @param Hex or Base32 string
 * 
 * @return
 *  In case the given string is not a Hex/Base32 key,
 *  an 'invalid_argument' exception is thrown.
 */
SecByteBlock DecodeKey(const std::string& key) {
    SecByteBlock    decodedKey;
    bool            isHex = true, isBase32 = true;

    // Check the format of the key
    for (char c : key) {
        // Check for valid hex characters
        if (!std::isxdigit(c)) isHex = false;
        // Check for valid Base32 characters
        if (!std::isalnum(c)
                || (std::toupper(c) > '7' && std::toupper(c) < 'A')
            )
            isBase32 = false;
    }

    // Decode the key based on its format
    if (isHex) {
        HexDecoder  decoder;
        decoder.Put((byte*)key.data(), key.size());
        decoder.MessageEnd();
        size_t size = decoder.MaxRetrievable();
        decodedKey.resize(size);
        decoder.Get(decodedKey, size);
    } else if (isBase32) {
        Base32Decoder decoder;
        decoder.Put((byte*)key.data(), key.size());
        decoder.MessageEnd();
        size_t size = decoder.MaxRetrievable();
        decodedKey.resize(size);
        decoder.Get(decodedKey, size);
    } else {
        throw std::invalid_argument("Key must be in Base32 or Hex format.");
    }

    return decodedKey;
}

// Convert a 64-bit counter to big-endian format
static void ConvertToBigEndian(uint64_t counter, uint8_t *outputBuffer) {
    for (int i = 7; i >= 0; --i) {
        outputBuffer[i] = counter & 0xFF;   // Extract the least significant byte
        counter >>= 8;                      // Shift to the next byte
    }
}

/**
 * @brief Check if the system is little-endian
 * 
 * If little-endian, the least significant byte (LSB) of a
 * multi-byte value is stored at the lowest memory address.
 * 
 * Memory layout:
 * Little-endian: [01 00]  -> testBytes[0] == 0x01
 * Big-endian:    [00 01]  -> testBytes[0] == 0x00
 */
static bool IsLittleEndian() {
    uint16_t    testValue = 0x1;
    uint8_t     *testBytes = reinterpret_cast<uint8_t*>(&testValue);
    return testBytes[0] == 0x1; // If the least significant byte is first, it's little-endian.
}

static void ConvertToBigEndianIfNeeded(uint64_t counter, uint8_t* outputBuffer) {
    if (IsLittleEndian()) {
        ConvertToBigEndian(counter, outputBuffer);
    } else {
        std::memcpy(outputBuffer, &counter, 8); // Already big-endian, just copy
    }
}

static CryptoPP::SecByteBlock computeCounter(uint64_t timeStep) {
    std::cout << "Step size (seconds): " << timeStep << std::endl;

    // Calculate the current time in seconds
	int64_t currentTime =
		std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now()
			    .time_since_epoch()
            ).count();

    std::cout << "Current time: " << currentTime << std::endl;

	uint64_t                counter = currentTime / timeStep;
    CryptoPP::SecByteBlock  counterByteArray(8);

    // Print the counter both in uppercase Hex and decimal formats
	std::cout << "Counter: 0X" << std::uppercase << std::hex << counter
        << " (" << std::dec << counter << ")" << std::endl;

    // Get a raw bytes representation of the counter (convert if needed)
    ConvertToBigEndianIfNeeded(counter, counterByteArray);

    return counterByteArray;
}

std::string	CryptoHandler::generateTOTPHmacSha1(
	const std::string &hexKey, uint64_t timeStep, int digits)
{
    std::string otpString = ""; // The TOTP code to return

    try {
        // 'hexKey' is the shared secret between client and server;
        // each HOTP generator has a different and unique secret.
        CryptoPP::SecByteBlock	decodedKey = DecodeKey(hexKey);

        /*
        * Generate the 'counter' needed by HMAC. 
        *	'counter' is an 8-byte counter value, the moving factor.
        *
        *	This counter MUST be synchronized between the HOTP generator)
        *	(client) and the HOTP validator (server).
        */

        CryptoPP::SecByteBlock  counterByteArray(8);
        const size_t            counterByteArraySize = 8;
        byte                    hmacDigest[CryptoPP::HMAC<CryptoPP::SHA1>::DIGESTSIZE];

        counterByteArray = computeCounter(timeStep);


        // Create an HMAC (Hash-based Message Authentication Code) object with SHA-1,
        // as defined in RFC 2104 [BCK2].
        std::cout << "TOTP mode: HMAC-SHA1" << std::endl;
        CryptoPP::HMAC<CryptoPP::SHA1> hmac(
			decodedKey, decodedKey.size()
			);

        /*
         * We compute the HMAC digest:
         *
         * An HMAC digest refers to the output generated by the HMAC process.
         * It is a byte array like here, or a fixed-size string, that serves as a
         * unique representation of the input data (message) combined with a secret key. 
         *
         * 'CalculateDigest' can be replaced by these two methods below used together:
         *  hmac.Update(counterByteArray, counterByteArraySize);
	     *  hmac.Final(hmacDigest);
         */
        hmac.CalculateDigest(hmacDigest, counterByteArray, counterByteArraySize);

        // Print the resulted HMAC digest
        std::cout << "HMAC-SHA1: ";
        for (size_t i = 0; i < sizeof(hmacDigest); ++i)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(hmacDigest[i]);
        }
        std::cout << std::endl;

        /*
            As the output of the HMAC-SHA-1 calculation is 160 bits,
            we must truncate this value to something that can be easily
            entered by a user.
        */
        // Extract the lower 31 bits as an integer
        int offset = hmacDigest[SHA1::DIGESTSIZE - 1] & 0x0F;
        uint32_t binaryCode = (hmacDigest[offset] & 0x7F) << 24 |
                            (hmacDigest[offset + 1] & 0xFF) << 16 |
                            (hmacDigest[offset + 2] & 0xFF) << 8 |
                            (hmacDigest[offset + 3] & 0xFF);

        /*
         * Compute TOTP code:
         *
         * If digit = 10⁶, this line below equals to:
         *  binaryCode %= 1000000;
         * 
         * This is to ensure that the resulting code is a fixed length,
         * specifically a 6-digit number.
         * 
         * The modulo operation also adds a layer of obfuscation.
         * An attacker who only sees the TOTP code (the 6-digit output) does not have
         * direct access to the original HMAC value.
         */
        uint32_t otp = binaryCode % static_cast<uint32_t>(std::pow(10, digits));
        
        // Format OTP as zero-padded string
        otpString = std::to_string(otp);
        while (otpString.size() < static_cast<size_t>(digits)) {
            otpString = "0" + otpString;
        }

    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Crypto++ exception: " << e.what() << std::endl;
    }

    return otpString;
}
