/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/21 14:59:56 by bgaertne         ###   ########.fr       */
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
					std::cout << irc_time() << YELLOW << iter_client->getIP() << " left the game." << RESET << std::endl;

					// Remove client from pass_list
					for (std::vector<int>::iterator i = pass_list.begin(); i != pass_list.end(); ++i) {
						if (*i == iter_client->getSocket()) {
							pass_list.erase(i);
							break;
						}
					}

					// Remove client from all channels
					std::vector<Channel> current_channel = iter_client->getCurrentChannels();
					for (std::vector<Channel>::iterator iter_channel = current_channel.begin(); iter_channel != current_channel.end(); ++iter_channel) {
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
					//std::cout << irc_time() << iter_client->getNickname() << ": " << client_input;
				}
				catch (const std::exception &e)
				{
					// Any exception thrown in the handling of the input is reported to both server and client
					std::string notif(irc_time() + iter_client->getNickname() + " : " + e.what());
					debug(notif);
					notif = irc_time() + RED + e.what() + '\n' + RESET;
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
			// placer ici le forcing sur le fait de mettre le nick et le user
			return ;
		}
	}
	size_t it = data_sent.find("PASS");
	if (it == data_sent.npos)
		throw std::runtime_error(RED "Wrong password, try again" RESET);
	it += 5;
	std::istringstream iss(&data_sent[it]);
	std::string password_sent;
	iss >> password_sent;
	if (password_sent != this->serv_password)
		throw std::runtime_error(RED "Wrong password, try again" RESET);
	//validation
	pass_list.push_back(sender->getSocket());
	std::string notif(GREEN "You are now logged in.\n" RESET);
	send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
	debug(sender->getIP() + " logged in.");
}


void Server::handle_client_input(std::string data_sent, std::vector<Client>::iterator &sender)
{
	std::istringstream iss(data_sent);
	std::string command;
	iss >> command;
	// parse client input
	if (command == "NICK") {
		debug(sender->getIP().append(" used command NICK"));
		sender->setNickname(data_sent, all_clients);
	}
	else if (command == "USER") {
		debug(sender->getIP().append(" used command USER"));
		sender->setUsername(data_sent);
	}
	else if (command == "JOIN") {
		debug(sender->getIP().append(" used command JOIN"));
		cmd_join(data_sent, sender);
	}
	else if (command == "PRIVMSG") {
		debug(sender->getIP().append(" used command PRIVMSG"));
		cmd_msg(sender, data_sent);
	}
	else if (command == "PART") {
		debug(sender->getIP().append(" used command PART"));
		std::istringstream iss(&data_sent[6]);
		std::string channel_name;
		iss >> channel_name;
		std::string notif = irc_time() + YELLOW + "You left " + channel_name + ".\n"+ RESET;
		send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		notif = YELLOW "I have left " + channel_name + "." + RESET;
		msg_to_channel(notif, channel_name, sender);
		std::vector<Channel> client_current_channels = sender->getCurrentChannels();


		////// MARCHE PO
		for (std::vector<Channel>::iterator iter_channel = client_current_channels.begin(); iter_channel != client_current_channels.end(); ++iter_channel) {
			if (iter_channel->getName() == channel_name + '\n') {
				iter_channel->removeClientFromChannel(sender->getSocket());
				if (iter_channel->getAllUsers().size() == 0)
					all_channels.erase(std::remove(all_channels.begin(), all_channels.end(), *iter_channel), all_channels.end());
			}
		}



	}
	else if (command == "MSG") {
		debug(sender->getIP().append(" used command MSG"));
		std::istringstream iss(&data_sent[4]);
		std::string channel_name;
		iss >> channel_name;
		msg_to_channel(&data_sent[channel_name.size() + 5], channel_name, sender);
	}
	else
	{
		if (sender->getLastInteraction().size())
			msg_to_channel(data_sent, sender->getLastInteraction(), sender);
	}
}


void	Server::cmd_msg(std::vector<Client>::iterator &sender, std::string data_sent)
{
	(void) sender;
	std::istringstream	iss(data_sent);
	std::string			receiver_nick;
	int i = 0;
	
	iss >> receiver_nick;
	i = i + receiver_nick.size() + 1;
	receiver_nick.clear();
	iss >> receiver_nick;
	i = i + receiver_nick.size() + 1;
	
	std::string message = &data_sent[i];
	for (size_t i = 0; i < all_clients.size(); i++)
	{
		if (all_clients[i].getNickname() == receiver_nick)
		{
			Client receiver = all_clients[i];
			std::string	notif(irc_time() + MAGENTA + sender->getNickname() + " : " + message + "\n" + RESET);
			send(receiver.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			return;
		}
	}
	std::string	notif(RED "No one named " + receiver_nick + " was found.\n");
	send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
}


void	Server::msg_to_channel(std::string msg, std::string channel_name, std::vector<Client>::iterator &sender)
{
	std::string message = irc_time() + "[" + channel_name + "] " + BLUE + sender->getNickname() + RESET + ": " + msg + "\n";
	bool found = false;
	for (size_t i = 0; i != all_channels.size(); i++) {
		debug(channel_name);
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

void	Server::cmd_join(std::string data_sent, std::vector<Client>::iterator &sender)
{
	if (data_sent[5] != '#' || data_sent[6] == ' ' || data_sent[6] == '\n')
			throw std::runtime_error(RED "Improper channel name in JOIN command" RESET);

	// Looking if the channel already exists
	std::string	channel_name(&data_sent[6], std::strlen(&data_sent[6]));
	channel_name.erase(std::remove(channel_name.begin(), channel_name.end(), '\n'), channel_name.end());
	for (size_t i = 0; i != all_channels.size(); i++) {
		if (all_channels[i].getName() == channel_name) {
			// Check if user is already in channel
			std::vector<int> users = all_channels[i].getAllUsers();
			if (std::find(users.begin(), users.end(), sender->getSocket()) != users.end())
				throw std::runtime_error("You are already connected to this channel.");
		
			// Joining the channel
			debug("Joining channel");
			all_channels[i].addClientToChannel(sender->getSocket()); // adding client to channel's user list
			sender->addToCurrentChannels(all_channels[i]);			 // adding channel to client's channel list
			sender->setLastInteraction(channel_name);
			// Notifying the client
			std::string	notif = irc_time() + MAGENTA + "You joined a new channel: " + channel_name + '\n' + RESET;
			send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			notif = GREEN " I have arrived." RESET;
			msg_to_channel(notif, channel_name, sender);
			return;
		}
	}

	// Channel does not exists, so we create it
	debug("Creating channel");
	Channel	newChannel(channel_name, sender->getSocket());
	newChannel.addClientToChannel(sender->getSocket()); // adding client to channel's user list
	newChannel.addClientToOperators(sender->getSocket()); // adding client to channel's operators, since he created it
	sender->addToCurrentChannels(newChannel); // adding channel to client's channel list
	all_channels.push_back(newChannel); // pushing newChannel into the all_channels list

	// Notifying the client
	std::string	notif = irc_time() + MAGENTA + "You created a channel named '" + channel_name + "'\n" + RESET
					+ irc_time() + MAGENTA + "You now have ADMIN rights in this channel.\n" + RESET
					+ irc_time() + GREEN + sender->getNickname() + " joined this channel.\n" + RESET;

	send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
}


/////////////////
//  Accessors  //
/////////////////
