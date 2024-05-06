/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/06 16:54:33 by bgaertne         ###   ########.fr       */
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

std::string getCanalName(const std::string &chaine)
{
	std::istringstream iss(chaine);
	std::string premierMot;
	iss >> premierMot;
	return premierMot;
}

void Server::send_message(std::string msg, std::string canal, std::vector<Client>::iterator &client)
{
	std::string message = "[" + canal + "] " + client->getNickname() + " : " + msg + "\n";
	canal += "\n";
	debug(canal);
	for (size_t i = 0; i != this->channels.size(); i++)
	{
		debug(channels[i].getName());
		if (channels[i].getName() == canal)
		{
			for (size_t j = 0; j != channels[i].getChannelUsers().size(); i++)
			{
				send(channels[i].getChannelUsers()[j], message.c_str(), message.size(), MSG_DONTWAIT);
			}
		}
	}
}

void Server::handle_client_input(std::string data_sent, std::vector<Client>::iterator &client)
{
	if (!data_sent.find("NICK"))
	{
		debug(client->getNickname().append(" used command NICK"));
		client->setNickname(data_sent);
	}
	else if (!data_sent.find("USER"))
	{
		debug(client->getNickname().append(" used command USER"));
		client->setUsername(data_sent);
	}
	else if (!data_sent.find("JOIN"))
	{
		debug(client->getNickname().append(" used command JOIN"));
		if (data_sent[5] != '#')
			throw std::runtime_error("Improper channel name in JOIN command\n");
		for (size_t i = 0; i != this->channels.size(); i++)
		{
			if (channels[i].getName() == &data_sent[6])
			{
				debug("channel exists");
				channels[i].addClientToChannel(client->getSocket()); // add client in channel
				client->addToCurrentChannels(channels[i]);			 // add channel in client
				send(client->getSocket(), "Channel joined !\n", strlen("Channel joined !\n"), MSG_DONTWAIT);
				return;
			}
		}
		Channel newChannel(&data_sent[6], client->getSocket());
		debug("channel created");
		newChannel.addClientToChannel(client->getSocket());
		newChannel.addClientToChannelOps(client->getSocket());
		client->addToCurrentChannels(newChannel);
		this->channels.push_back(newChannel);

		send(client->getSocket(), "Channel joined !\n", strlen("Channel joined !\n"), MSG_DONTWAIT);
	}
	else
	{
		if (!data_sent.find("/msg"))
		{
			send_message(&data_sent[getCanalName(&data_sent[5]).size() + 6], getCanalName(&data_sent[5]), client);
		}
		else if (!data_sent.find("/say"))
			;
		else
			;
	}
	// /msg <#canal> <msg>	Envoyer un message dans un channel non rejoins par le client
	// /say <msg>			Envoyer un message dans le channel rejoins par le client
	// <msg>				Envoyer un message dans le dernier channel ou le client a interagit (et est rejoins)
	// exemple msg : [channel_name] (user-nick)name : <msg>
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
	serv_address.sin_addr.s_addr = INADDR_ANY;
	serv_address.sin_port = htons(serv_port);

	// Binding et mise en ecoute du serv_socket
	if (bind(serv_socket, (struct sockaddr *)&serv_address, sizeof(serv_address)) == -1)
		this->end("Unable to bind serv_socket");
	if (listen(serv_socket, MAX_WAITLIST) == -1)
		this->end("Unable to listen");

	std::cout << BLUE << "Server listening on port " << serv_port << RESET << std::endl;

	// clients fds
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

		// Surveiller les events sur les sockets de readfds
		if ((select(maxFd + 1, &readfds, NULL, NULL, NULL) < 0) && (errno != EINTR))
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
			std::cout << YELLOW << "[CLIENT] New connection from " << client.getIP() << RESET << std::endl;

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
					std::cout << YELLOW << "[CLIENT] " << it->getNickname() << " left server (" << it->getIP() << ")" << std::endl;
					close(it->getSocket());
					it = clients.erase(it);
					continue;
				}

				std::string client_input(buffer, bytesReceived);
				try
				{
					handle_client_input(client_input, it);
				}
				catch (const std::exception &e)
				{
					debug(e.what());
					send(it->getSocket(), e.what(), std::strlen(e.what()), 0);
				}
				std::cout << "[" << it->getNickname() << "] : " << client_input;
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
