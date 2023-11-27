#include "CryptoHandler.hpp"
using namespace CryptoPP;

CryptoHandler::CryptoHandler() {}
CryptoHandler::~CryptoHandler() {}

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
    }
    catch(const Exception& e) {
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
    }
    catch(const Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}