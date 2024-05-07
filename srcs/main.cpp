/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:11:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/07 01:13:40 by bgaertne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

int main(int argc, char **argv)
{
	try
	{
		if (argc != 3)
			throw std::runtime_error("Usage:    ./irc <port> <password>");

		parsing_args(argv);
		Server server("Concorde", argv[2], atoi(argv[1]));
		server.start();
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << "[ERROR]" << e.what() << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

void parsing_args(char **argv)
{
	std::string port = argv[1];
	std::string password = argv[2];

	for (size_t i = 0; i != port.size(); i++)
	{
		if (!isdigit(port[i]))
			throw std::runtime_error("Invalid Port");
	}
	if (password.find(' ') != std::string::npos)
		throw std::runtime_error("Invalid Password");
}
