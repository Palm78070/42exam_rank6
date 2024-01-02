NAME := outfile

CC := gcc

CFLAGS := -Wall -Werror -Wextra -g -fsanitize=address

SRCS := mini_serv.c \

OBJS := $(SRCS:.o=.c)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clear_bind:
	kill -9 $$(lsof -i :8080 | grep IPv4 | awk '{print $$2}')

clean:
	rm -rf *.o *dSYM

fclean: clean
	rm -rf $(NAME)

re: clean fclean all

.PHONY: all clean re fclean
