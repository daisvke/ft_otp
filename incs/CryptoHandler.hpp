#ifndef CRYPTOHANDLER_HPP
# define CRYPTOHANDLER_HPP

# include <iostream>
# include <algorithm>
# include <cctype>
# include <string>

// The key has to have at least 64 characters
# define OTP_MIN_KEY_STRENGTH	64

class	CryptoHandler
{
	public:
		CryptoHandler();
		~CryptoHandler();

		bool	static isValidHexStr(const std::string& str);
};

#endif