/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:11:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/07 11:51:48 by vpoirot          ###   ########.fr       */
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
		std::cerr << "[ERROR]" << e.what() << std::endl;
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
			throw std::runtime_error("Invalid Port.");
	}
	if (password.find(' ') != std::string::npos)
		throw std::runtime_error("Invalid Password.");
}

std::string		irc_time()
{
	time_t now = time(0);
	
	tm *localTime = localtime(&now);

	char str[100];
	strftime(str, sizeof(str), "[%H:%M] ", localTime);
	return std::string(str);
}

std::vector<std::string>	splitString(std::string client_input, char spliter)
{
	std::vector<std::string> result;
	std::string::size_type start = 0;
	std::string::size_type end = client_input.find(spliter);

	while (end != std::string::npos) {
		result.push_back(client_input.substr(start, end - start + 1));
		start = end + 1;
		end = client_input.find(spliter, start);
	}

	if (start < client_input.length()) {
		result.push_back(client_input.substr(start));
	}

	return result;
}
