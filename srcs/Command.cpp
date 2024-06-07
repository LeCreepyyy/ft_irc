/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 10:23:28 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/07 13:25:48 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"



void	Server::cmd_invite(std::string data_sent, Client& sender) {
	std::vector<Client>::iterator target;
	std::string target_nickname = "#";
	std::istringstream iss(&data_sent[7]);
	std::string channel_name;
	iss >> channel_name;
	
	if (channel_name[0] != '#') {
		if (!sender.getAllInteractions().size())
			throw std::runtime_error("Incorrect channnel name.");
		target_nickname = channel_name;
		channel_name = "#" + sender.getLastInteraction().getName();
	}

	std::vector<Channel>::iterator channel;
	for (channel = all_channels.begin(); channel != all_channels.end(); channel++) {
		if (channel->getName() == &channel_name[1]) {
			break ;
		}
	}
	if (channel == all_channels.end())
		throw std::runtime_error("Channel not found.");

	if (channel->getWhitelistStatus() == true) {
		std::vector<Client> op_list = channel->getAllOperators();
		std::vector<Client>::iterator op_it;
		for (op_it = op_list.begin(); op_it != op_list.end(); op_it++) {
			if (*op_it == sender)
				break ;
		}
		if (op_it == op_list.end())
			throw std::runtime_error("You are not operator on this channel.");
	}

	if (target_nickname == "#") {
		std::istringstream iss(&data_sent[8 + channel_name.size()]);
		std::string new_target;
		iss >> new_target;
		target_nickname = new_target;
	};
	for (target = all_clients.begin(); target !=  all_clients.end(); target++) {
		if (target->getNickname() == target_nickname)
			break;
	}
	if (target == all_clients.end())
		throw std::runtime_error("Target does not exist");

	std::vector<Client> channelAllUser = channel->getAllUsers();
	for (std::vector<Client>::iterator it = channelAllUser.begin(); it != channelAllUser.end(); it++) {
		if (*it == *target)
			throw std::runtime_error("Target is already on this channel.");
	}

	if (channel->getUserLimit() > -1)
		if (channel->getAllUsers().size() >= static_cast<size_t>(channel->getUserLimit()))
			throw std::runtime_error("Channel is full.");
	
	if (channel->getWhitelistStatus() == true)
		channel->addClientToWhitelist(*target);
	std::string message = "You has been invited on : " + channel->getName() + "\n";
	send(target->getSocket(), message.c_str(), message.size(), MSG_DONTWAIT);
}

/**
 * I keep # at begin of the channel name
*/
void	Server::cmd_kick(std::string data_sent, Client& sender) {

	// data_sent ex: KICK (<#Channel Name>) <Client Nickname> [why]

	std::vector<Client>::iterator target;
	std::string	target_nickname = "#";
	// 1: set channel_name
	std::istringstream iss(&data_sent[5]);
	std::string channel_name;
	iss >> channel_name;

	if (channel_name[0] != '#') {
		if (!sender.getAllInteractions().size())
			throw std::runtime_error("Incorrect channel name.");
		target_nickname = channel_name;
		channel_name = "#" + sender.getLastInteraction().getName();
	}

	std::vector<Channel>::iterator channel;
	for (channel = all_channels.begin(); channel != all_channels.end(); channel++) {
		if (channel->getName() == &channel_name[1]) {
			break ;
		}
	}
	if (channel == all_channels.end())
		throw std::runtime_error("Channel not found.");

	std::vector<Client> op_list = channel->getAllOperators();
	std::vector<Client>::iterator op_it;
	for (op_it = op_list.begin(); op_it != op_list.end(); op_it++) {
		if (*op_it == sender)
			break ;
	}
	if (op_it == op_list.end())
		throw std::runtime_error("You are not operator on this channel.");

	if (target_nickname == "#") {
		std::istringstream iss(&data_sent[6 + channel_name.size()]);
		std::string new_target;
		iss >> new_target;
		target_nickname = new_target;
	}
	for (target = all_clients.begin(); target !=  all_clients.end(); target++) {
		if (target->getNickname() == target_nickname)
			break;
	}
	if (target == all_clients.end())
		throw std::runtime_error("Target does not exist");
	
	std::vector<Client> channelAllUser = channel->getAllUsers();
	for (std::vector<Client>::iterator it = channelAllUser.begin(); it != channelAllUser.end(); it++) {
		if (*it == *target) {
			channel->removeClientFromWhitelist(*target);
			channel->removeClientFromOperators(*target);
			cmd_part("PART " + channel_name, *target);
			std::string message = "You have been kicked of : " + channel->getName() + "\n";
			send(target->getSocket(), message.c_str(), message.size(), MSG_DONTWAIT);
			return ;
		}
	}
	throw std::runtime_error("Target is not connected to " + channel_name);

}

