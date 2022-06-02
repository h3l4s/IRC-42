DEFAULT = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m
TESTCOL = \033[1;35m

UNAME = $(shell uname -s)

CFLAGS =-g3 -fsanitize=address -Wall -Wextra -Werror -std=c++98
RM = rm -f
CC1 = c++
CC2 = c++
SRC_DIR = src
OBJ_DIR = obj
BIN = ircserv
NAME = ircserv
INC = inc/server.hpp
ECHO = echo
ECHON = echo -n
ECHOE = printf

SRC = main.cpp server.cpp

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all: $(NAME)

$(NAME): create_dirs $(OBJ)
	@$(CC1) $(CFLAGS) -I $(INC) $(OBJ) -o $@
	@$(ECHON) "                                            \r"
	@$(ECHO) "\rCompilation succed$(GREEN)"
	@$(ECHO) "$(WHITE)$(BIN) is up to date!$(WHITE)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(ECHON) "                                            \r"
	@$(ECHON) "Compiling $(GRAY)$<$(GREEN)\r"
	@$(CC2) $(CFLAGS) -I $(INC) -c $< -o $@

create_dirs:
	@mkdir -p $(OBJ_DIR)

clean:
	@$(ECHON) "$(GREEN)Cleaning up object files in $(OBJ_DIR), and $(OBJ_LFT_DIR)...\r"
	@$(ECHON) "                                                                     \r"
	@$(ECHO) "$(GRAY)\rCleaning done"
	@$(RM) -r $(OBJ_DIR)

fclean: clean
	@$(RM) -r $(BIN)
	@$(ECHO) "$(GRAY)Removed $(BIN)$(GREEN)"

re: fclean all

git:
	git add .
	git commit
	git push

-include $(OBJ_DIR)/*.d

.PHONY: all clean fclean create_dirs re