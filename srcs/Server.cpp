/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/07 02:29:47 by bgaertne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

////////////////////////////////
//  Constructors, Destructor  //
////////////////////////////////

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



///////////////
//  Methods  //
///////////////

void Server::start()
{
	// Creating the server's socket
	serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_socket == -1)
		throw std::runtime_error("serv_socket creation failed");

	int option = 1;
	if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		this->crash("setsockopt() exception");

	// Socket configuration
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = INADDR_ANY;
	serv_address.sin_port = htons(serv_port);

	if (bind(serv_socket, (struct sockaddr *)&serv_address, sizeof(serv_address)) == -1)
		this->crash("Unable to bind serv_socket");
	if (listen(serv_socket, MAX_WAITLIST) == -1)
		this->crash("Unable to listen");

	std::cout << BLUE << "Server listening on port " << serv_port << RESET << std::endl;

	// Server is now fully configured.
	// Listening to any events on server/clients sockets endlessly

	// Preparing storage for client sockets
	std::vector<int> client_sockets;
	fd_set monitored;
	int maxFd;

	while (true)
	{
		// Clear the fd_set
		FD_ZERO(&monitored);
		// (re)Adding the server socket in the fd_set
		FD_SET(serv_socket, &monitored);
		maxFd = serv_socket;

		// (re)Adding each client socket in the fd_set
		for (std::vector<Client>::iterator it = all_clients.begin(); it != all_clients.end(); ++it) {
			int clientSocket = it->getSocket();
			FD_SET(clientSocket, &monitored);
			maxFd = std::max(maxFd, clientSocket);
		}

		// Watch out for any events happening on any sockets stored in the fd_set
		if ((select(maxFd + 1, &monitored, NULL, NULL, NULL) < 0) && (errno != EINTR))
			this->crash("Select() exception");

		// Event occured on the server socket : has to be a new client connection
		if (FD_ISSET(serv_socket, &monitored)) {
			// Accepting new client connexion, then storing the new client in the 'clients' vector
			Client client;
			int clientSocket = accept(serv_socket, (struct sockaddr *)&(client.getAddressREF()), &(client.getAddressLenREF()));
			if (clientSocket == -1)
				this->crash("Could not accept connexion");

			client.setSocket(clientSocket);
			client.setIP(inet_ntoa(client.getAddress().sin_addr));
			all_clients.push_back(client);

			std::cout << YELLOW << "[CLIENT] New connection from " << client.getIP() << RESET << std::endl;
		}

		// Event occured on a client socket : either a disconnection, or a text input
		for (std::vector<Client>::iterator it = all_clients.begin(); it != all_clients.end();)
		{
			if (FD_ISSET(it->getSocket(), &monitored)) {
				// Receiving data from client, using a buffer
				char buffer[1024];
				int bytesReceived = recv(it->getSocket(), buffer, 1024, 0);

				// If buffer is empty, its a disconnection
				if (bytesReceived <= 0) {
					std::cout << YELLOW << "[CLIENT] " << it->getNickname() << " left server (" << it->getIP() << ")" << RESET << std::endl;
					close(it->getSocket());
					it = all_clients.erase(it);
					continue;
				}

				// Otherwise, handle the input
				try
				{
					std::string client_input(buffer, bytesReceived);
					handle_client_input(client_input, it);
					std::cout << "[" << it->getNickname() << "]: " << client_input;
				}
				catch (const std::exception &e)
				{
					// Any exception thrown in the handling of the input is reported to
					// both server and client as should a parsing error be
					debug(e.what());
					send(it->getSocket(), e.what(), std::strlen(e.what()), 0);
				}
			}
			++it;
		}
	}
}


void Server::crash(std::string log)
{
	close(serv_socket);
	if (log.length() != 0)
		throw std::runtime_error(log);
}


void Server::handle_client_input(std::string data_sent, std::vector<Client>::iterator &sender)
{
	if (!data_sent.find("NICK")) {
		debug(sender->getNickname().append(" used command NICK"));
		sender->setNickname(data_sent);
	}

	else if (!data_sent.find("USER")) {
		debug(sender->getNickname().append(" used command USER"));
		sender->setUsername(data_sent);
	}

	else if (!data_sent.find("JOIN")) {
		debug(sender->getNickname().append(" used command JOIN"));
		cmd_join(data_sent, sender);
	}

	else {
		if (!data_sent.find("/msg")) {
			std::istringstream iss(&data_sent[5]);
			std::string channel_name;
			iss >> channel_name;
			msg_to_channel(&data_sent[channel_name.size() + 6], channel_name, sender);
		}
		/*else if (!data_sent.find("/say"))
			;
		else
			;*/
	}
	// /msg <#canal> <msg>	Envoyer un message dans un channel non rejoins par le client
	// /say <msg>			Envoyer un message dans le channel rejoins par le client
	// <msg>				Envoyer un message dans le dernier channel ou le client a interagit (et est rejoins)
	// exemple msg : [channel_name] (user-nick)name : <msg>
}


void	Server::msg_to_channel(std::string msg, std::string channel_name, std::vector<Client>::iterator &sender)
{
	std::string message = "[" + channel_name + "] " + sender->getNickname() + " : " + msg + "\n";
	channel_name += "\n";
	for (size_t i = 0; i != all_channels.size(); i++) {
		if (all_channels[i].getName() == channel_name) {
			for (size_t j = 0; j != all_channels[i].getAllUsers().size(); i++) {
				send(all_channels[i].getAllUsers()[j], message.c_str(), message.size(), MSG_DONTWAIT);
			}
		}
	}
}

void	Server::cmd_join(std::string data_sent, std::vector<Client>::iterator &sender)
{
	if (data_sent[5] != '#')
			throw std::runtime_error("Improper channel name in JOIN command\n");

	// Looking if the channel already exists
	std::string	channel_name(&data_sent[6], std::strlen(&data_sent[6]));
	for (size_t i = 0; i != all_channels.size(); i++) {
		if (all_channels[i].getName() == channel_name) {
			// Joining the channel
			debug("Channel exists: Joining...");
			all_channels[i].addClientToChannel(sender->getSocket()); // adding client to channel's user list
			sender->addToCurrentChannels(all_channels[i]);			 // adding channel to client's channel list
			// Notifying the client
			std::string	notif = " === You joined : " + channel_name;
			send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			return;
		}
	}

	// Channel does not exists, so we create it
	debug("Channel does not exists: creating it...");
	Channel	newChannel(&data_sent[6], sender->getSocket());
	newChannel.addClientToChannel(sender->getSocket()); // adding client to channel's user list
	newChannel.addClientToOperators(sender->getSocket()); // adding client to channel's operators, since he created it
	sender->addToCurrentChannels(newChannel); // adding channel to client's channel list
	all_channels.push_back(newChannel); // pushing newChannel into the all_channels list

	// Notifying the client
	std::string	notif = " === You created and joined : " + channel_name + " === You have ADMIN rights in this channel.\n";
	send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
}


/////////////////
//  Accessors  //
/////////////////
