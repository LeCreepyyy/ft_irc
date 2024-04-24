/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/24 12:46:24 by bgaertne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

Server::Server(std::string name, std::string password, int port)
{
	serv_name = name;
	serv_password = password;
	serv_port = port;
	std::memset(&serv_address, 0, sizeof(serv_address));
}

Server::~Server()
{
	;
}

void Server::start()
{

	// Creation du serv_socket
	serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_socket == -1)
		throw std::runtime_error("[ERROR] serv_socket creation failed");

	int option = 1;
	if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		this->end("Setsockopt() exception");

	// Configuration du serv_socket
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_address.sin_port = htons(serv_port);

	// Binding et mise en ecoute du serv_socket
	if (bind(serv_socket, (struct sockaddr *)&serv_address, sizeof(serv_address)) == -1)
		this->end("Unable to bind serv_socket");
	if (listen(serv_socket, MAX_WAITLIST) == -1)
		this->end("Unable to listen");

	std::cout << BLUE << "Server listening on port " << serv_port << RESET << std::endl;

	// clients fds
	std::vector<int>	client_sockets ;
	fd_set				readfds;
	int					maxFd;

	while (true)
	{
		// clear le fd_set
		FD_ZERO(&readfds);

        // ajouter le socket du server dans fd_set
        FD_SET(serv_socket, &readfds);
        maxFd = serv_socket;

        // ajouter le socket client dans fd_set
       	for (std::vector<int>::const_iterator it = client_sockets.begin(); it != client_sockets.end(); ++it) {
   			int clientSocket = *it;
   			FD_SET(clientSocket, &readfds);
    		maxFd = std::max(maxFd, clientSocket);
		}

        // Surveiller les events sur les socket de readfds
        int activity = select(maxFd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR))
            this->end("Select() exception");

        // si ya un event sur le serv_socket, c'est une nouvelle connexion de client
        if (FD_ISSET(serv_socket, &readfds)) {
            struct sockaddr_in	client_address;
            socklen_t 			client_address_len = sizeof(client_address);
            int client_socket = accept(serv_socket, (struct sockaddr*)&client_address, &client_address_len);
            if (client_socket == -1)
                this->end("Could not accept connexion");

            std::cout << "New connection from " << inet_ntoa(client_address.sin_addr) << std::endl;

            // ajouter le nouveau socket client dans le container 
            client_sockets.push_back(client_socket);
        }

        // gerer les events des sockets client
        for (std::vector<int>::iterator it = client_sockets.begin(); it != client_sockets.end();) {
            int client_socket = *it;
            if (FD_ISSET(client_socket, &readfds)) {
                char buffer[1024];
                int bytesReceived = recv(client_socket, buffer, 1024, 0);
                if (bytesReceived <= 0) {
                    std::cout << "Client disconnected" << std::endl;
                    close(client_socket);
                    it = client_sockets.erase(it);
                    continue;
                }

                std::string message(buffer, bytesReceived);
                std::cout << "Received message from client: " << message;

                // Parse and handle IRC message
                // Here you would implement your IRC protocol logic
                // For simplicity, let's just echo the message back to the client
                send(client_socket, buffer, bytesReceived, 0);
            }
            ++it;
        }
	}
}

void Server::end(std::string log)
{
	close(serv_socket);
	if (log.length() != 0)
		throw std::runtime_error(log);
}

void Server::handle_client(int client_socket)
{
	(void)client_socket;
}