# =====================================================
# This is the Makefile for the Command Line Interface
#======================================================


# ==========================
# Build Configuration
# ==========================

NAME				=	ft_otp
CXX					=	g++
CXXFLAGS			=	-g -std=c++11 -Wall -Wextra -Werror
LDFLAGS				=	-lcryptopp -lqrencode -lpng
RM					=	rm -rf

# Secret key files
HEX_KEY_FILE		=	../keys/key.hex
BASE32_KEY_FILE		=	../keys/key.base32
BAD_KEY_FILE		=	../keys/key.base32hex
ENCRYPTED_KEY_FILE	=	ft_otp.key
KEY_QRCODE_FILE		=	qrcode.png

 
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
# Source & Header Files
# ==========================

# 'core' folder is shared with GUI
INCS		=	$(wildcard *.hpp) $(wildcard ../core/*.hpp)
SRCS		=	$(wildcard *.cpp) $(wildcard ../core/*.cpp)


# ==========================
# Object Files
# ==========================

OBJS_DIR		=	objs/
OBJS_DIR_CORE	=	core/
OBJS			=	$(SRCS:%.cpp=$(OBJS_DIR)%.o)


# ==========================
# Building
# ==========================

.PHONY: all clean fclean re hex b32 bad tests

all: $(NAME)

# Main target
$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Object files
$(OBJS_DIR)%.o: %.cpp $(INCS)
	@mkdir -p $(dir $@)
	@echo "$(CXX) $(INCS) $(CXXFLAGS) -c $<"; \
		$(CXX) $(CXXFLAGS) -c $< -o $@ || \
		{	echo "\n$(ERROR) Compilation failed for $<."; \
			echo "$(INFO) Possible reason: libraries are not installed or correctly linked."; \
			echo "$(INFO)"; \
			echo "$(INFO) To install the libraries on Ubuntu/Debian-based systems:"; \
			echo "$(INFO)"; \
			echo "$(INFO) First, check the available Crypto++ packages for your system:"; \
			echo "$(INFO) \tapt search libcrypto++"; \
			echo "$(INFO)"; \
			echo "$(INFO) Replace 'X' by the version you've found during the previous step:"; \
			echo "$(INFO) \tsudo apt install libcrypto++X libcrypto++-dev libcrypto++-utils libcrypto++-doc libqr4encode-dev libpng-dev libssl-dev -y"; \
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
	./ft_otp -gq $(1) -v; \
	GEN_STATUS=$$?; \
	if [ $$GEN_STATUS -eq 0 ]; then \
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
	GENERATED_TOTP=$$(./ft_otp -k $(ENCRYPTED_KEY_FILE)); \
	EXPECTED_TOTP=$$(oathtool --totp $(3) $(shell cat $(1))); \
	EXP_STATUS=$$?; \
	echo "--------------------------------------------------"; \
	echo "ft_otp status: $$GEN_STATUS, oathtool status: $$EXP_STATUS"; \
	if [ $$GEN_STATUS -eq 0 ] && [ $$EXP_STATUS -eq 0 ]; then \
		echo "Generated TOTP:\t$$GENERATED_TOTP"; \
		echo "Expected TOTP:\t$$EXPECTED_TOTP"; \
		if [ "$$GENERATED_TOTP" = "$$EXPECTED_TOTP" ]; then \
			echo "$(DONE) TOTPs match"; \
		else \
			echo "$(ERROR) TOTPs do not match."; \
		fi; \
	else \
		echo "$(ERROR) Command execution failed."; \
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
	@$(MAKE) bad
	@echo "$(INFO) Tests completed."

# Targets to call a pseudo function for a specific key
hex: all
	$(call process_test_key, $(HEX_KEY_FILE), "Hex")

b32: all
	$(call process_test_key, $(BASE32_KEY_FILE), "Base32", "-b")

bad: all
	$(call process_test_key, $(BAD_KEY_FILE), "bad")


# ==========================
# Cleaning
# ==========================

clean:
	$(RM) $(OBJS_DIR_CORE) $(OBJS_DIR) $(KEY_QRCODE_FILE)

fclean: clean
	$(RM) $(NAME)

re: fclean all
