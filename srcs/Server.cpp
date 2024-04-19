/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: creepy <creepy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/19 18:22:12 by creepy           ###   ########.fr       */
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
    // Creation du socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
        throw std::runtime_error("[ERROR] Sockets creation failed");
    // Configuration du socket avec 
    _adress.sin_family = AF_INET;
    _adress.sin_addr.s_addr = htonl(INADDR_ANY);
    _adress.sin_port = htons(_port);
    std::cout << GREEN << "[SERVER] Config OK" << RESET  << std::endl;
}

std::string getClientName(int client_socket) {
    static std::map<int, std::string> client_name;

    std::map<int, std::string>::iterator it = client_name.find(client_socket);
    if (it != client_name.end())
        return it->second;
    return ("none");
}

void handle_client_action(const std::string & command, const std::string & name) {
    if (command == "/quit") {
        std::cout << YELLOW << "[CLIENT] " << name << " left the server" << std::endl;
    }
    else if (command.substr(0, 5) == "/join") {
        std::string channel_name = command.substr(6);
        //gerer le channel
        debugIRC("/join work");
    }
    else {
        std::cerr << MAGENTA << "[CLIENT] excute unknow command" << std::endl;
    }
}

void Server::start() {
    if (bind(_socket, (struct sockaddr *)&_adress, sizeof(_adress)) == -1
        || listen(_socket, MAX_WAITLIST) == -1) {
        close(_socket);
        throw std::runtime_error("[ERROR] Socket setup failed");
    }
    struct pollfd fds[1];
    fds[0].fd = _socket;
    fds[0].events = POLLIN;
    std::cout << GREEN << "[SERVER] Listening..." << RESET  << std::endl;
    int numClients = 0;
    while (true) {
        int numReady = poll(fds, numClients + 1, -1);
        if (numReady == -1) {
            std::cerr << "Erreur lors de l'appel à poll()" << std::endl;
            close(_socket);
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) {
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
            numClients++;
        }

        for (int i = 1; i <= numClients; ++i) {
            if (fds[i].revents & POLLIN) { // Données à lire
                char buffer[1024];
                ssize_t bytesRead = recv(fds[i].fd, buffer, 1024, 0);
                if (bytesRead <= 0) {
                    if (bytesRead == 0) {
                        std::cout << YELLOW << "[CLIENT] left server" << RESET << std::endl;
                        numClients--;
                    } else
                        throw std::runtime_error("data client can not read");
                    close(fds[i].fd);
                    continue;
                }
                std::string client_data(buffer, bytesRead);
                std::string client_name = getClientName(fds[i].fd);
            }
        }

        //close(client_socket);
    }
}

void Server::end() {
    close(_socket);
}

void Server::handle_client(int client_socket) {
    (void)client_socket;
}
