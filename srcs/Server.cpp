/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/16 14:02:41 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

Server::Server(std::string name, std::string password, int port) {
    _name = name;
    _password = password;
    _port = port;
    std::memset(&_adress, 0, sizeof(_adress));
}

Server::~Server() {
    ;
}

void Server::config() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
        throw std::runtime_error("[ERROR] Sockets creation failed");
    _adress.sin_family = AF_INET;
    _adress.sin_addr.s_addr = INADDR_ANY;
    _adress.sin_port = htons(_port);
    std::cout << GREEN << "[SERVER] Config OK" << RESET  << std::endl;
}

void Server::start() {
    if (bind(_socket, (struct sockaddr *)&_adress, sizeof(_adress)) == -1
        || listen(_socket, MAX_WAITLIST) == -1) {
        close(_socket);
        throw std::runtime_error("[ERROR] Socket setup failed");
    }
    std::cout << GREEN << "[SERVER] Listening..." << RESET  << std::endl;
    while (true) {
        struct sockaddr_in client_adress;
        socklen_t client_adress_len = sizeof(client_adress);
        int client_socket = accept(_socket, (struct sockaddr *)&client_adress, &client_adress_len);
        if (client_socket == -1) {
            close(_socket);
            throw std::runtime_error("[ERROR] Client rejected");
        }
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_adress.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::cout << YELLOW << "[CLIENT] " << clientIP << " connected" << RESET << std::endl;
        //close(client_socket);
    }
}

void Server::end() {
    close(_socket);
}
