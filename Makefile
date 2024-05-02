
NAME        := ircserv
CC         := c++
FLAGS    := -Wall -Wextra -Werror -std=c++98

SRCS        :=      srcs/main.cpp \
						srcs/Server.cpp \
						srcs/Client.cpp \
						srcs/Channel.cpp \
                          
OBJS        := ${SRCS:.cpp=.o}

.cpp.o:
	@${CC} ${FLAGS} -c $< -o ${<:.cpp=.o}


CLR_RMV		:= \033[0m
RED		    := \033[1;31m
GREEN		:= \033[1;32m
YELLOW		:= \033[1;33m
BLUE		:= \033[1;34m
CYAN 		:= \033[1;36m
RM		    := rm -f

${NAME}:	${OBJS}
			@echo "$(GREEN)Compilation ${CLR_RMV}of ${YELLOW}$(NAME) ${CLR_RMV}..."
			@${CC} ${FLAGS} -o ${NAME} ${OBJS}
			@echo "$(GREEN)$(NAME) created[0m ✔️"

all:		${NAME}

clean:
			@${RM} *.o */*.o */*/*.o
			@echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)objs ✔️"

fclean:		clean
			@${RM} ${NAME}
			@echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)binary ✔️"

re:			fclean all