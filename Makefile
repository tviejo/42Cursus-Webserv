NAME        =   webserv

SRC_DIR     =   srcs/

OBJ_DIR     =   objs/

SRCS        =   main.cpp

SRCS		+=  autoindex/autoindex.cpp

SRCS		+=  cgi/cgi.cpp

SRCS		+=  config/config.cpp

SRCS		+=  request/HttpRequest.cpp

SRCS		+=  response/Response.cpp
SRCS		+=  response/OutgoingData.cpp

SRCS		+=  server/Server.cpp

SRCS		+=  utils/utils.cpp
SRCS		+=  utils/timer.cpp
SRCS		+=  utils/Terminal.cpp

vpath %.cpp $(SRC_DIR)

OBJS        =   $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRCS))

CC          =   c++

CFLAGS      =   -Wall -Wextra -Werror -std=c++98 -MMD -MP -g3

INCLUDES    =   -I includes/ -I libft/includes/

LIBFT       =   libft/libft.a

LIBS        =   -L libft -lft

D_FILES		=	$(OBJS:.o=.d)

RM          =   rm -f

GREEN=\033[0;32m
RED=\033[0;31m
BLUE=\033[0;34m
END=\033[0m
BOLD_START=\e[1m
BOLD_END=\e[0m

ifeq ($(debug), true)
    CFLAGS += -g3 -fsanitize=address,undefined
endif

define PRINT_LOADING
	@printf "$(GREEN)Compiling libft["
	@for i in $(shell seq 0 10 100); do \
		printf "▓"; \
		sleep 0.1; \
	done
	@printf "] 100%%$(RESET)\n$(END)"
endef

all:            $(LIBFT) ${NAME}
				@echo "$(GREEN)$(BOLD_START)${NAME} created$(BOLD_END)$(END)"

${NAME}:        ${OBJS}
				$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)

$(OBJ_DIR)%.o: %.cpp
				@echo "$(BLUE)Compiling: $@ $(END)"
				mkdir -p $(OBJ_DIR)	$(OBJ_DIR)/autoindex $(OBJ_DIR)/cgi $(OBJ_DIR)/config $(OBJ_DIR)/request $(OBJ_DIR)/response $(OBJ_DIR)/server $(OBJ_DIR)/utils
				$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT):
				@$(PRINT_LOADING)
				$(MAKE) -s all -C libft/


clean:
				$(RM) -r $(OBJ_DIR)
				${RM} ${OBJS}
				@echo "$(RED)Clean libft$(END)"
				$(MAKE) clean -s -C ./libft/
				@echo "$(GREEN)$(BOLD_START)Clean done$(BOLD_END)$(END)"

fclean: clean
				${RM} ${NAME}
				$(MAKE) fclean -s -C ./libft/
				@echo "$(RED)Fclean libft$(END)"
				@echo "$(GREEN)$(BOLD_START)Fclean done$(BOLD_END)$(END)"

re: fclean all

sinclude $(D_FILES)

.PHONY: all clean fclean re
.SILENT:
