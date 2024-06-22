/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 10:23:28 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/17 14:00:063 by vpoirot          ###   ########.fr       */
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
			throw std::runtime_error(ERR_NEEDMOREPARAMS(sender.getServName(), sender.getNickname()));
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
		throw std::runtime_error(ERR_NOSUCHCHANNEL(sender.getServName(), sender.getNickname(), channel_name));

	if (channel->getWhitelistStatus() == true) {
		std::vector<Client> op_list = channel->getAllOperators();
		std::vector<Client>::iterator op_it;
		for (op_it = op_list.begin(); op_it != op_list.end(); op_it++) {
			if (*op_it == sender)
				break ;
		}
		if (op_it == op_list.end())
			throw std::runtime_error(ERR_CHANOPRIVSNEEDED(sender.getServName(), sender.getNickname(), channel_name));
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
		throw std::runtime_error(ERR_NOSUCHNICK(sender.getServName(), sender.getNickname(), target_nickname));

	std::vector<Client> channelAllUser = channel->getAllUsers();
	for (std::vector<Client>::iterator it = channelAllUser.begin(); it != channelAllUser.end(); it++) {
		if (*it == *target)
			throw std::runtime_error(ERR_USERONCHANNEL(sender.getServName(), sender.getNickname(), target_nickname, channel_name));
	}

	if (channel->getUserLimit() > -1)
		if (channel->getAllUsers().size() >= static_cast<size_t>(channel->getUserLimit()))
			throw std::runtime_error(ERR_CHANNELISFULL(sender.getServName(), sender.getNickname(), channel_name));
	
	if (channel->getWhitelistStatus() == true)
		channel->addClientToWhitelist(*target);
	d_send(*target, RPL_INVITING(sender.getServName(), sender.getNickname(), target_nickname, channel_name));
}


