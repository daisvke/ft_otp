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