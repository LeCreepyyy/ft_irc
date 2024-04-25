/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/25 13:29:07 by vpoirot          ###   ########.fr       */
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
	std::vector<Client> clients;
	std::vector<int> client_sockets;
	fd_set readfds;
	int maxFd;

	while (true)
	{
		// clear le fd_set
		FD_ZERO(&readfds);

		// ajouter le socket du server dans fd_set
		FD_SET(serv_socket, &readfds);
		maxFd = serv_socket;

		// ajouter le socket client dans fd_set
		for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			int clientSocket = it->getSocket();
			FD_SET(clientSocket, &readfds);
			maxFd = std::max(maxFd, clientSocket);
		}

		// Surveiller les events sur les socket de readfds
		int activity = select(maxFd + 1, &readfds, NULL, NULL, NULL);
		if ((activity < 0) && (errno != EINTR))
			this->end("Select() exception");

		// si ya un event sur le serv_socket, c'est une nouvelle connexion de client
		if (FD_ISSET(serv_socket, &readfds))
		{
			Client client;
			int clientSocket = accept(serv_socket, (struct sockaddr *)&(client.getAddressREF()), &(client.getAddressLenREF()));
			if (clientSocket == -1)
				this->end("Could not accept connexion");
			client.setClientSocket(clientSocket);
			client.setIP(inet_ntoa(client.getAddress().sin_addr));
			std::cout << "New connection from " << client.getIP() << std::endl;

			// ajouter le nouveau socket client dans le container
			clients.push_back(client);
		}

		// gerer les events des sockets client
		for (std::vector<Client>::iterator it = clients.begin(); it != clients.end();)
		{
			if (FD_ISSET(it->getSocket(), &readfds))
			{
				char buffer[1024];
				int bytesReceived = recv(it->getSocket(), buffer, 1024, 0);
				if (bytesReceived <= 0)
				{
					std::cout << "Client disconnected" << std::endl;
					close(it->getSocket());
					it = clients.erase(it);
					continue;
				}

				std::string message(buffer, bytesReceived);
				// parse msg + exec command
				std::cout << it->getUsername() << " send : " << message;

				// Parse and handle IRC message
				// Here you would implement your IRC protocol logic
				// For simplicity, let's just echo the message back to the client
				
				//send(it->getSocket(), buffer, bytesReceived, 0);
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