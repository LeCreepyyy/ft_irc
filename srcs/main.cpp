/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:11:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/15 14:19:03 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/irc.hpp"

int main(int argc, char** argv) {
    
    // parsing [./ircserv <port> <password>] try catch

    int socket;// = socket();
}

int check_port(char* port) {
    for (int i = 0; i != 0; i++) {
        if (port[i] < '0' || port[i] > '9')
            return (-1); // throw runtime_error "port invalid"
    }
    return (atoi(port));
}
