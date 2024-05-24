/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 10:23:28 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/24 10:25:21 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

void	Server::cmd_join(std::string data_sent, std::vector<Client>::iterator &sender)
{
	if (data_sent[5] != '#' || data_sent[6] == ' ' || data_sent[6] == '\n')
			throw std::runtime_error("Improper channel name in JOIN command");

	// Looking if the channel already exists
	std::string	channel_name(&data_sent[6], std::strlen(&data_sent[6]));
	channel_name.erase(std::remove(channel_name.begin(), channel_name.end(), '\n'), channel_name.end());
	for (size_t i = 0; i != all_channels.size(); i++) {
		if (all_channels[i].getName() == channel_name) {
			// Check if user is already in channel
			std::vector<int> users = all_channels[i].getAllUsers();
			if (std::find(users.begin(), users.end(), sender->getSocket()) != users.end())
				throw std::runtime_error("You are already connected to this channel.");
		
			// Joining the channel
			all_channels[i].addClientToChannel(sender->getSocket()); // adding client to channel's user list
			sender->addToCurrentChannels(all_channels[i]);			 // adding channel to client's channel list
			sender->setLastInteraction(channel_name);
			// Notifying the client
			std::string	notif = irc_time() + MAGENTA + "You joined a new channel: " + channel_name + '\n' + RESET;
			send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			notif = GREEN " I have arrived." RESET;
			msg_to_channel(notif, channel_name, sender);
			return;
		}
	}

	// Channel does not exists, so we create it
	Channel	newChannel(channel_name, sender->getSocket());
	newChannel.addClientToChannel(sender->getSocket()); // adding client to channel's user list
	sender->addToCurrentChannels(newChannel); // adding channel to client's channel list
	all_channels.push_back(newChannel); // pushing newChannel into the all_channels list

	// Notifying the client
	std::string	notif = irc_time() + MAGENTA + "You created a channel named '" + channel_name + "'\n" + RESET
					+ irc_time() + MAGENTA + "You now have ADMIN rights in this channel.\n" + RESET
					+ irc_time() + GREEN + sender->getNickname() + " joined this channel.\n" + RESET;
	sender->setLastInteraction(channel_name);
	send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
}


void	Server::cmd_privmsg(std::string data_sent, std::vector<Client>::iterator &sender)
{
	std::istringstream	iss(data_sent);
	std::string			receiver_nick;
	int i = 0;
	
	iss >> receiver_nick;
	i = i + receiver_nick.size() + 1;
	receiver_nick.clear();
	iss >> receiver_nick;
	i = i + receiver_nick.size() + 1;
	
	std::string message = &data_sent[i];
	for (size_t i = 0; i < all_clients.size(); i++)
	{
		if (all_clients[i].getNickname() == receiver_nick)
		{
			Client receiver = all_clients[i];
			std::string	notif(irc_time() + MAGENTA + sender->getNickname() + " : " + message + "\n" + RESET);
			send(receiver.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			return;
		}
	}
	std::string	notif("No one named " + receiver_nick + " was found.\n");
	send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
}


void	Server::cmd_msg(std::string data_sent, std::vector<Client>::iterator &sender) {
	std::istringstream iss(&data_sent[4]);
	std::string channel_name;
	iss >> channel_name;
	std::vector<Channel> channel_client = sender->getCurrentChannels();
	for (std::vector<Channel>::iterator it = channel_client.begin(); it != channel_client.end(); it++) {
		if (it->getName() == channel_name) {
			msg_to_channel(&data_sent[channel_name.size() + 5], channel_name, sender);
			sender->setLastInteraction(channel_name);
			return ;
		}
	}
	throw std::runtime_error("You are not connected to this channel.");
}


void	Server::cmd_part(std::string data_sent, std::vector<Client>::iterator &sender)
{
	std::istringstream iss(&data_sent[6]);
	std::string channel_name;
	iss >> channel_name;

	std::string notif =  YELLOW "You left " + channel_name + ".\n"+ RESET;
	send(sender->getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);

	for (std::vector<Channel>::iterator it = all_channels.begin(); it != all_channels.end();) {
		if (it->getName() == channel_name) {
			it->removeClientFromChannel(sender->getSocket());
			sender->removeInteraction(channel_name);
			if (it->getAllUsers().empty()) {
				it = all_channels.erase(it);
			} else {
				notif = YELLOW + sender->getNickname() + " left " + channel_name + "." RESET;
				msg_to_channel(notif, channel_name, sender);
				++it;
			}
		} else {
			++it;
		}
	}
}