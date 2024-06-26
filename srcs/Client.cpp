/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:09:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/28 10:39:51 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"

////////////////////////////////
//  Constructors, Destructor  //
////////////////////////////////

Client::Client()
{
	nickname = "/";
	registered = false;
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

bool	Client::operator!=(const Client& other) const {
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

void	Client::setRegistered(bool status) {
	this->registered = status;
}
bool	Client::getRegistered() {
	return this->registered;
}

// Nickname
void					Client::setServName(std::string servname) {
	serv_name = servname;
}
std::string				Client::getServName() {
	return serv_name;
}


std::string	Client::setNickname(std::string cmd, std::vector<Client> &all_clients) {
	std::string last = nickname;
	size_t start = 5;
    if (cmd.size() <= start)
        throw std::runtime_error(ERR_NONICKNAMEGIVEN(serv_name, last));


    size_t end = cmd.find_first_of(" \r\n", start);
    if (end == std::string::npos)
        end = cmd.length();

    std::string temp = cmd.substr(start, end - start);
    if (temp.empty())
        throw std::runtime_error(ERR_NONICKNAMEGIVEN(serv_name, nickname));

    std::string invalidChars = " ,*?!@./";
    if (temp[0] == '#' || temp.find_first_of(invalidChars) != std::string::npos)
    	throw std::runtime_error(ERR_ERRONEUSNICKNAME(serv_name, last, temp));

    for (std::vector<Client>::iterator it = all_clients.begin(); it != all_clients.end(); it++) {
        if (it->nickname == temp)
            throw std::runtime_error(ERR_NICKNAMEINUSE(serv_name, last, temp));
    }
    nickname = temp;
	if (this->registered == false && this->nickname != "/" && this->username.size() > 0) {
		this->registered = true;
		std::string notif = RPL_WELCOME(serv_name, this->nickname) + RPL_YOURHOSTIS(serv_name, this->nickname) + RPL_CREATIONDATE(serv_name, this->nickname) + RPL_SERVINFOS(serv_name, this->nickname);
		d_send(*this, notif);
	}
	return (last);
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
	if (getUsername().size() > 0)
		throw std::runtime_error(ERR_ALREADYREGISTERED(serv_name , nickname));
	for (size_t it = 0; prompt[it] || it < prompt.size(); it += word_size) {
		std::istringstream iss(&prompt[it]);
		std::string next_word;
		iss >> next_word;
		word_size = next_word.size();
		word_size++;
		temp.push_back(next_word);
		args_count++;
	}
	if (args_count < 3)
		throw std::runtime_error(ERR_NEEDMOREPARAMS(serv_name, nickname + " USER"));
	username = temp;
	if (this->registered == false && this->nickname != "/" && this->username.size() > 0) {
		this->registered = true;
		std::string notif = RPL_WELCOME(serv_name, this->nickname) + RPL_YOURHOSTIS(serv_name, this->nickname) + RPL_CREATIONDATE(serv_name, this->nickname) + RPL_SERVINFOS(serv_name, this->nickname);
		d_send(*this, notif);
	}
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
		throw std::runtime_error(ERR_NOSUCHCHANNEL(serv_name, nickname, "No channel interactions"));
	return (last_interaction.back());
}

std::vector<Channel> Client::getAllInteractions() {
	return (last_interaction);
}

void	Client::removeFromLastInteraction(Channel& target) {
	std::vector<Channel>::iterator it = std::remove(last_interaction.begin(), last_interaction.end(), target);
	last_interaction.erase(it, last_interaction.end());
}

