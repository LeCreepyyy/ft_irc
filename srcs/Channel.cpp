/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:42:29 by bgaertne          #+#    #+#             */
/*   Updated: 2024/07/02 10:20:40 by bgaertne         ###   ########.fr       */
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

bool Channel::operator!=(const Channel &other) const {
	return this->name == other.name;
}

bool Channel::operator!=(const size_t size) const {
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

// MODE
std::string	Channel::getModes() {
	std::string mode_list = "+";
	if (on_whitelist)
		mode_list += "i";
	if (topic_restricted)
		mode_list += "t";
	if (user_limit > 0)
		mode_list += "l";
	if (password_protected)
		mode_list += "k";
	return (mode_list);
}

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
		throw std::runtime_error(ERR_CHANOPRIVSNEEDED(sender.getServName(), sender.getNickname(), name));
	this->topic = newTopic;
}

std::string		Channel::getTopic() {
	return this->topic;
}

void	Channel::setTopicRestriction(bool status, Client& sender) {
	if (this->topic_restricted == true) {
		if (status == false) {
			this->topic_restricted = false;
			std::string notif = RPL_SETMODE(sender.getNickname(), sender.getUsername()[1], name, "-t");
			d_send(sender, notif);
		}
	}
	else {
		if (status == true) {
			this->topic_restricted = true;
			std::string notif = RPL_SETMODE(sender.getNickname(), sender.getUsername()[1], name, "+t");
			d_send(sender, notif);
		}
	}
}

bool	Channel::getTopicRestriction() {
	return this->topic_restricted;
}

std::string Channel::rplTopic(Client& sender) {
	if(topic.size() == 0)
		return (RPL_NOTOPIC(sender.getServName(), sender.getNickname(), "#" + name));
	return (RPL_TOPIC(sender.getNickname(), sender.getUsername()[0], sender.getUsername()[1], name, topic));
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
		if (isUserOp(sender)) {
			this->addClientToOperators(target);
			notif = RPL_SETMODE(sender.getNickname(), sender.getUsername()[1], name, "+o");
			d_send(sender, notif);
		}
	}
	else
	{
		if (isUserOp(sender)) {
			this->removeClientFromOperators(target);
			notif = RPL_SETMODE(sender.getNickname(), sender.getUsername()[1], name, "-o");
			d_send(sender, notif);
		}
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
		if (status == false) {
			this->whitelist.clear();
			this->on_whitelist = false;
			std::string notif = RPL_SETMODE(sender.getNickname(), sender.getUsername()[1], name, "-i");
			d_send(sender, notif);
		}
	}
	else
	{
		if (status == true) {
			this->whitelist.push_back(sender);
			this->on_whitelist = true;
			std::string notif = RPL_SETMODE(sender.getNickname(), sender.getUsername()[1], name, "+i");
			d_send(sender, notif);
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
				throw std::runtime_error(ERR_UNKNOWERROR(sender.getServName(), sender.getNickname(), "Invalid password."));
			this->password = password;
			std::string notif = RPL_SETMODE(sender.getNickname(), sender.getUsername()[1], name, "+k");
			d_send(sender, notif);
		}
		else {
			this->password_protected = false;
			this->password = "/";
			std::string notif = RPL_SETMODE(sender.getNickname(), sender.getUsername()[1], name, "-k");
			d_send(sender, notif);
		}
	}
	else {
		if (status == true) {
			if (password.empty())
				throw std::runtime_error(ERR_UNKNOWERROR(sender.getServName(), sender.getNickname(), "Invalid password."));
			this->password_protected = true;
			this->password = password;
			std::string notif = RPL_SETMODE(sender.getNickname(), sender.getUsername()[1], name, "+k");
			d_send(sender, notif);
		}
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

void	Channel::setUserLimit(int limit, std::string arg, Client& sender) {
	std::string option = "-l";
	if (limit == -1)
		throw std::runtime_error(RPL_CHANNELMODEIS(sender.getServName(), sender.getNickname(), name, option));
	else if (limit <= 0)
		throw std::runtime_error(ERR_NEEDMOREPARAMS(sender.getServName(), sender.getNickname()));
	this->user_limit = limit;
	option = "+l " + arg;
	throw std::runtime_error(RPL_CHANNELMODEIS(sender.getServName(), sender.getNickname(), name, option));
}

// LIST OF NAMES
std::string			Channel::getListOfNames() {
	std::vector<Client> users = all_users;
	std::vector<Client> ops = all_operators;
	std::string names;
	
	for (std::vector<Client>::iterator ops_it = ops.begin(); ops_it != ops.end(); ops_it++) {
		for (std::vector<Client>::iterator users_it = users.begin(); users_it != users.end(); ) {
			if (*users_it == *ops_it) {
				users_it = users.erase(users_it);
				break;
			}
			else
				++users_it;
		}
	}

	for (std::vector<Client>::iterator ops_it = ops.begin(); ops_it != ops.end(); ops_it++)
		names += "@" + ops_it->getNickname() + " ";
	for (std::vector<Client>::iterator users_it = users.begin(); users_it != users.end(); users_it++)
		names += users_it->getNickname() + " ";
	return (names);
}

bool	Channel::isOP(Client& client) {
	for (std::vector<Client>::iterator it = all_operators.begin(); it != all_operators.end(); it++)
		if (client == *it)
			return true;
	return false;
}