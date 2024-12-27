NAME				=	ft_otp
CXX					=	c++
CXXFLAGS			=	-std=c++11 -Wall -Wextra -Werror
INCS_DIR			=	incs/
INCS				=	-I incs/
LDFLAGS				=	-L./cryptopp -lcryptopp
INCS				+=	-I cryptopp/
CRYPTOPP_LIB		=	cryptopp/libcryptopp.a

ENCRYPTED_KEY_FILE	=	ft_otp.key

# ANSI escape codes for stylized output
RESET 		= \033[0m
GREEN		= \033[32m
YELLOW		= \033[33m
RED			= \033[31m

# Logs levels
INFO 		= $(YELLOW)[INFO]$(RESET)
ERROR		= $(RED)[ERROR]$(RESET)
DONE		= $(GREEN)[DONE]$(RESET)


#######################################
#				F I L E S			  #
#######################################

#		S O U R C E  F I L E S		  #

SRCS_DIR			=	srcs/
SRCS_FILES			=	$(notdir $(wildcard $(SRCS_DIR)*.cpp))
SRCS				=	$(addprefix $(SRCS_DIR), $(SRCS_FILES))


#			O B J .  F I L E S		  #

OBJS_DIR			=	objs/
OBJS_FILES			=	$(SRCS_FILES:.cpp=.o)
OBJS				=	$(addprefix $(OBJS_DIR), $(OBJS_FILES))


#######################################
#				R U L E S			  #
#######################################

.PHONY: all clean fclean re

#		  B U I L D  R U L E S		  #

all: $(NAME)

$(NAME): $(CRYPTOPP_LIB) $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp $(INCS_DIR)
	mkdir -p $(OBJS_DIR)
	$(CXX) $(INCS) $(CXXFLAGS) -o $@ -c $<


#            C R Y P T O P P          #

# If cryptopp folder is missing, download it, then compile.
# If only the library is missing, compile it.
$(CRYPTOPP_LIB):
	@if [ ! -d "cryptopp" ]; then \
		git clone https://github.com/weidai11/cryptopp.git; \
	fi
	make -C cryptopp/

#              T E S T I N G          #

hex: all
	@echo "$(INFO) Testing with a Hex key..."
	@echo "$(INFO) Generate and save the encrypted key to the external file 'ft_otp.key'."
	./ft_otp -g keys/key.hex
	@echo "$(DONE)"
	@echo "--------------------------------------------------"

	@echo "$(INFO) Decode the encrypted key and generate a TOTP code from it."
	./ft_otp $(ENCRYPTED_KEY_FILE) -k
	@echo "$(DONE)"

	@echo "--------------------------------------------------"
	@echo "$(INFO) Compare our TOTP code to the one delivered by 'oathtool'."
	oathtool --totp $(shell cat keys/key.hex) -v
	@echo "$(DONE)"


# C L E A N  &  O T H E R  R U L E S  #

RM = rm -rf

clean:
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all