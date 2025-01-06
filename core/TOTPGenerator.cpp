#include "TOTPGenerator.hpp"
using namespace CryptoPP;

TOTPGenerator::TOTPGenerator(bool verbose): _verbose(verbose) {}
TOTPGenerator::~TOTPGenerator() {}

uint8_t TOTPGenerator::isValidHexOrBase32(const std::string &str)
{
    if (str.size() < OTP_MIN_KEY_STRENGTH)
        return 0;
    // By default Hex and Base32 are set to true
    uint8_t keyFormat = OTP_KEYFORMAT_DEFAULT;

    for (char c : str)
    {
        /*
         * Check for valid hex characters using bitwise operations.
         *
         * Bitwise operations are faster than other operations because they
         * operate directly on binary digits at the hardware level.
         *
         * Here, keyFormat & OTP_KEYFORMAT_HEX is checking if OTP_KEYFORMAT_HEX
         * flag is set in keyFormat.
         *  Ex.: keyFormat (00000001) & OTP_KEYFORMAT_HEX (00000001) = 00000001 (flag is set)
         *
         * On the other hand, keyFormat ^= OTP_KEYFORMAT_HEX will toggle the
         * OTP_KEYFORMAT_HEX flag in keyFormat.
         *  Ex.: keyFormat (00000001) ^⁼ OTP_KEYFORMAT_HEX (00000001)
         *      <=> keyFormat (00000001) = keyFormat ^ OTP_KEYFORMAT_HEX (00000001)
         *      = 00000000 (unset OTP_KEYFORMAT_HEX in keyFormat)
         *
         */
        if ((keyFormat & OTP_KEYFORMAT_HEX) && !std::isxdigit(c))
            keyFormat ^= OTP_KEYFORMAT_HEX;

        // Check for valid Base32 characters
        if ((keyFormat & OTP_KEYFORMAT_BASE32) &&
            c != '=' && (!std::isalnum(c) || (std::toupper(c) > '7' && std::toupper(c) < 'A')))
            keyFormat ^= OTP_KEYFORMAT_BASE32;
    }

    return keyFormat;
}

static SecByteBlock convertStringToBytes(const char *str, int size)
{
    SecByteBlock key(
        reinterpret_cast<const byte *>(str), size);
    return key;
}

std::string TOTPGenerator::encryptAES(std::string plain)
{
    // Convert macro key and initialization vector to the approriate data type
    SecByteBlock    key = convertStringToBytes(OTP_AES_KEY, OTP_AES_KEY_LEN);
    SecByteBlock    iv = convertStringToBytes(OTP_AES_IV, OTP_AES_IV_LEN);
    HexEncoder      encoder(new FileSink(std::cout));
    std::string     cipher;

    if (_verbose)
        std::cout << "Plain text: " << plain << std::endl;

    try
    {
        CBC_Mode<AES>::Encryption e;
        e.SetKeyWithIV(key, key.size(), iv);

        StringSource s(plain, true,
                       new StreamTransformationFilter(
                            e,
                            new StringSink(cipher)
                        ) // StreamTransformationFilter
        ); // StringSource
    }
    catch (const Exception &e)
    {
        std::cerr << FMT_ERROR << " " << e.what() << std::endl;
        exit(1);
    }

    if (_verbose) {
        std::cout << "Key: ";
        encoder.Put(key, key.size());
        encoder.MessageEnd();
        std::cout << std::endl;

        std::cout << "IV: ";
        encoder.Put(iv, iv.size());
        encoder.MessageEnd();
        std::cout << std::endl;

        std::cout << "Cipher text: ";
        encoder.Put((const byte *)&cipher[0], cipher.size());
        encoder.MessageEnd();
        std::cout << std::endl;
    }

    return cipher;
}

// Function to perform AES decryption
std::string TOTPGenerator::decryptAES(std::string &cipher)
{
    // Convert macro key and initialization vector to the approriate data type
    SecByteBlock    key = convertStringToBytes(OTP_AES_KEY, OTP_AES_KEY_LEN);
    SecByteBlock    iv = convertStringToBytes(OTP_AES_IV, OTP_AES_IV_LEN);
    std::string     recovered;

    try
    {
        CBC_Mode<AES>::Decryption d;
        d.SetKeyWithIV(key, key.size(), iv);

        StringSource s(
            cipher,
            true,
            new StreamTransformationFilter(
                d,
                new StringSink(recovered)));
    }
    catch (const Exception &e)
    {
        std::cerr << FMT_ERROR << " " << e.what() << std::endl;
        exit(1);
    }
    return recovered;
}

