# ==========================
# Build Configuration
# ==========================

NAME				=	ft_otp
CXX					=	clang++
CXXFLAGS			=	-std=c++11 -Wall -Wextra -Werror
INCS_DIR			=	incs/
INCS				=	-I incs/
LDFLAGS				=	-lcryptopp
INCS_FILES			=	$(wildcard $(INCS_DIR)*.hpp)
RM					=	rm -rf

# Secret key files
HEX_KEY_FILE		=	keys/key.hex
BASE32_KEY_FILE		=	keys/key.base32
BAD_KEY_FILE		=	keys/key.base32hex
ENCRYPTED_KEY_FILE	=	ft_otp.key


# ==========================
# ANSI Escape Codes
# ==========================

# ANSI escape codes for stylized output
RESET 		= \033[0m
GREEN		= \033[32m
YELLOW		= \033[33m
RED			= \033[31m

# Logs levels
INFO 		= $(YELLOW)[INFO]$(RESET)
ERROR		= $(RED)[ERROR]$(RESET)
DONE		= $(GREEN)[DONE]$(RESET)


# ==========================
# Source Files
# ==========================

SRCS_DIR			=	srcs/
SRCS_FILES			=	$(notdir $(wildcard $(SRCS_DIR)*.cpp))
SRCS				=	$(addprefix $(SRCS_DIR), $(SRCS_FILES))


# ==========================
# Object Files
# ==========================

OBJS_DIR			=	objs/
OBJS_FILES			=	$(SRCS_FILES:.cpp=.o)
OBJS				=	$(addprefix $(OBJS_DIR), $(OBJS_FILES))


# ==========================
# Building
# ==========================

.PHONY: all clean fclean re hex b32 err tests

all: $(NAME)

# Main target
$(NAME): $(OBJS) $(INCS_FILES)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Object files
$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp $(INCS_DIR)
	@mkdir -p $(OBJS_DIR)
	@echo "$(CXX) $(INCS) $(CXXFLAGS) -o $@ -c $<"; \
		$(CXX) $(INCS) $(CXXFLAGS) -o $@ -c $< || \
		{	echo "\n$(ERROR) Compilation failed for $<."; \
			echo "$(INFO) Possible reason: Crypto++ library is not installed or correctly linked."; \
			echo "$(INFO)"; \
			echo "$(INFO) To install the library on Ubuntu/Debian-based systems:"; \
			echo "$(INFO)"; \
			echo "$(INFO) First, check the available packages for your system:"; \
			echo "$(INFO) \t\tapt search libcrypto++"; \
			echo "$(INFO) Or, on Termux:"; \
			echo "$(INFO) \t\tpkg search cryptopp"; \
			echo "$(INFO)"; \
			echo "$(INFO) Replace 'X' by the version you've found during the previous step:"; \
			echo "$(INFO) \t\tsudo apt install libcrypto++X libcrypto++-dev libcrypto++-utils libcrypto++-doc"; \
			echo "$(INFO) Or, on Termux:"; \
			echo "$(INFO) \t\tpkg install cryptopp\n"; \
			exit 1;}


# ==========================
# Testing
# ==========================

# A "pseudo-function" to process each type of key file during tests
# Param1: the path to the original secret key
# Param2: key format to display with echo()
# Param3: option for oathtool --totp ('-b' for base32)

process_test_key = \
	@echo "$(INFO) Testing with a $(2) key..."; \
	echo "$(INFO) Generating and saving the encrypted key to the external file 'ft_otp.key'..."; \
	echo "$(INFO) Running ./$(NAME) -g with $(1) file...\n"; \
	./ft_otp -g $(1) -v; \
	if [ $$? -eq 0 ]; then \
		echo "$(DONE)"; \
		echo "--------------------------------------------------"; \
		echo "$(INFO) Decoding the encrypted key and generating a TOTP code from it..."; \
		echo "$(INFO) Running ./$(NAME) -k with $(ENCRYPTED_KEY_FILE) file...\n"; \
		./ft_otp $(ENCRYPTED_KEY_FILE) -k -v; \
		if [ $$? -eq 0 ]; then \
			echo "$(DONE)"; \
		else \
			echo "$(ERROR)"; \
		fi; \
	else \
		echo "$(ERROR)"; \
	fi; \
	echo "--------------------------------------------------"; \
	echo "$(INFO) Comparing our TOTP code to the one delivered by 'oathtool'..."; \
	echo "$(INFO) Running oathtool --totp -v with $(1) file...\n"; \
	oathtool --totp $(3) $(shell cat $(1)) -v; \
	if [ $$? -eq 0 ]; then \
		echo "$(DONE)"; \
	else \
		echo "$(ERROR)"; \
	fi

# Test all keys
tests:
	@echo "$(INFO) Starting tests..."
	@echo "\n"
	@echo "$(INFO) ##################################################"
	@echo "$(INFO) #                    H  E  X                     #"
	@echo "$(INFO) ##################################################"
	@$(MAKE) hex
	@echo "\n\n"
	@echo "$(INFO) ##################################################"
	@echo "$(INFO) #                  B A S E  3 2                  #"
	@echo "$(INFO) ##################################################"
	@$(MAKE) b32
	@echo "\n\n"
	@echo "$(INFO) ##################################################"
	@echo "$(INFO) #                  B A D   K E Y                 #"
	@echo "$(INFO) ##################################################"
	@$(MAKE) err
	@echo "$(INFO) Tests completed."

# Targets to call a pseudo function for a specific key
hex: all
	$(call process_test_key, $(HEX_KEY_FILE), "Hex")

b32: all
	$(call process_test_key, $(BASE32_KEY_FILE), "Base32", "-b")

err: all
	$(call process_test_key, $(BAD_KEY_FILE), "bad")


# ==========================
# Cleaning
# ==========================

clean:
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all
