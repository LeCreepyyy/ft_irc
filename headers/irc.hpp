/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:09:45 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/17 13:54:12 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <string>
# include <cstdlib>
# include <cstring>
# include <sys/types.h>
# include <sys/socket.h>
# include <cerrno>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <poll.h>

# include "Server.hpp"

# define RESET   "\033[0m"
# define RED     "\033[31m" // ERROR
# define GREEN   "\033[32m" // LOG - START END
# define YELLOW  "\033[33m" // CLIENT CONNECTION - DISCONECTION
# define BLUE    "\033[34m" // ...
# define MAGENTA "\033[35m" // CLIENT ACTION
# define CYAN    "\033[36m" // DEBUG
# define WHITE   "\033[37m" // ...

void    check_port(std::string port);
void    debugIRC(std::string str);

#endif