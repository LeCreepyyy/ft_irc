#include "../headers/Channel.hpp"

Channel::Channel(std::string name, int op_sockets)
{
	if (name.length() > 199 || name.find('\x07') != std::string::npos || name.find(',') != std::string::npos)
		throw std::runtime_error("[Error] Improper channel name");
	channel_name = name;
	channel_ops.push_back(op_sockets);
}

Channel::~Channel()
{

}


void	Channel::setName(std::string name) {
	if (name.length() > 199 || name.find('\x07') != std::string::npos || name.find(',') != std::string::npos)
		throw std::runtime_error("[Error] Improper channel name");
	else
		channel_name = name;
}

std::string		Channel::getName() {
	return channel_name;
}

void	Channel::setMode(std::string mode) {
	this->channel_mode = mode;
}

std::string		Channel::getMode() {
	return channel_mode;
}

std::vector<int>&	Channel::getChannelOps() {
	return channel_ops;
}

void	Channel::addClientToChannelOps(int client_socket) {
	channel_ops.push_back(client_socket);
}

void	Channel::removeClientFromChannelOps(int client_socket) {
	for (std::vector<int>::iterator it; it != channel_ops.end(); it++) {
		if (*it == client_socket)
			channel_ops.erase(it);
	}
}

std::vector<int>& Channel::getChannelUsers() {
	return channel_users;
}

void	Channel::addClientToChannel(int client_socket) {
	channel_users.push_back(client_socket);
}

void	Channel::removeClientFromChannel(int client_socket) {
	for (std::vector<int>::iterator it; it != channel_users.end(); it++) {
		if (*it == client_socket)
			channel_users.erase(it);
	}
	removeClientFromChannelOps(client_socket);
}