void	Server::cmd_join(std::string data_sent, Client& sender)
{
	std::istringstream iss(&data_sent[5]);
	std::string channel_name;
	std::string password;
	iss >> channel_name;
	iss >> password;

	if (channel_name[0] != '#')
		throw std::runtime_error("Invalid channel name.");
	channel_name = &channel_name[1];
	// Looking if the channel already exists
	for (size_t i = 0; i != all_channels.size(); i++) {
		if (all_channels[i].getName() == channel_name) {
			// Check if user is already in channel
			std::vector<Client> users = all_channels[i].getAllUsers();
			if (std::find(users.begin(), users.end(), sender) != users.end())
				throw std::runtime_error("You are already connected to this channel.");
		
			// If channel is limited, check if place is left
			if (all_channels[i].getUserLimit() > -1)
				if (all_channels[i].getAllUsers().size() >= static_cast<size_t>(all_channels[i].getUserLimit()))
					throw std::runtime_error("Channel is full.");

			// If channel is whitelisted, is the user on the whitelist
			if (all_channels[i].getWhitelistStatus()) {
			std::vector<Client> whitelist = all_channels[i].getWhitelist();
			if (std::find(whitelist.begin(), whitelist.end(), sender) == whitelist.end())
				throw std::runtime_error("Channel is whitelisted. You need to be invited by a channel operator.");			
			}

			// if channel is protected by password, look for it in the user input.
			if (all_channels[i].getPasswordStatus() == true) {
				if (all_channels[i].getPassword() != password)
					throw std::runtime_error("Wrong password.");
			}
			
			// Joining the channel
			all_channels[i].addClientToChannel(sender); // adding client to channel's user list
			//sender->addToCurrentChannels(all_channels[i]);			 // adding channel to client's channel list
			sender.setLastInteraction(all_channels[i]);
			// Notifying the client
			std::string	notif = irc_time() + "You joined a new channel: " + channel_name + '\n';
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			notif = " I have arrived.";
			msg_to_channel(notif, all_channels[i], sender);
			return;
		}
	}

	// Channel does not exists, so we create it
	Channel	newChannel(channel_name, sender);
	newChannel.addClientToChannel(sender); // adding client to channel's user list
	//sender->addToCurrentChannels(newChannel); // adding channel to client's channel list
	all_channels.push_back(newChannel); // pushing newChannel into the all_channels list

	// Notifying the client
	std::string	notif = irc_time() + "You created a channel named '" + channel_name + "'\n"
					+ irc_time() + "You now have ADMIN rights in this channel.\n"
					+ irc_time() + sender.getNickname() + " joined this channel.\n";
	sender.setLastInteraction(newChannel);
	send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
}


void	Server::cmd_privmsg(std::string data_sent, Client& sender)
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
			if (message.size() == 0)
				return;
			std::string	notif(irc_time() + sender.getNickname() + " : " + message + "\n");
			send(receiver.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			return;
		}
	}
	std::string	notif("No one named " + receiver_nick + " was found.\n");
	send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
}

void	Server::cmd_msg(std::string data_sent, Client& sender) {
	std::istringstream iss(&data_sent[4]);
	std::string channel_name;
	iss >> channel_name;

	Channel target = getChannel(channel_name);
	if (sender.getAllInteractions().size() && sender.getLastInteraction() == target) {
		msg_to_channel(&data_sent[channel_name.size() + 5], target, sender);
		return;
	}
	for (std::vector<Channel>::iterator it = all_channels.begin(); it != all_channels.end(); it++) {
		if (it->getName() == channel_name) {
			std::vector<Client> channel_users = it->getAllUsers();
			if (find(channel_users.begin(), channel_users.end(), sender) != channel_users.end()) {
				msg_to_channel(&data_sent[channel_name.size() + 5], target, sender);
				return;
			}
			throw std::runtime_error("You are not connected to this channel.");
		}
	}
	throw std::runtime_error("Channel not found");
}

