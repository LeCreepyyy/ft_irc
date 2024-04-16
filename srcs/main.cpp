/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:11:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/16 11:06:39 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/irc.hpp"

void check_port(std::string port);

int main(int argc, char** argv) {
    
    int port;
    try {
        if (argc != 3)
            throw std::runtime_error("/ERROR/ Invalid arg");
        check_port(argv[1]);
        port = atoi(argv[1]);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    //std::string password = argv[2];
    //int socket;// = socket();
}

void check_port(std::string port) {
    for (size_t i = 0; i != port.size(); i++) {
        std::cout << port[i] << std::endl;
        if (port[i] < '0' || port[i] > '9')
            throw std::runtime_error("/ERROR/ Port invalid");
    }
}
