/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:09:45 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/29 11:26:33 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <string>
# include <algorithm>
# include <cstdlib>
# include <cstring>
# include <map>
# include <list>
# include <sys/types.h>
# include <sys/socket.h>
# include <cerrno>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <poll.h>
# include <vector>
# include <sstream>
# include <ctime>

# define RESET   "\033[0m" // Reset
# define RED     "\033[31m" // Error
# define GREEN   "\033[32m" // Connection
# define YELLOW  "\033[33m" // Disconnection
# define MAGENTA "\033[35m" // Whispers
# define GRAY    "\033[37m" // Debug
# define CYAN    "\033[36m" // Client notif
# define BLUE    "\033[34m" // Server

# include "Channel.hpp"
# include "Client.hpp"
# include "Server.hpp"

void    		parsing_args(char **argv);
std::string		irc_time();

template<typename T>
void debug(T log) {
	std::cout << "\033[37m" << "[DEBUG] " << log << RESET << std::endl;
}

#endif