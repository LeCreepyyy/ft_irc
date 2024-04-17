/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:11:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/17 13:54:05 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/irc.hpp"

int main(int argc, char** argv) 
{
    
    int port;
    try {
        if (argc != 3)
            throw std::runtime_error("[ERROR] Invalid arg");
        check_port(argv[1]);
        port = atoi(argv[1]);
    }
    catch (const std::exception& e) {
        std::cerr << RED << e.what() << RESET  << std::endl;
        return (1);
    }
    Server server("Concorde", argv[2], port);
    try {
        server.config();
        server.start();
        server.end();
    }
    catch (const std::exception& e) {
        std::cerr << RED << e.what() << RESET << std::endl;
        return (1);
    }
    return (0);
}

void    check_port(std::string port) {
    for (size_t i = 0; i != port.size(); i++) {
        if (port[i] < '0' || port[i] > '9')
            throw std::runtime_error("[ERROR] Port invalid");
    }
}

void    debugIRC(std::string str)
{
    std::cout << CYAN << "[DEBUG] " << str << RESET << std::endl;
} 
