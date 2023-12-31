NAME				=	ft_otp
CXX					=	c++
CXXFLAGS			=	-std=c++11 -Wall -Wextra #-Werror
INCS_DIR			=	incs/
INCS				=	-I incs/
LDFLAGS				=	-L./cryptopp -lcryptopp
INCS				+=	-I cryptopp/

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

#		  B U I L D  R U L E S		  #

all: $(NAME)

$(NAME): cryptopp $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp $(INCS_DIR)
	mkdir -p $(OBJS_DIR)
	$(CXX) $(INCS) $(CXXFLAGS) -o $@ -c $<


#         C R Y P T O P P             #

cryptopp:
	git clone https://github.com/weidai11/cryptopp.git
	make -C cryptopp/


# C L E A N  &  O T H E R  R U L E S  #

RM = rm -rf

clean:
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re