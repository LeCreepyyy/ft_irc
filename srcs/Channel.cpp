#include "../headers/Channel.hpp"

Channel::Channel(std::string name)
{
	if (name.length() > 199 || name.find('\x07') != std::string::npos || name.find(',') != std::string::npos)
		throw std::runtime_error("[Error] Improper channel name");
	channel_name = name;
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
	;
}

std::string		Channel::getMode() {
	return channel_mode;
}

std::list<int>&	Channel::getChannelOps() {
	return channel_ops
}

void	Channel::addToChannelOps(int socket_to_op) {
	channel_ops.pushback(socket_to_op);
}