void	Server::cmd_part(std::string data_sent, Client& sender)
{
	std::istringstream iss(&data_sent[5]);
	std::string channel_name;
	iss >> channel_name;

	if (channel_name.size() == 0 && sender.getAllInteractions().size())
		channel_name = "#" + sender.getLastInteraction().getName();
	else if (channel_name[0] != '#' || sender.getAllInteractions().size() == 0)
		throw std::runtime_error("Channel not found.");

	channel_name = &channel_name[1];
	for (std::vector<Channel>::iterator it = all_channels.begin(); it != all_channels.end();) {
		if (it->getName() == channel_name) {
			it->removeClientFromChannel(sender);
			sender.removeFromLastInteraction(*it);
			std::string notif = "You left " + channel_name + ".\n";
			send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
			if (it->getAllUsers().empty()) {
				it = all_channels.erase(it);
			} else {
				notif = sender.getNickname() + " left " + channel_name + ".";
				msg_to_channel(notif, *it, sender);
				++it;
			}
		} else {
			++it;
		}
	}
}


void	Server::cmd_topic(std::string data_sent, Client& sender)
{
	std::istringstream iss(&data_sent[6]);
	std::string target_name;
	std::string topic;
	iss >> target_name;
	iss >> topic;

	Channel target;
	if (target_name.size() > 1 && target_name[0] == '#')
		target = getChannel(target_name);
	else
		target = sender.getLastInteraction();
	for (std::vector<Channel>::iterator it = all_channels.begin(); it != all_channels.end(); it++) {
		if (*it == target) {
			if (topic.empty())
				send(sender.getSocket(), it->getTopic().c_str(), it->getTopic().size(), MSG_DONTWAIT);
			else
				it->setTopic(topic, sender);
			return;
		}
	}
	throw std::runtime_error("Channel not found.");
}


void	Server::cmd_CAP(std::string data_sent, Client& sender)
{
	std::istringstream iss(data_sent);
	std::string subcommand;
	iss >> subcommand;
	iss >> subcommand;

	if (subcommand == "LS") {
		std::string response = "CAP * LS :\n";
		send(sender.getSocket(), response.c_str(), response.size(), MSG_DONTWAIT);
		std::cout << "CAP LS responded." << std::endl;
	}
	else if (subcommand == "END") {
		std::cout << "CAP negotiation ended." << std::endl;
	}
}

