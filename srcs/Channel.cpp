/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:42:29 by bgaertne          #+#    #+#             */
/*   Updated: 2024/05/14 13:48:39 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Channel.hpp"

////////////////////////////////
//  Constructors, Destructor  //
////////////////////////////////

Channel::Channel(std::string name, int op_socket)
{
	if (name.length() > 199 || name.find('\x07') != std::string::npos || name.find(',') != std::string::npos)
		throw std::runtime_error("[Error] Improper channel name");
	this->name = name;
	this->all_operators.push_back(op_socket);
}

Channel::~Channel()
{

}



///////////////
//  Methods  //
///////////////



/////////////////
//  Accessors  //
/////////////////

void	Channel::setName(std::string name) {
	if (name.length() > 199 || name.find('\x07') != std::string::npos || name.find(',') != std::string::npos)
		throw std::runtime_error("[Error] Improper channel name");
	else
		this->name = name;
}
std::string		Channel::getName() {
	return this->name;
}


void	Channel::setMode(std::string mode) {
	this->mode = mode;
}
std::string		Channel::getMode() {
	return this->mode;
}


std::vector<int>&	Channel::getAllOperators() {
	return this->all_operators;
}
void	Channel::addClientToOperators(int client_socket) {
	this->all_operators.push_back(client_socket);
}
void	Channel::removeClientFromOperators(int client_socket) {
	for (std::vector<int>::iterator it = all_operators.begin(); it != this->all_operators.end(); it++) {
		if (*it == client_socket)
			this->all_operators.erase(it);
	}
}



std::vector<int>& Channel::getAllUsers() {
	return this->all_users;
}
void	Channel::addClientToChannel(int client_socket) {
	this->all_users.push_back(client_socket);
}
void	Channel::removeClientFromChannel(int client_socket) {
	for (std::vector<int>::iterator it = all_users.begin(); it != this->all_users.end(); it++) {
		if (*it == client_socket)
			this->all_users.erase(it);
	}
	removeClientFromOperators(client_socket);
}