std::vector<uint8_t> decodeBase32RFC4648(const std::string &base32String)
{
    static const std::string    base32Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::vector<uint8_t>        decoded;
    uint32_t                    buffer = 0;
    int                         bitsLeft = 0;

    for (char c : base32String)
    {
        if (c == '=')   // Padding character
            break;      // Padding character means we have reached the end of the key

        size_t index = base32Alphabet.find(std::toupper(c));
        if (index == std::string::npos)
            throw std::invalid_argument("Invalid Base32 character");

        buffer = (buffer << 5) | index;
        bitsLeft += 5;

        if (bitsLeft >= 8)
        {
            decoded.push_back((buffer >> (bitsLeft - 8)) & 0xFF);
            bitsLeft -= 8;
        }
    }
    return decoded;
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
SecByteBlock TOTPGenerator::DecodeKey(const std::string &key)
{
    SecByteBlock decodedKey;
    // bool            isHex = true, isBase32 = true;

    uint8_t keyFormat = isValidHexOrBase32(key);
    // Decode the key based on its format
    if (keyFormat & OTP_KEYFORMAT_HEX)
    {
        HexDecoder  decoder;
        decoder.Put((byte *)key.data(), key.size());
        decoder.MessageEnd();
        size_t  size = decoder.MaxRetrievable();
        decodedKey.resize(size);
        decoder.Get(decodedKey, size);

        if (_verbose) {
            std::cout << "Hex Key: ";
            for (size_t i = 0; i < decodedKey.size(); ++i)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(decodedKey[i]);
            }
            std::cout << std::dec << std::endl;
        }

        return decodedKey;
    }
    else if (keyFormat & OTP_KEYFORMAT_BASE32)
    {
        std::vector<uint8_t> decodedByteVector = decodeBase32RFC4648(key);
        SecByteBlock decodedKey(decodedByteVector.data(), decodedByteVector.size());

        if (_verbose) {
            std::cout << "Base32 secret: " << key << std::endl;

            std::cout << "Decoded Base32 Key: ";
            for (size_t i = 0; i < decodedKey.size(); ++i)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(decodedKey[i]) << std::dec;
            }
            std::cout << std::endl;
        }
        return decodedKey;
    }
    else throw std::invalid_argument("Key must be in Base32 or Hex format.");
}

// Convert a 64-bit counter to big-endian format
static void ConvertToBigEndian(uint64_t counter, uint8_t *outputBuffer)
{
    for (int i = 7; i >= 0; --i)
    {
        outputBuffer[i] = counter & 0xFF; // Extract the least significant byte
        counter >>= 8;                    // Shift to the next byte
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
static bool IsLittleEndian()
{
    uint16_t testValue = 0x1;
    uint8_t *testBytes = reinterpret_cast<uint8_t *>(&testValue);
    return testBytes[0] == 0x1; // If the least significant byte is first, it's little-endian.
}

static void ConvertToBigEndianIfNeeded(uint64_t counter, uint8_t *outputBuffer)
{
    if (IsLittleEndian())
    {
        ConvertToBigEndian(counter, outputBuffer);
    }
    else
    {
        std::memcpy(outputBuffer, &counter, 8); // Already big-endian, just copy
    }
}

CryptoPP::SecByteBlock TOTPGenerator::computeCounter(uint64_t timeStep)
{
    // Calculate the current time in seconds
    int64_t currentTime =
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now()
                .time_since_epoch())
            .count();

    uint64_t counter = currentTime / timeStep;
    CryptoPP::SecByteBlock counterByteArray(8);

    // Print the counter both in uppercase Hex and decimal formats
    if (_verbose) {
        std::cout << "Step size (seconds): " << timeStep << std::endl;
        std::cout << "Current time: " << currentTime << std::endl;
        std::cout << "Counter: 0X" << std::uppercase << std::hex << counter
            << std::dec << " (" << counter << ")" << std::endl;
    }

    // Get a raw bytes representation of the counter (convert if needed)
    ConvertToBigEndianIfNeeded(counter, counterByteArray);

    return counterByteArray;
}

std::string TOTPGenerator::generateTOTPHmacSha1(
    const std::string &userKey, uint64_t timeStep, int digits)
{
    std::string otpString = ""; // The TOTP code to return

    try
    {
        // 'hexKey' is the shared secret between client and server;
        // each HOTP generator has a different and unique secret.
        CryptoPP::SecByteBlock decodedKey = DecodeKey(userKey);

        /*
         * Generate the 'counter' needed by HMAC.
         *	'counter' is an 8-byte counter value, the moving factor.
         *
         *	This counter MUST be synchronized between the HOTP generator)
         *	(client) and the HOTP validator (server).
         */

        CryptoPP::SecByteBlock counterByteArray(8);
        const size_t counterByteArraySize = 8;
        byte hmacDigest[CryptoPP::HMAC<CryptoPP::SHA1>::DIGESTSIZE];

        counterByteArray = computeCounter(timeStep);

        // Create an HMAC (Hash-based Message Authentication Code) object with SHA-1,
        // as defined in RFC 2104 [BCK2].
        CryptoPP::HMAC<CryptoPP::SHA1> hmac(
            decodedKey, decodedKey.size());

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
        if (_verbose) {
            std::cout << "TOTP mode: HMAC-SHA1" << std::endl;
            std::cout << "HMAC-SHA1: ";
            for (size_t i = 0; i < sizeof(hmacDigest); ++i)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0')
                        << static_cast<int>(hmacDigest[i]);
            }
            std::cout << std::dec << std::endl;
        }

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
        while (otpString.size() < static_cast<size_t>(digits))
        {
            otpString = "0" + otpString;
        }
    }
    catch (const CryptoPP::Exception &e)
    {
        std::cerr << "Crypto++ exception: " << e.what() << std::endl;
    }

    return otpString;
}
