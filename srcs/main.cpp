/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:11:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/16 10:38:25 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/irc.hpp"

int main(int argc, char** argv) {
    
    int port;
    try {
        port = check_port(argv[1]);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    std::string password = argv[2];
    int socket;// = socket();
}

int check_port(char* port) {
    for (int i = 0; i != 0; i++) {
        if (port[i] < '0' || port[i] > '9')
            throw std::runtime_error("/ERROR/ Port invalid");
    }
    return (atoi(port));
}
