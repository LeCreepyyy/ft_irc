/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/23 14:06:05 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

Server::Server(std::string name, std::string password, int port) {
    serv_name = name;
    serv_password = password;
    serv_port = port;
    std::memset(&serv_address, 0, sizeof(serv_address));
}

Server::~Server() {
    ;
}


void Server::start() {

    // Creation du serv_socket
    serv_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_socket == -1)
        throw std::runtime_error("[ERROR] serv_socket creation failed");
    
    // Configuration du serv_socket
    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_address.sin_port = htons(serv_port);
    
    // Binding et mise en ecoute du serv_socket
    if (bind(serv_socket, (struct sockaddr *)&serv_address, sizeof(serv_address)) == -1)
        this->end("Unable to bind serv_socket");
    if (listen(serv_socket, MAX_WAITLIST) == -1)
        this->end("Unable to listen");
    
    
    debugIRC("Listening...");
    
    // clients fds
    std::vector<pollfd> fds;
     
    while (true) {

        struct sockaddr_in  client_address;
		socklen_t           client_address_len = sizeof(client_address);
		int client_socket = accept(serv_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1)
            this->end("Client rejected");

        pollfd client_fd;
		client_fd.fd = client_socket;
		client_fd.events = POLLIN;
		fds.push_back(client_fd);

        char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(client_address.sin_addr), clientIP, INET_ADDRSTRLEN);
		std::cout << YELLOW "[CLIENT] " << clientIP << " try to connect" << RESET << std::endl;
        
        int poll_events = poll(&fds[0], fds.size(), 150);
        if (poll_events == -1)
                this->end("poll() exception");
        for (size_t i = 0; i != fds.size(); i++) {
            std::cout << i << " : " << fds[i].fd << std::endl;
        }
        while (poll_events != 0)
        {   
            for (size_t i = 0; i < fds.size(); i++)
            {
                if (fds[i].revents & POLLIN)
                {
                    char buffer[1024];
                    ssize_t bytesRead = recv(client_socket, buffer, 1024, 0);
                    if (bytesRead == -1) {
                        std::cerr << "Erreur lors de la lecture des données du client" << std::endl;
                        close(client_socket);
                        continue;
                    }
                    else if (bytesRead == 0)
                    {
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        std::cout << "Connection closed by client" << std::endl;
                        break;
                    }
                    else
                    {
                        buffer[bytesRead] = '\0';
                        std::string data(buffer, bytesRead);
                        size_t userPos = data.find("USER");
                        size_t nickPos = data.find("NICK");
                        if (userPos != std::string::npos && nickPos != std::string::npos) {
                            std::string username = data.substr(userPos + 5, data.find('\n', userPos) - (userPos + 5));
                            std::string nickname = data.substr(nickPos + 5, data.find('\n', nickPos) - (nickPos + 5));
                            std::cout << "Nom d'utilisateur : " << username << std::endl;
                            std::cout << "Pseudo : " << nickname << std::endl;
                        }
                        std::string welcomeMessage = "Bienvenue sur le serveur IRC !";
                        send(client_socket, welcomeMessage.c_str(), welcomeMessage.length(), 0);
                    }
                    memset(&buffer, 0, sizeof(buffer));
                }
            }
            poll_events = poll(&fds[0], fds.size(), 150);
            if (poll_events == -1)
                this->end("poll() exception");
        }
    }
}

void Server::end(std::string log) {
    close(serv_socket);
    if (log.length() != 0)
        throw std::runtime_error(log);
}

void Server::handle_client(int client_socket) {
    (void)client_socket;
}