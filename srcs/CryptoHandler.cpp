#include "CryptoHandler.hpp"

CryptoHandler::CryptoHandler() {}
CryptoHandler::~CryptoHandler() {}

bool CryptoHandler::isValidHexStr(const std::string& str) {
    return !str.empty() &&
		// Check if all characters are hex digits
		std::all_of(str.begin(), str.end(), ::isxdigit) &&
		// Check if the key has the minimum expected amount of characters
		str.size() >= OTP_MIN_KEY_STRENGTH;
}