/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:09:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/30 12:53:04 by bgaertne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"

////////////////////////////////
//  Constructors, Destructor  //
////////////////////////////////

Client::Client()
{
	nickname = "/";
}


Client::~Client()
{
	;
}



///////////////
//  Methods  //
///////////////

bool	Client::operator==(const Client& other) const {
	return(this->client_socket == other.client_socket);
}



/////////////////
//  Accessors  //
/////////////////

// Socket
void	Client::setSocket(int socket) {
	client_socket = socket;
}

int	Client::getSocket() {
	return (client_socket);
}




// Nickname
void	Client::setNickname(std::string cmd, std::vector<Client> &all_clients) {
	size_t start = 5;
	if (!cmd[start])
		throw std::runtime_error("Nickname cannot be empty.");
	size_t end = cmd.find_first_of(" \n", start);
	if (end == std::string::npos)
		end = cmd.length();
	std::string temp = cmd.substr(start, end - start);
	if (temp.size() < 1)
		throw std::runtime_error("Nickname cannot be empty.");
	for (std::vector<Client>::iterator it = all_clients.begin(); it != all_clients.end(); it++)
	{
		if (it->nickname == temp)
			throw std::runtime_error("Nickname already in use. Try another one.");
	}
	if (temp[0] == '#')
		throw std::runtime_error("\"#\" is forbidden.");
	nickname = temp;
	std::string notif(GREEN "You are now known as " + nickname + ".\n" RESET);
	send(client_socket, notif.c_str(), strlen(notif.c_str()), MSG_DONTWAIT);
}

std::string	Client::getNickname() {
	return (nickname);
}




// Username
void	Client::setUsername(std::string cmd) {
	std::vector<std::string> temp;
	std::string prompt = &cmd[4];
	size_t	word_size = 0;
	int		args_count = 0;
	for (size_t it = 0; prompt[it] || it < prompt.size(); it += word_size) {
		std::istringstream iss(&prompt[it]);
		std::string next_word;
		iss >> next_word;
		word_size = next_word.size();
		word_size++;
		temp.push_back(next_word);
		args_count++;
	}
	if (args_count != 5)
		throw std::runtime_error("USER <username> <hostname> <servername> <realname>");
	username = temp;
	std::string notif(GREEN "Username has been set.\n" RESET);
	send(client_socket, notif.c_str(), strlen(notif.c_str()), MSG_DONTWAIT);
}

std::vector<std::string>	Client::getUsername() {
	return (username);
}




// Address & AddressLen
void	Client::setAddress(struct sockaddr_in addr) {
	client_address = addr;
}

struct sockaddr_in	Client::getAddress() {
	return (client_address);
}

struct sockaddr_in &Client::getAddressREF() {
	return (this->client_address);
}

void Client::setAddressLen(socklen_t addr_len) {
	client_address_len = addr_len;
}
socklen_t Client::getAddressLen() {
	return (client_address_len);
}
socklen_t &Client::getAddressLenREF() {
	return (client_address_len);
}




// IP
void Client::setIP(std::string x) {
	ip = x;
}

std::string Client::getIP() {
	return (ip);
}




// Last Interaction
void	Client::setLastInteraction(Channel& target) {
	for (std::vector<Channel>::iterator it = last_interaction.begin(); it != last_interaction.end(); it++) {
		if (*it == target)
			last_interaction.erase(it);
	}
	last_interaction.push_back(target);
}

Channel& Client::getLastInteraction() {
	if (last_interaction.size() == 0)
		throw std::runtime_error("You have had no interaction with any channel. Please express the target.");
	return (last_interaction.back());
}

std::vector<Channel> Client::getAllInteractions() {
	return (last_interaction);
}

void	Client::removeFromLastInteraction(Channel& target) {
	std::vector<Channel>::iterator it = std::remove(last_interaction.begin(), last_interaction.end(), target);
	last_interaction.erase(it, last_interaction.end());
}