void	Server::cmd_kick(std::string data_sent, Client& sender) {

	std::vector<Client>::iterator target;
	std::string	target_nickname = "#";
	// 1: set channel_name
	std::istringstream iss(&data_sent[5]);
	std::string channel_name;
	iss >> channel_name;

	if (channel_name[0] != '#') {
		if (!sender.getAllInteractions().size())
			throw std::runtime_error(ERR_NOSUCHNICK(serv_name, sender.getNickname(), channel_name));
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
		throw std::runtime_error(ERR_NOSUCHCHANNEL(serv_name, sender.getNickname(), &channel_name[1]));

	std::vector<Client> op_list = channel->getAllOperators();
	std::vector<Client>::iterator op_it;
	for (op_it = op_list.begin(); op_it != op_list.end(); op_it++) {
		if (*op_it == sender)
			break ;
	}
	if (op_it == op_list.end())
		throw std::runtime_error(ERR_CHANOPRIVSNEEDED(serv_name, sender.getNickname(), &channel_name[1]));

	std::string comment;
	if (target_nickname == "#") {
		std::istringstream iss(&data_sent[6 + channel_name.size()]);
		std::string new_target;
		iss >> new_target;
		comment = clean_message(&data_sent[6 + channel_name.size() + new_target.size()]);
		target_nickname = new_target;
	}
	for (target = all_clients.begin(); target !=  all_clients.end(); target++) {
		if (target->getNickname() == target_nickname)
			break;
	}
	if (target == all_clients.end())
		throw std::runtime_error(ERR_USERNOTINCHANNEL(serv_name, sender.getNickname(), target_nickname, channel_name));
	
	std::vector<Client> channelAllUser = channel->getAllUsers();
	for (std::vector<Client>::iterator it = channelAllUser.begin(); it != channelAllUser.end(); it++) {
		if (*it == *target) {
			channel->removeClientFromWhitelist(*target);
			channel->removeClientFromOperators(*target);
			cmd_part("PART " + channel_name, *target);
			std::string message = RPL_KICK(sender.getNickname(), sender.getUsername()[0], sender.getUsername()[1], channel_name, target_nickname, comment);
			cmd_to_channel(message, *channel, sender);
			d_send(*target, message);
			return ;
		}
	}
	throw std::runtime_error(ERR_USERNOTINCHANNEL(serv_name, sender.getNickname(), target_nickname, channel_name));
}

void	Server::cmd_join(std::string data_sent, Client& sender)
{
	std::istringstream iss(&data_sent[5]);
	std::string channel_name;
	std::string password;
	iss >> channel_name;
	iss >> password;

	if (channel_name[0] != '#')
		throw std::runtime_error(ERR_BADCHANMASK(serv_name, sender.getNickname(), channel_name));
	channel_name = &channel_name[1];
	// Looking if the channel already exists
	for (size_t i = 0; i != all_channels.size(); i++) {
		if (all_channels[i].getName() == channel_name) {
			// Check if user is already in channel
			std::vector<Client> users = all_channels[i].getAllUsers();
			if (std::find(users.begin(), users.end(), sender) != users.end())
				throw std::runtime_error(ERR_UNKNOWERROR(serv_name, sender.getNickname(), "You already joined this channel"));
		
			// If channel is limited, check if place is left
			if (all_channels[i].getUserLimit() > -1)
				if (all_channels[i].getAllUsers().size() >= static_cast<size_t>(all_channels[i].getUserLimit()))
					throw std::runtime_error(ERR_CHANNELISFULL(serv_name, sender.getNickname(), channel_name));

			// If channel is whitelisted, is the user on the whitelist
			if (all_channels[i].getWhitelistStatus()) {
			std::vector<Client> whitelist = all_channels[i].getWhitelist();
			if (std::find(whitelist.begin(), whitelist.end(), sender) == whitelist.end())
				throw std::runtime_error(ERR_INVITEONLYCHAN(serv_name, sender.getNickname(), channel_name));
			}

			// if channel is protected by password, look for it in the user input.
			if (all_channels[i].getPasswordStatus() == true) {
				if (all_channels[i].getPassword() != password)
					throw std::runtime_error(ERR_BADCHANNELKEY(serv_name, sender.getNickname(), channel_name));
			}
			
			// Joining the channel
			all_channels[i].addClientToChannel(sender);// adding client to channel's user list
			sender.setLastInteraction(all_channels[i]);
			// Notifying the client
			
			std::string	notif = RPL_JOIN(sender.getNickname(), sender.getUsername()[0], sender.getUsername()[1], channel_name);
			cmd_to_channel(notif, all_channels[i], sender);
			std::string notif2 = all_channels[i].rplTopic(sender);
			std::string	notif3 = RPL_NAMREPLY(serv_name, sender.getNickname(), channel_name, all_channels[i].getListOfNames())
			std::string	notif4 = RPL_ENDOFNAMES(serv_name, sender.getNickname(), channel_name);
			d_send(sender, notif2);
			d_send(sender, notif3);
			d_send(sender, notif4);
			
			return;
		}
	}

	// Channel does not exists, so we create it
	Channel	newChannel(channel_name, sender);
	newChannel.addClientToChannel(sender); // adding client to channel's user list
	all_channels.push_back(newChannel); // pushing newChannel into the all_channels list
	sender.setLastInteraction(newChannel);

	// Notifying the client
	std::string	notif = RPL_JOIN(sender.getNickname(), sender.getUsername()[0], sender.getUsername()[1], channel_name);
	cmd_to_channel(notif, newChannel, sender);
	std::string notif2 = newChannel.rplTopic(sender);
	std::string	notif3 = RPL_NAMREPLY(serv_name, sender.getNickname(), channel_name, newChannel.getListOfNames())
	std::string	notif4 = RPL_ENDOFNAMES(serv_name, sender.getNickname(), channel_name);
	d_send(sender, notif2);
	d_send(sender, notif3);
	d_send(sender, notif4);
}

void	Server::cmd_privmsg(std::string data_sent, Client& sender)
{
	std::istringstream	iss(data_sent);
	std::string			target, command, message;
	
	iss >> command >> target;
	std::getline(iss, message);
	size_t pos = message.find_first_not_of(" ");
    if (pos != std::string::npos) {
        message = message.substr(pos);
    }
	if (message.empty()) {
        throw std::runtime_error(ERR_NOTEXTTOSEND(serv_name, sender.getNickname()));
    }
	message = &message[1];

	if (target[0] == '#') {
		Channel chan = getChannel(target, sender);

		std::vector<Client> ChanAllUsers = chan.getAllUsers();
		if (std::find(ChanAllUsers.begin(), ChanAllUsers.end(), sender) != ChanAllUsers.end()) {
			msg_to_channel(message, chan, sender);
			return;	
		} else {
			throw std::runtime_error(ERR_CANNOTSENDTOCHAN(serv_name, sender.getNickname(), "#" + chan.getName()));
		}
	}
	else {
		Client cli = getClient(target, sender);
		std::string notif = RPL_PRIVMSG(sender.getNickname(), sender.getUsername()[0], serv_name, target, message);
		d_send(cli, notif);
	}
}

void	Server::cmd_part(std::string data_sent, Client& sender)
{
	std::istringstream iss(&data_sent[5]);
	std::string channel_name;
	iss >> channel_name;

	if (channel_name.size() == 0 && sender.getAllInteractions().size())
		channel_name = "#" + sender.getLastInteraction().getName();
	else if (channel_name.size() == 0 && sender.getAllInteractions().size() == 0)
		throw std::runtime_error(ERR_NOSUCHCHANNEL(serv_name, sender.getNickname(), channel_name));

	channel_name = &channel_name[1];
	for (std::vector<Channel>::iterator it = all_channels.begin(); it != all_channels.end();) {
		if (it->getName() == channel_name) {
			it->removeClientFromChannel(sender);
			sender.removeFromLastInteraction(*it);
			std::string notif = RPL_USERLEFT(sender.getNickname(), sender.getUsername()[0], serv_name, channel_name);
			d_send(sender, notif);
			if (it->getAllUsers().empty()) {
				it = all_channels.erase(it);
			} else {
				notif = " left this channel.";
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
	if (target_name.empty())
		throw std::runtime_error(ERR_NEEDMOREPARAMS(serv_name, sender.getNickname() + " TOPIC"));

	topic = clean_message(&data_sent[6 + target_name.size()]);
	topic = &topic[1];

	Channel target;
	if (target_name.size() > 1 && target_name[0] == '#')
		target = getChannel(target_name, sender);
	else
		throw std::runtime_error(ERR_NOSUCHCHANNEL(serv_name, sender.getNickname(), target_name));
	for (std::vector<Channel>::iterator it = all_channels.begin(); it != all_channels.end(); it++) {
		if (*it == target) {
			if (topic.empty()) {
				std::string msg = target.rplTopic(sender);
				d_send(sender, msg);
			}
			else {
				it->setTopic(topic, sender);
				cmd_to_channel(RPL_NEWTOPICSET(sender.getNickname(), sender.getUsername()[0], sender.getUsername()[1], it->getName(), topic), *it, sender);
			}
			return;
		}
	}
	throw std::runtime_error(ERR_NOSUCHCHANNEL(sender.getServName(), sender.getNickname(),target_name));
}

void	Server::cmd_CAP(std::string data_sent, Client& sender)
{
	std::istringstream iss(data_sent);
	std::string subcommand;
	iss >> subcommand;
	iss >> subcommand;

	if (subcommand == "LS") {
		std::string response = "CAP * LS :\n";
		d_send(sender, response);
		std::cout << "CAP LS responded." << std::endl;
	}
	else if (subcommand == "END") {
		std::cout << "CAP negotiation ended." << std::endl;
	}
}

void	Server::cmd_mode(std::string data_sent, Client& sender)
{
	std::istringstream iss(&data_sent[5]);
	std::string target, options, args;
	iss >> target >> options;

	Channel target_channel;
	Client target_client;

	int option_sign;
	if (options[0] == '+')
		option_sign = 1;
	else if (options[0] == '-')
		option_sign = 0;

	if (target.size() > 0 && target[0] == '#') {
		target_channel = getChannel(target, sender);
		int i = 0;
		while (options[++i]) {
			if (option_sign == 1) {
				if (options[i] == 'b')
					throw std::runtime_error(RPL_BANLIST(serv_name, sender.getNickname(), target_channel.getName()));
			}
			if (target_channel.isUserOp(sender) == false)
				throw std::runtime_error(ERR_CHANOPRIVSNEEDED(serv_name, sender.getNickname(), target_channel.getName()));
			if (option_sign == 1) {
				if (options[i] == 'i')
					target_channel.setWhitelist(true, sender);
				else if (options[i] == 't')
					target_channel.setTopicRestriction(true, sender);
				else if (options[i] == 'k') {
					iss >> args;
					target_channel.setPassword(true, args, sender);
				}
				else if (options[i] == 'o') {
					iss >> args;
					target_channel.opUser(true, getClient(args, sender), sender);
				}
				else if (options[i] == 'l') {
					int limit = -1;
					iss >> args;
					if (!args.empty())
						limit = atoi(args.c_str());
					target_channel.setUserLimit(limit, sender);
				}
				else 
					throw std::runtime_error(ERR_UNKNOWERROR(serv_name, sender.getNickname(), "Invalid option in MODE command."));
			}
			if (option_sign == 0) { // '-'
				if (options[i] == 'i')
					target_channel.setWhitelist(false, sender);
				else if (options[i] == 't')
					target_channel.setTopicRestriction(false, sender);
				else if (options[i] == 'k') {
					target_channel.setPassword(false, "/", sender);
				}
				else if (options[i] == 'o') {
					iss >> args;
					target_channel.opUser(false, getClient(args, sender), sender);
				}
				else if (options[i] == 'l') {
					target_channel.setUserLimit(-1, sender);
				}
				else 
					throw std::runtime_error(ERR_UNKNOWERROR(serv_name, sender.getNickname(), "Invalid option in MODE command."));

			}
		}
	}
	else {
		target_client == getClient(target, sender);
		int i = 0;
		while (options[++i]) {
			if (option_sign == 0)
				throw std::runtime_error(ERR_UNKNOWERROR(serv_name, sender.getNickname(), "Invalid option in MODE command."));
			if (option_sign == 1) {
				if (options[i] == 'i')
					throw std::runtime_error(ERR_USERSDONTMATCH(serv_name, sender.getNickname()));
			}
		}
	}
}


void	Server::cmd_ping(std::string data_sent, Client& sender) {
	if (!data_sent[5])
		throw std::runtime_error(ERR_NEEDMOREPARAMS(serv_name, sender.getNickname() + " PING"));
	data_sent.erase(0, 4);
	std::string reply_msg = PONG(serv_name, data_sent);
	d_send(sender, reply_msg);
}

void	Server::cmd_notice(std::string data_sent, Client& sender) {
	std::istringstream iss(&data_sent[6]);
	std::string target_name;
	iss >> target_name;

	if (target_name.empty())
		throw std::runtime_error(ERR_NEEDMOREPARAMS(sender.getServName(), sender.getNickname() + " NOTICE"));
	std::string message = clean_message(&data_sent[6 + target_name.size()]);
	if (target_name[0] == '#')
		cmd_to_channel(RPL_NOTICE(sender.getServName(), sender.getNickname(), target_name, message), getChannel(target_name, sender), sender);
	else
		d_send(getClient(target_name, sender), RPL_NOTICE(sender.getServName(), sender.getNickname(), target_name, message));
}

void	Server::cmd_who(std::string data_sent, Client& sender) {
	std::istringstream iss(&data_sent[3]);
	std::string channel;
	iss >> channel;

	debug(channel);
	std::string notif;
	Channel chan = getChannel(channel, sender);
	std::string op_string = "";
	if (chan.isOP(sender))
		op_string += "@";
	notif = RPL_WHOREPLY(serv_name, sender.getNickname(), channel, sender.getUsername()[0], sender.getUsername()[1], op_string, sender.getUsername()[3]);
	d_send(sender, notif);
	notif = RPL_ENDOFWHO(serv_name, sender.getNickname(), channel);
	d_send(sender, notif);
}

void	Server::cmd_help(Client& sender)
{
	std::string nickname = sender.getNickname();
	std::string username = sender.getUsername()[0];
	std::string hostname = sender.getUsername()[1];
	d_send(sender, RPL_HELPSTART(nickname, username, hostname));
	
	d_send(sender, RPL_HELPTXT(nickname, username, hostname, "NICK", "/nick <nickname>" ));
	d_send(sender, RPL_HELPTXT(nickname, username, hostname, "PRIVMSG", "/privmsg <target> <message>"));
	d_send(sender, RPL_HELPTXT(nickname, username, hostname, "JOIN", "/join <#channel> [password]"));	
	d_send(sender, RPL_HELPTXT(nickname, username, hostname, "PART", "/part [#channel]"));
	d_send(sender, RPL_HELPTXT(nickname, username, hostname, "INVITE", "/invite [#channel] <target>"));
	d_send(sender, RPL_HELPTXT(nickname, username, hostname, "KICK", "/kick [#channel] <target>"));
	d_send(sender, RPL_HELPTXT(nickname, username, hostname, "MODE", "/mode [#channel] <+|- OPTION>"));
	d_send(sender, RPL_HELPTXT(nickname, username, hostname, "TOPIC", "/topic [#channel] <topic>"));
	d_send(sender, RPL_ENDOFHELP(nickname, username, hostname));
}

void	Server::cmd_quit(std::string data_sent, Client& sender)
{
	std::vector<Channel> allInteractions = sender.getAllInteractions();
	for (std::vector<Channel>::iterator chan_it = allInteractions.begin(); chan_it != allInteractions.end(); chan_it++)
		cmd_to_channel(RPL_QUIT(sender.getNickname(), sender.getUsername()[0], sender.getUsername()[1], clean_message(&data_sent[5])), *chan_it, sender);
}

