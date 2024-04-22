/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/22 14:42:26 by vpoirot          ###   ########.fr       */
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
    std::cout << GREEN << "[SERVER] Listening..." << RESET  << std::endl;

    std::vector<pollfd> fds;
     
    /*while (true) {

        struct sockaddr_in cli_address;
		socklen_t cli_address_len = sizeof(cli_address);
		int cli_socket = accept(_socket, (struct sockaddr *)&cli_address, &cli_address_len);

        if (cli_socket == -1)
            this->end("Client rejected");

        char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(cli_address.sin_addr), clientIP, INET_ADDRSTRLEN);
		std::cout << YELLOW "[CLIENT] " << clientIP << " connected" << RESET << std::endl;

        pollfd client_fd;
		client_fd.fd = cli_socket;
		client_fd.events = POLLIN;
		fds.push_back(client_fd);

        std::cout << "fds.size() =" << fds.size() << std::endl;
        
        int poll_nbr = poll(&fds[0], fds.size(), 250);
        if (poll_nbr == -1)
                this->end("poll() exception");
            
        while (poll_nbr != 0)
        {   
            for (size_t i = 0; i < fds.size(); i++)
            {
                if (fds[i].revents & POLLIN)
                {
                    char buffer[1024];
                    int bytes_received = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                    if (bytes_received == -1)
                        this->end("recv() exception");
                    else if (bytes_received == 0)
                    {
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        std::cout << "Connection closed by client" << std::endl;
                        break;
                    }
                    else
                    {
                        buffer[bytes_received] = '\0';
                        std::cout << "Received from client " << fds[i].fd << ": " << buffer << std::endl;
                    }
                }
            }
            poll_nbr = poll(&fds[0], fds.size(), 250);
            if (poll_nbr == -1)
                this->end("poll() exception");
        }
    }*/



    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(_socket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Erreur lors de l'acceptation de la connexion" << std::endl;
            close(_socket);
            exit(EXIT_FAILURE);
        }

        // Récupération de l'adresse IP du client
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::cout << "Client connecté depuis l'adresse IP : " << clientIP << std::endl;

        // Lecture des données envoyées par le client
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, 1024, 0);
        if (bytesRead == -1) {
            std::cerr << "Erreur lors de la lecture des données du client" << std::endl;
            close(clientSocket);
            continue;
        }

        // Analyse des commandes USER et NICK envoyées par le client
        std::string data(buffer, bytesRead);
        size_t userPos = data.find("USER");
        size_t nickPos = data.find("NICK");
        if (userPos != std::string::npos && nickPos != std::string::npos) {
            std::string username = data.substr(userPos + 5, data.find('\n', userPos) - (userPos + 5));
            std::string nickname = data.substr(nickPos + 5, data.find('\n', nickPos) - (nickPos + 5));
            std::cout << "Nom d'utilisateur : " << username << std::endl;
            std::cout << "Pseudo : " << nickname << std::endl;
        }

        // Envoyer une réponse au client (par exemple, un message de bienvenue)
        std::string welcomeMessage = "Bienvenue sur le serveur IRC !";
        send(clientSocket, welcomeMessage.c_str(), welcomeMessage.length(), 0);

        //close(clientSocket); // Fermer la connexion avec le client
    }

}

void Server::end(std::string msg) {
    close(_socket);
    if (msg.length() != 0)
        throw std::runtime_error(msg);
}

void Server::handle_client(int client_socket) {
    (void)client_socket;
}
