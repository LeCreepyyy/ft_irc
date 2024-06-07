/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:09:45 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/07 11:25:56 by bgaertne         ###   ########.fr       */
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

# define RESET   "\x033[0m" // Reset
# define RED     "\x033[31m" // Error
# define GREEN   "\x033[32m" // Connection
# define YELLOW  "\x033[33m" // Disconnection
# define MAGENTA "\x033[35m" // Whispers
# define GRAY    "\x033[37m" // Debug
# define CYAN    "\x033[36m" // Client notif
# define BLUE    "\x033[34m" // Server

# include "Channel.hpp"
# include "Client.hpp"
# include "Server.hpp"

void						parsing_args(char **argv);
std::string					irc_time();
std::vector<std::string>	splitString(std::string client_input, char spliter);

template<typename T>
void debug(T log) {
	std::cout << "\033[37m" << "[DEBUG] " << log << RESET << std::endl;
}

#endif