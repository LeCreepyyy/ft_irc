/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:20:47 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/18 14:04:46 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/**
 * BUG join marche tres mal (ALED)
 * /ping <arg> truc chelou dessus
 * revoir tout les throw
 * Verif via GPT et d'autres server IRC, TOUTES les commmandes que l'on doit refaire :
 *  - finir les messages
 * Rajouter des fonctions pour simplifier la communications entre client et server (pas sur)
*/

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

void Server::start() {
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

	std::cout << irc_time() << "Server listening on port " << serv_port << std::endl;

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
			client.setServName(serv_name);
			client.setSocket(clientSocket);
			client.setIP(inet_ntoa(client.getAddress().sin_addr));
			all_clients.push_back(client);
			std::cout << irc_time() << "New connection" << std::endl;
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
					quit(*iter_client);
					continue;
				}

				// Otherwise, handle the input
				try
				{
					std::string client_input(buffer, bytesReceived);
					std::vector<std::string> substrings = splitString(client_input, '\n');
					
					for (std::vector<std::string>::iterator i = substrings.begin() ; i < substrings.end(); i++) {
						debug(*i);
						handle_client_input(*i, *iter_client);
					}
				}
				catch (const std::exception &e)
				{
					std::string notif = e.what();
					d_send(*iter_client, notif);
				}
			}
			++iter_client;
		}
	}
}

void Server::crash(std::string log) {
	close(serv_socket);
	if (log.length() != 0)
		throw std::runtime_error(log);
}

void Server::quit(Client& iter_client) {
	std::cout << iter_client.getNickname() << " left the game." << std::endl;
	for (std::vector<Client>::iterator i = pass_list.begin(); i != pass_list.end(); ++i) {
	// Remove client from pass_list
		if (*i == iter_client) {
			pass_list.erase(i);
			
			break;
		}
	}

	for (std::vector<Channel>::iterator iter_channel = all_channels.begin(); iter_channel != all_channels.end(); ) {
	// Remove client from all channels, and their whitelists
		iter_channel->removeClientFromWhitelist(iter_client);
		iter_channel->removeClientFromChannel(iter_client);
		iter_channel->removeClientFromOperators(iter_client);
		if (iter_channel->getAllUsers().size() == 0)
			all_channels.erase(std::remove(all_channels.begin(), all_channels.end(), *iter_channel), all_channels.end());
		else
			++iter_channel;
	}

	// Close the client socket and remove the client from all_clients
	all_clients.erase(find(all_clients.begin(), all_clients.end(), iter_client));
	close(iter_client.getSocket());
}

void Server::check_password(std::string data_sent, Client& sender) {
	for (std::vector<Client>::iterator i = pass_list.begin(); i != pass_list.end(); i++) {
		if (*i == sender) {
			std::istringstream iss(data_sent);
			std::string cmd;
			iss >> cmd;
			if (sender.getNickname() == "/" && cmd != "NICK")
				throw std::runtime_error("Choose a Nickname.");
			else if (sender.getNickname() != "/" && sender.getUsername().size() == 0 && cmd != "USER")
				throw std::runtime_error("Choose a Username.");
			return ;
		}
	}
	size_t it = data_sent.find("PASS");
	if (it == data_sent.npos) {
		d_send(sender, "Wrong password !");
		quit(sender);
	}
	it += 5;
	std::istringstream iss(&data_sent[it]);
	std::string password_sent;
	iss >> password_sent;
	if (password_sent != this->serv_password) {
		d_send(sender, "Wrong password !");
		quit(sender);
	}
	//validation
	pass_list.push_back(sender);
	std::string notif("You are now logged in.\n");
	d_send(sender, notif);
	std::cout << sender.getIP() << " logged in." << std::endl;
}


