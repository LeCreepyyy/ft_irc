/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:09:45 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/24 13:36:29 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <string>
# include <cstdlib>
# include <cstring>
# include <map>
# include <sys/types.h>
# include <sys/socket.h>
# include <cerrno>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <poll.h>
# include <vector>

// for client object
# define NICKNAME 0
# define USERNAME 1
# define CLIENT_SOCKET 2
# define CLIENT_ADDRESS 3
# define CLIENT_ADDRESS_LEN 4

# define RESET   "\033[0m" // RESET COLOR
# define RED     "\033[31m" // ERROR
# define GREEN   "\033[32m" // LOG - START END
# define YELLOW  "\033[33m" // CLIENT CONNECTION - DISCONECTION
# define MAGENTA "\033[35m" // CLIENT ACTION
# define GRAY    "\033[37m" // CLIENT INFO
# define CYAN    "\033[36m" // DEBUG
# define BLUE    "\033[34m" // SERVER
# define WHITE   "\033[37m" // ...

# include "Server.hpp"
# include "Client.hpp"

void    parsing(char **argv);

template<typename T>
void debug(T log) {
	std::cout << "\033[36m" << "[DEBUG] " << log << std::endl;
}

#endif