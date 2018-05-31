# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: amaindro <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/05/31 14:50:34 by amaindro          #+#    #+#              #
#    Updated: 2018/05/31 15:05:26 by amaindro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = woody_woodpacker

SRC = woody.c encrypt.c

OBJ = $(SRC:.c=.o)

LIB = -L libft/ -lft

MAKELIB = make -C ./libft

CLEANLIB = make clean -C ./libft

FCLEANLIB = make fclean -C ./libft

all : $(NAME)

$(NAME) : $(OBJ)
	@$(MAKELIB)
	@gcc -Wall -Wextra -Werror -c $(SRC)
	@gcc -Wall -Wextra -Werror -o $(NAME) $(OBJ) $(LIB)

clean :
	@rm -rf $(OBJ)
	@$(CLEANLIB)

fclean : clean
	@rm -rf $(NAME)
	@$(FCLEANLIB)

re : fclean all

.PHONY: all clean fclean re
