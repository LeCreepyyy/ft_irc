/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:09:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/07 11:53:16 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"

////////////////////////////////
//  Constructors, Destructor  //
////////////////////////////////

Client::Client()
{
	static int Usercode = 1000;
	std::ostringstream oss;
	oss << Usercode;
	std::string code = oss.str();
	nickname = "/";
	username = "User" + code;
	Usercode++;
}


Client::~Client()
{
	;
}



///////////////
//  Methods  //
///////////////



/////////////////
//  Accessors  //
/////////////////

void	Client::setSocket(int socket) {
	client_socket = socket;
}
int	Client::getSocket() {
	return (client_socket);
}


void	Client::setNickname(std::string cmd) {
	size_t start = 5;
	if (start == std::string::npos)
		nickname = "/";
	size_t end = cmd.find_first_of(" \n", start);
	if (end == std::string::npos)
		end = cmd.length();
	nickname = cmd.substr(start, end - start);
}
std::string	Client::getNickname() {
	return (nickname);
}


void	Client::setUsername(std::string cmd) {
	size_t start = 5;
	if (start == std::string::npos)
		username = "/";
	size_t end = cmd.find_first_of(" \n", start);
	if (end == std::string::npos)
		end = cmd.length();
	username = cmd.substr(start, end - start);
}
std::string	Client::getUsername() {
	return (username);
}


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


void Client::setIP(std::string x) {
	ip = x;
}
std::string Client::getIP() {
	return (ip);
}


std::vector<Channel>	Client::getCurrentChannels() {
	return current_channels;
}
void	Client::addToCurrentChannels(Channel channel) {
	current_channels.push_back(channel);
}


void	Client::setLastInteraction(std::string channel_name) {
	this->last_interaction = channel_name;
}

std::string Client::getLastInteraction() {
	return this->last_interaction;
}