void	Server::cmd_mode(std::string data_sent, Client& sender)
{
	std::istringstream iss(&data_sent[5]);
	std::string channel_name;
	std::string option;
	std::string second_target;
	iss >> channel_name;
	iss >> option;
	iss >> second_target;

	Channel target;
	if (channel_name.size() > 1 && channel_name[0] == '#')
		target = getChannel(channel_name);
	else {
		second_target = option;
		option = channel_name;
		target = sender.getLastInteraction();
	}
	for (std::vector<Channel>::iterator channel_it = all_channels.begin(); channel_it != all_channels.end(); channel_it++) {
		if (*channel_it == target) {
			bool is_op = false;
			for (std::vector<Client>::iterator operator_it = channel_it->getAllOperators().begin(); operator_it != channel_it->getAllOperators().end(); operator_it++)
				if (*operator_it == sender)
					is_op = true;
			if (is_op == false)
				throw std::runtime_error("Only channel operators can use the MODE command.");
			if (option == "+i")
				return (channel_it->setWhitelist(true, sender));
			if (option == "-i")
				return (channel_it->setWhitelist(false, sender));
			if (option == "+t")
				return (channel_it->setTopicRestriction(true, sender));
			if (option == "-t")
				return (channel_it->setTopicRestriction(false, sender));
			if (option == "+k")
				return (channel_it->setPassword(true, second_target, sender));
			if (option == "-k")
				return (channel_it->setPassword(false, "/", sender));
			if (option == "+o" || option == "-o") {
				for (std::vector<Client>::iterator it = this->all_clients.begin() ; it != this->all_clients.end() ; it++) {
					if (it->getNickname() == second_target) {
						if (option == "+o")
							return (channel_it->opUser(true, *it, sender));
						else if (option == "-o")
							return (channel_it->opUser(false, *it, sender));
					}
				}
				throw std::runtime_error("Could not find anyone matching this Nickname.");
			}
			if (option == "+l" || option == "-l")
			{
				int limit = -1;
				if (option == "+l") {
					if (second_target.empty())
						throw std::runtime_error("Invalid limit. (>0 && <99)");
					limit = atoi(second_target.c_str());
					if (limit <= 0 || limit > 99)
						throw std::runtime_error("Invalid limit. (>0 && <99)");
					if (static_cast<size_t>(limit) < channel_it->getAllUsers().size())
						throw std::runtime_error("Too much users in channel to set this limit.");
				}
				std::string notif = "Limit was set.\n";
				send(sender.getSocket(), notif.c_str(), notif.size(), MSG_DONTWAIT);
				return (channel_it->setUserLimit(limit));
			}
			throw std::runtime_error("Invalid option in MODE command.");
		}
	}
	throw std::runtime_error("Channel not found.");
}


void	Server::cmd_ping(std::string data_sent, Client& sender) {
	if (!data_sent[5])
		throw std::runtime_error("Missing arg : PING");
	data_sent.erase(0, 4);
	std::string reply_msg = "PONG" + data_sent;
	send(sender.getSocket(), reply_msg.c_str(), strlen(reply_msg.c_str()), MSG_DONTWAIT);
}


void	Server::cmd_help(std::string data_sent, Client& sender)
{
	std::istringstream iss(&data_sent[5]);
	std::string arg;
	iss >> arg;

	send(sender.getSocket(), "\nGlobal command :\n", strlen("Global command :\n"), MSG_DONTROUTE);
	
	send(sender.getSocket(), "-> NICK <nickname>\n", strlen("-> NICK <nickname>\n"), MSG_DONTWAIT);
	send(sender.getSocket(), "-> USER <username> <hostname> <servername> <realname>\n", strlen("-> USER <username> <hostname> <servername> <realname>\n"), MSG_DONTWAIT);
	send(sender.getSocket(), "-> PRIVMSG <target> <message>\n", strlen("-> PRIVMSG <target> <message>\n"), MSG_DONTWAIT);
	send(sender.getSocket(), "-> MSG <#channel> <message>\n", strlen("-> MSG <#channel> <message>\n"), MSG_DONTWAIT);
	send(sender.getSocket(), "-> JOIN <#channel> (password)\n", strlen("-> JOIN <#channel> (password)\n"), MSG_DONTWAIT);
	send(sender.getSocket(), "-> HELP\n\n", strlen("-> HELP (commande)\n\n"), MSG_DONTWAIT);
	
	send(sender.getSocket(), "Only in channel :\n", strlen("Only in channel :\n"), MSG_DONTROUTE);
	
	send(sender.getSocket(), "-> PART (#channel)\n", strlen("-> PART (#channel)\n"), MSG_DONTWAIT);
	send(sender.getSocket(), "-> INVITE (#channel) <target>\n\n", strlen("-> INVITE (#channel) <target>\n\n"), MSG_DONTWAIT);
	
	send(sender.getSocket(), "-Only operator :\n", strlen("-Only operator :\n"), MSG_DONTROUTE);
	
	send(sender.getSocket(), "-> KICK (#channel) <target>\n", strlen("-> KICK (#channel) <target>\n"), MSG_DONTWAIT);
	send(sender.getSocket(), "-> MODE (#channel) <+ or -OPTION>\n", strlen("-> MODE (#channel) <+ or -OPTION>\n"), MSG_DONTWAIT);
	send(sender.getSocket(), "-> TOPIC (#channel) <message>\n\n", strlen("-> TOPIC (#channel) <message>\n\n"), MSG_DONTWAIT);
}

