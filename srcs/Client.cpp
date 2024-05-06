/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:09:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/06 16:56:51 by bgaertne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"

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

// accessor Client Socket

void Client::setClientSocket(int socket)
{
	client_socket = socket;
}

int Client::getSocket()
{
	return (client_socket);
}

// accessor Client Nickname

void Client::setNickname(std::string cmd)
{
	size_t start = 5;
	
	if (start == std::string::npos)
		nickname = "/";
	size_t end = cmd.find_first_of(" \n", start);
	if (end == std::string::npos)
		end = cmd.length();
	nickname = cmd.substr(start, end - start);
}

std::string Client::getNickname()
{
	return (nickname);
}

// accessor Client Username

void Client::setUsername(std::string cmd)
{
	size_t start = 5;

	if (start == std::string::npos)
		username = "/";
	size_t end = cmd.find_first_of(" \n", start);
	if (end == std::string::npos)
		end = cmd.length();
	username = cmd.substr(start, end - start);
}

std::string Client::getUsername()
{
	return (username);
}

// accessor Client Address

void Client::setAddress(struct sockaddr_in addr)
{
	client_address = addr;
}

struct sockaddr_in Client::getAddress()
{
	return (client_address);
}

struct sockaddr_in &Client::getAddressREF()
{
	return (this->client_address);
}

// accessor Client Address Len

void Client::setAddressLen(socklen_t addr_len)
{
	client_address_len = addr_len;
}

socklen_t Client::getAddressLen()
{
	return (client_address_len);
}

socklen_t &Client::getAddressLenREF()
{
	return (client_address_len);
}

// accessor Client IP

std::string Client::getIP()
{
	return (ip);
}

void Client::setIP(std::string x)
{
	ip = x;
}

std::vector<Channel>	Client::get_current_channels() {
	return current_channels;
}

void	Client::addToCurrentChannels(Channel channel) {
	current_channels.push_back(channel);
}
