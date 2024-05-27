/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/27 14:22:33 by vpoirot          ###   ########.fr       */
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
		throw std::runtime_error("Failed to create server socket.");

	int option = 1;
	if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		this->crash("Failed to configure server socket.");

	// Socket configuration
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = INADDR_ANY;
	serv_address.sin_port = htons(serv_port);

	if (bind(serv_socket, (struct sockaddr *)&serv_address, sizeof(serv_address)) == -1)
		this->crash("Unable to bind server socket.");
	if (listen(serv_socket, MAX_WAITLIST) == -1)
		this->crash("Unable to listen on server socket.");

	std::cout << irc_time() << BLUE << "Server listening on port " << serv_port << RESET << std::endl;

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

		// Event occured on the server socket : its a new client connection
		if (FD_ISSET(serv_socket, &monitored)) {
			// Accepting new client connexion, then storing the new client in the 'clients' vector
			Client client;
			int clientSocket = accept(serv_socket, (struct sockaddr *)&(client.getAddressREF()), &(client.getAddressLenREF()));
			if (clientSocket == -1)
				this->crash("Could not accept connexion");

			client.setSocket(clientSocket);
			client.setIP(inet_ntoa(client.getAddress().sin_addr));
			all_clients.push_back(client);

			std::cout << irc_time() << GREEN << "New connection" << RESET << std::endl;
		}

		// Event occured on a client socket : either a disconnection, or a text input
		for (std::vector<Client>::iterator iter_client = all_clients.begin(); iter_client != all_clients.end();)
		{
			if (FD_ISSET(iter_client->getSocket(), &monitored)) {
				// Receiving data from client, using a buffer
				char buffer[1024];
				int bytesReceived = recv(iter_client->getSocket(), buffer, 1024, 0);

				// If buffer is empty, it's a disconnection
				if (bytesReceived <= 0) {
					std::cout << irc_time() << YELLOW << iter_client->getNickname() << " left the game." << RESET << std::endl;

					// Remove client from pass_list
					for (std::vector<int>::iterator i = pass_list.begin(); i != pass_list.end(); ++i) {
						if (*i == iter_client->getSocket()) {
							pass_list.erase(i);
							break;
						}
					}

					// Remove client from all channels, and their whitelists
					std::vector<Channel> current_channels = iter_client->getCurrentChannels();
					for (std::vector<Channel>::iterator iter_channel = current_channels.begin(); iter_channel != current_channels.end(); ++iter_channel) {
						iter_channel->removeToWhitelist(iter_client->getSocket());
						iter_channel->removeClientFromChannel(iter_client->getSocket());
						if (iter_channel->getAllUsers().size() == 0)
							all_channels.erase(std::remove(all_channels.begin(), all_channels.end(), *iter_channel), all_channels.end());
					}

					// Close the client socket and remove the client from all_clients
					close(iter_client->getSocket());
					iter_client = all_clients.erase(iter_client);
					continue;
				}

				// Otherwise, handle the input
				try
				{
					std::string client_input(buffer, bytesReceived);
					check_password(client_input, iter_client);
					handle_client_input(client_input, iter_client);
				}
				catch (const std::exception &e)
				{
					// Any exception thrown in the handling of the input is reported to both server and client
					// std::string notif(irc_time() + CYAN + iter_client->getNickname() + RESET ": " + RED + e.what() + RESET);
					// std::cout << notif << std::endl;
					std::string notif = irc_time() + RED + e.what() + '\n' + RESET;
					send(iter_client->getSocket(), notif.c_str(), std::strlen(notif.c_str()) + 1, 0);
				}
			}
			++iter_client;
		}
	}
}


void Server::crash(std::string log)
{
	close(serv_socket);
	if (log.length() != 0)
		throw std::runtime_error(log);
}


void Server::check_password(std::string data_sent, std::vector<Client>::iterator &sender) {
	for (size_t i = 0; i != pass_list.size(); i++) {
		if (pass_list[i] == sender->getSocket()) {
			std::istringstream iss(data_sent);
			std::string cmd;
			iss >> cmd;
			if (sender->getNickname() == "/" && cmd != "NICK")
				throw std::runtime_error("Choose a Nickname (NICK <nickname>)");
			else if (sender->getNickname() != "/" && sender->getUsername().size() == 0 && cmd != "USER")
				throw std::runtime_error("Choose a Username (USER <username> <hostname> <servername> <realname>)");
			return ;
		}
	}
	size_t it = data_sent.find("PASS");
	if (it == data_sent.npos)
		throw std::runtime_error("Wrong password, try again.");
	it += 5;
	std::istringstream iss(&data_sent[it]);
	std::string password_sent;
	iss >> password_sent;
	if (password_sent != this->serv_password)
		throw std::runtime_error("Wrong password, try again.");
	//validation
	pass_list.push_back(sender->getSocket());
	std::string notif(GREEN "You are now logged in.\n" RESET);
	send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
	std::cout << sender->getIP() << " logged in." << std::endl;
}

void Server::handle_client_input(std::string data_sent, std::vector<Client>::iterator &sender)
{
	std::istringstream iss(data_sent);
	std::string command;
	iss >> command;

	// parse client input
	if (command == "NICK")
		sender->setNickname(data_sent, all_clients);
	else if (command == "USER")
		sender->setUsername(data_sent);
	else if (command == "JOIN")
		cmd_join(data_sent, sender);
	else if (command == "PRIVMSG")
		cmd_privmsg(data_sent, sender);
	else if (command == "PART")
		cmd_part(data_sent, sender);
	else if (command == "KICK")
		cmd_kick(data_sent, sender);
	else if (command == "MSG")
		cmd_msg(data_sent, sender);
	else if (command == "MODE")
		cmd_mode(data_sent, sender);
	else if (command == "INVITE")
		cmd_invite(data_sent, sender);
	else if (command == "TOPIC")
		cmd_topic(data_sent, sender);
	else if (command == "HELP")
		cmd_help(data_sent, sender);
	else if (command != "PASS") {
		if (sender->getInteractions().size())
			msg_to_channel(data_sent, sender->getLastInteraction(), sender);
		else
			throw std::runtime_error("Unrecognized command. Try using 'HELP'");
	}
}



void	Server::msg_to_channel(std::string msg, std::string channel_name, std::vector<Client>::iterator &sender)
{
	std::string message = irc_time() + "[" + channel_name + "] " + BLUE + sender->getNickname() + RESET + ": " + msg + "\n";
	bool found = false;
	for (size_t i = 0; i != all_channels.size(); i++) {
		if (all_channels[i].getName() == channel_name || all_channels[i].getName() == channel_name + "\n") {
			found = true;
			for (size_t j = 0; j != all_channels[i].getAllUsers().size(); j++) {
				send(all_channels[i].getAllUsers()[j], message.c_str(), strlen(message.c_str()), MSG_DONTWAIT);
			}
		}
	}
	if (found == false)
		throw std::runtime_error("No channel found. Message was not sent.");
}



/////////////////
//  Accessors  //
/////////////////
