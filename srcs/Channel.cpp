/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:42:29 by bgaertne          #+#    #+#             */
/*   Updated: 2024/05/27 14:19:22 by vpoirot          ###   ########.fr       */
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
	this->on_whitelist = false;
	this->topic_restricted = false;
	this->password_protected = false;
	this->name = name;
	this->topic = "Undefinied channel topic.\n";
	this->all_operators.push_back(op_socket);
}

Channel::~Channel()
{

}



///////////////
//  Methods  //
///////////////

bool Channel::operator==(const Channel &other) const {
	return this->name == other.name;
}

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


void	Channel::setTopic(std::string newTopic, int client_socket) {
	bool is_op = false;
	for (std::vector<int>::iterator it = all_operators.begin(); it != all_operators.end(); it++) {
		if (*it == client_socket)
			is_op = true;
	}
	if (this->topic_restricted == true && is_op != true)
		throw std::runtime_error("Channel #" + this->name + ": topic may only be modified by operators.");
	if (find(this->all_users.begin(), this->all_users.end(), client_socket) == this->all_users.end())
		throw std::runtime_error("You need to be connected to this channel.");
	this->topic = newTopic + '\n';
}
std::string		Channel::getTopic() {
	return this->topic;
}
void	Channel::setTopicLimit(bool status, int client_socket) {
	if (this->topic_restricted == true) {
		if (status == true)
			throw std::runtime_error("Channel #" + this->name + ": topic is already limited to operators modification.");
		else {
			this->topic_restricted = false;
			std::string notif = irc_time() + MAGENTA "Channel #" + this->name + ": topic may now be changed by everyone.\n" RESET;
			send(client_socket, notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
	}
	else {
		if (status == false)
			throw std::runtime_error("Channel #" + this->name + ": topic is already publicly modifiable.");
		else {
			this->topic_restricted = true;
			std::string notif = irc_time() + MAGENTA "Channel #" + this->name + ": topic modification is now limited to operators.\n" RESET;
			send(client_socket, notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
	}
}


std::vector<int>&	Channel::getAllOperators() {
	return this->all_operators;
}
void	Channel::addClientToOperators(int client_socket) {
	this->all_operators.push_back(client_socket);
}
void	Channel::removeClientFromOperators(int client_socket) {
	std::vector<int>::iterator new_end = std::remove(all_operators.begin(), all_operators.end(), client_socket);
	all_operators.erase(new_end, all_operators.end());
}


std::vector<int>& Channel::getAllUsers() {
	return this->all_users;
}
void	Channel::addClientToChannel(int client_socket) {
	this->all_users.push_back(client_socket);
}
void	Channel::removeClientFromChannel(int client_socket) {
	removeClientFromOperators(client_socket);	
	std::vector<int>::iterator new_end = std::remove(all_users.begin(), all_users.end(), client_socket);
	all_users.erase(new_end, all_users.end());
}


void	Channel::setWhitelist(bool status, int client_socket)
{
	if (this->on_whitelist == true)
	{
		if (status == true)
			throw std::runtime_error("Channel #" + this->name + ": already invite-only.");
		else {
			this->whitelist.clear();
			this->on_whitelist = false;
			std::string notif = irc_time() + MAGENTA "Channel #" + this->name + ": public.\n" RESET;
			send(client_socket, notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
	}
	else
	{
		if (status == false)
			throw std::runtime_error("Channel #" + this->name + ": already public.");
		else {
			this->whitelist.push_back(client_socket);
			this->on_whitelist = true;
			std::string notif = irc_time() + MAGENTA "Channel #" + this->name + ": invite-only.\n" RESET;
			send(client_socket, notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
	}
}
std::vector<int>	Channel::getWhitelist()
{
	return this->whitelist;
}


bool	Channel::addToWhitelist(int client_socket) {
	for (std::vector<int>::iterator it = whitelist.begin(); it != whitelist.end(); it++) {
		if (*it == client_socket)
			return (false);
	}
	whitelist.push_back(client_socket);
	return (true);
}
bool	Channel::removeToWhitelist(int client_socket) {
	for (std::vector<int>::iterator it = whitelist.begin(); it != whitelist.end(); it++) {
		if (*it == client_socket) {
			whitelist.erase(it);
			return (true);
		}
	}
	return (false);
}
bool	Channel::getWhitelistStatus() {
	return (on_whitelist);
}
