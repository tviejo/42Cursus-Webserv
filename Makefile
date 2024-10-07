NAME = webserv

CPP = c++

CPP_FLAGS = -Wall -Wextra -Werror -std=c++98 -g3

SRC_PATH = srcs/

SRC_FILE =	main.cpp \

SRCS = $(addprefix $(SRC_PATH), $(SRC_FILE))

INCLUDE = -I include/

OBJ_DIR = obj/

OBJ_FILE = $(SRCS:.cpp=.o)

OBJS = $(OBJ_FILE)

all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	$(CPP) $(CPP_FLAGS) $(INCLUDE) -c $< -o $@

$(OBJ_DIR): 
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS)
	$(CPP) $(CPP_FLAGS) $(INCLUDE) -o $(NAME) $(OBJS)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONE: all clean fclean re
