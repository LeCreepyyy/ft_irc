/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:42:29 by bgaertne          #+#    #+#             */
/*   Updated: 2024/06/07 11:50:23 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Channel.hpp"

////////////////////////////////
//  Constructors, Destructor  //
////////////////////////////////

Channel::Channel() {
	;
}

Channel::Channel(std::string name, Client oper)
{
	if (name.length() > 199 || name.find('\x07') != std::string::npos || name.find(',') != std::string::npos)
		throw std::runtime_error("[Error] Improper channel name");
	this->on_whitelist = false;
	this->topic_restricted = false;
	this->password_protected = false;
	this->name = name;
	this->user_limit = -1;
	this->topic = "Undefinied channel topic.\n";
	this->all_operators.push_back(oper);
}

Channel::~Channel()
{

}



///////////////
//  Methods  //
///////////////

// OPERATORS OVERRIDE
bool Channel::operator==(const Channel &other) const {
	return this->name == other.name;
}

bool Channel::operator==(const size_t size) const {
	return (this->all_users.size() == size);
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		this->name = other.name;
		this->all_operators = other.all_operators;
		this->all_users = other.all_users;
		this->on_whitelist = other.on_whitelist;
		this->whitelist = other.whitelist;
		this->topic_restricted = other.topic_restricted;
		this->topic = other.topic;
		this->password_protected = other.password_protected;
		this->password = other.password;
		this->user_limit = other.user_limit;
	}
	return *this;
}


/////////////////
//  Accessors  //
/////////////////

// NAME
void	Channel::setName(std::string name) {
	if (name.length() > 199 || name.find('\x07') != std::string::npos || name.find(',') != std::string::npos)
		throw std::runtime_error("[Error] Improper channel name");
	else
		this->name = name;
}

std::string		Channel::getName() {
	return this->name;
}




// TOPIC
void	Channel::setTopic(std::string newTopic, Client& sender) {
	if (this->topic_restricted == true && !isUserOp(sender))
		throw std::runtime_error("Channel #" + this->name + ": topic may only be modified by operators.");
	if (find(this->all_users.begin(), this->all_users.end(), sender) == this->all_users.end())
		throw std::runtime_error("You need to be connected to this channel.");
	this->topic = newTopic + '\n';
}

std::string		Channel::getTopic() {
	return this->topic;
}

void	Channel::setTopicRestriction(bool status, Client& sender) {
	if (this->topic_restricted == true) {
		if (status == true)
			throw std::runtime_error("Channel #" + this->name + ": topic is already limited to operators modification.");
		else {
			this->topic_restricted = false;
			std::string notif = irc_time() + "Channel #" + this->name + ": topic may now be changed by everyone.\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
	}
	else {
		if (status == false)
			throw std::runtime_error("Channel #" + this->name + ": topic is already publicly modifiable.");
		else {
			this->topic_restricted = true;
			std::string notif = irc_time() + "Channel #" + this->name + ": topic modification is now limited to operators.\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
	}
}

bool	Channel::getTopicRestriction() {
	return this->topic_restricted;
}




// USER OPERATORS
std::vector<Client>&	Channel::getAllOperators() {
	return this->all_operators;
}

void	Channel::addClientToOperators(Client& target) {
	this->all_operators.push_back(target);
}

void	Channel::removeClientFromOperators(Client& client) {
	std::vector<Client>::iterator new_end = std::remove(all_operators.begin(), all_operators.end(), client);
	all_operators.erase(new_end, all_operators.end());
}

void	Channel::opUser(bool status, Client& target, Client& sender) {
	std::string notif;
	if (status == true) {
		if (!isUserOp(sender)) {
			this->addClientToOperators(target);
			notif = "User got promoted to channel operator.\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
		else
			throw std::runtime_error("User is already operator in this channel.");
	}
	else
	{
		if (isUserOp(sender)) {
			this->removeClientFromOperators(target);
			notif = "User is no longer operator in this channel.\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
		else
			throw std::runtime_error("User is not operator in this channel.");
	}
}
bool	Channel::isUserOp(Client& target) {
	if (find(this->all_operators.begin(), this->all_operators.end(), target) != this->all_operators.end())
		return true;
	return false;
}




// USERS
std::vector<Client>& Channel::getAllUsers() {
	return this->all_users;
}

void	Channel::addClientToChannel(Client& target) {
	this->all_users.push_back(target);
}

void	Channel::removeClientFromChannel(Client& target) {
	std::vector<Client>::iterator new_end = std::remove(all_users.begin(), all_users.end(), target);
	all_users.erase(new_end, all_users.end());
}
bool	Channel::isUserInChannel(Client& target) {
	if (find(this->all_users.begin(), this->all_users.end(), target) != this->all_users.end())
			return true;
		return false;
}




// WHITELIST
void	Channel::setWhitelist(bool status, Client& sender) {
	if (this->on_whitelist == true)
	{
		if (status == true)
			throw std::runtime_error("Channel #" + this->name + ": already invite-only.");
		else {
			this->whitelist.clear();
			this->on_whitelist = false;
			std::string notif = irc_time() + "Channel #" + this->name + ": public.\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
	}
	else
	{
		if (status == false)
			throw std::runtime_error("Channel #" + this->name + ": already public.");
		else {
			this->whitelist.push_back(sender);
			this->on_whitelist = true;
			std::string notif = irc_time() + "Channel #" + this->name + ": invite-only.\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
	}
}

std::vector<Client>	Channel::getWhitelist() {
	return this->whitelist;
}
bool	Channel::addClientToWhitelist(Client& target) {
	for (std::vector<Client>::iterator it = whitelist.begin(); it != whitelist.end(); it++) {
		if (*it == target)
			return (false);
	}
	whitelist.push_back(target);
	return (true);
}

bool	Channel::removeClientFromWhitelist(Client& target) {
	for (std::vector<Client>::iterator it = whitelist.begin(); it != whitelist.end(); it++) {
		if (*it == target) {
			whitelist.erase(it);
			return (true);
		}
	}
	return (false);
}
bool	Channel::getWhitelistStatus() {
	return (on_whitelist);
}

bool	Channel::isUserOnWhitelist(Client& target) {
	if (find(this->whitelist.begin(), this->whitelist.end(), target) != this->whitelist.end())
			return true;
		return false;
}




// PASSWORD
void	Channel::setPassword(bool status, std::string password, Client& sender) {
	if (this->password_protected == true) {
		if (status == true) {
			if (password.empty())
				throw std::runtime_error("Invalid password.");
			this->password = password;
			std::string notif = "Password updated.\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
		else {
			this->password_protected = false;
			this->password = "/";
			std::string notif = "Channel is no longer protected by a password.\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
	}
	else {
		if (status == true) {
			if (password.empty())
				throw std::runtime_error("Invalid password.");
			this->password_protected = true;
			this->password = password;
			std::string notif = "Channel is now protected by password.\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
		}
		else
			throw std::runtime_error("Channel is not protected by password.");
	}
}

std::string		Channel::getPassword() {
	return this->password;
}

bool	Channel::getPasswordStatus() {
	return this->password_protected;
}




// USER LIMIT
int		Channel::getUserLimit() {
	return this->user_limit;
}

void	Channel::setUserLimit(int limit) {
	this->user_limit = limit;
}