void Server::handle_client_input(std::string data_sent, Client& sender)
{
	std::istringstream iss(data_sent);
	std::string command;
	iss >> command;

	if (command == "CAP")
		return;
	check_password(data_sent, sender);
	if (command == "NICK") {
		std::string lastname = sender.getNickname();
		sender.setNickname(data_sent, all_clients);
		std::vector<Channel> lastiter = sender.getAllInteractions();
		for (std::vector<Channel>::iterator it = lastiter.begin(); it != lastiter.end(); it++)
			msg_to_channel(lastname + " is now known as: " + sender.getNickname(), *it, sender);
	}
	else if (command == "USER")
		sender.setUsername(data_sent);
	else if (command == "JOIN")
		cmd_join(data_sent, sender);
	else if (command == "QUIT")
		cmd_quit(data_sent, sender);
	else if (command == "PRIVMSG")
		cmd_privmsg(data_sent, sender);
	else if (command == "PART")
		cmd_part(data_sent, sender);
	else if (command == "KICK")
		cmd_kick(data_sent, sender);
	else if (command == "MODE")
		cmd_mode(data_sent, sender);
	else if (command == "INVITE")
		cmd_invite(data_sent, sender);
	else if (command == "TOPIC")
		cmd_topic(data_sent, sender);
	else if (command == "HELP")
		cmd_help(sender);
	else if (command == "PING")
		cmd_ping(data_sent, sender);
	else if (command == "WHO")
		cmd_who(data_sent, sender);
	else if (command != "PASS" && command != "CAP") {
		if (sender.getAllInteractions().size())
			msg_to_channel(data_sent, sender.getLastInteraction(), sender);
		else
			throw std::runtime_error(ERR_UNKNOWERROR(serv_name, sender.getNickname(), "Unrecognized command. Try using 'HELP'"));
	}
}


void	Server::msg_to_channel(std::string msg, Channel target, Client& sender) {
	std::istringstream iss(msg);
	std::string tmp;
	iss >> tmp;
	if (tmp.empty())
		return;
	
	std::string message = RPL_PRIVMSG(sender.getNickname(), sender.getUsername()[0], serv_name, "#" + target.getName(), msg);
	bool found = false;
	for (size_t i = 0; i != all_channels.size(); i++) {
		if (all_channels[i].getName() == target.getName() || all_channels[i].getName() == target.getName() + "\n") {
			found = true;
			for (size_t j = 0; j != all_channels[i].getAllUsers().size(); j++) {
				if (all_channels[i].getAllUsers()[j].getSocket() != sender.getSocket())
					d_send(all_channels[i].getAllUsers()[j], message);
			}
			break ;
		}
	}
	if (found == false)
		throw std::runtime_error(ERR_NOSUCHCHANNEL(serv_name, sender.getNickname(), target.getName()));
}

void	Server::cmd_to_channel(std::string msg, Channel target, Client& sender) {
	bool found = false;
	for (size_t i = 0; i != all_channels.size(); i++) {
		if (all_channels[i].getName() == target.getName() || all_channels[i].getName() == target.getName() + "\n") {
			found = true;
			for (size_t j = 0; j != all_channels[i].getAllUsers().size(); j++) {
				if (all_channels[i].getAllUsers()[j].getSocket() != sender.getSocket())
					d_send(all_channels[i].getAllUsers()[j], msg);
			}
			break ;
		}
	}
	if (found == false)
		throw std::runtime_error(ERR_NOSUCHCHANNEL(serv_name, sender.getNickname(), target.getName()));
}

/////////////////
//  Accessors  //
/////////////////

Channel&		Server::getChannel(std::string channel_name, Client& sender) {
	if (channel_name.size() == 0)
		throw std::runtime_error(ERR_NEEDMOREPARAMS(serv_name, sender.getNickname()));
	if (channel_name[0] != '#')
		throw std::runtime_error(ERR_NOSUCHCHANNEL(serv_name, sender.getNickname(), channel_name));
	for (std::vector<Channel>::iterator channel_it = all_channels.begin(); channel_it != all_channels.end(); channel_it++) {
		if (channel_it->getName() == &channel_name[1])
			return (*channel_it);
	}
	throw std::runtime_error(ERR_NOSUCHCHANNEL(serv_name, sender.getNickname(), channel_name));
}

Client&			Server::getClient(std::string client_name, Client& sender) {
	if (client_name.size() == 0)
		throw std::runtime_error(ERR_NEEDMOREPARAMS(serv_name, sender.getNickname()));
	for (std::vector<Client>::iterator client_it = all_clients.begin(); client_it != all_clients.end(); client_it++) {
		if (client_it->getNickname() == client_name)
			return (*client_it);
	}
	throw std::runtime_error(ERR_NOSUCHNICK(serv_name, sender.getNickname(), client_name));
}


