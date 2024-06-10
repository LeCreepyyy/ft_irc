/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:09:45 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/10 14:27:01 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <string>
# include <algorithm>
# include <cstdlib>
# include <cstring>
# include <map>
# include <list>
# include <sys/types.h>
# include <sys/socket.h>
# include <cerrno>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <poll.h>
# include <vector>
# include <sstream>
# include <ctime>

# include "Channel.hpp"
# include "Client.hpp"
# include "Server.hpp"

#define ERR_NOTREGISTERED(source)                       "451 " + source + " :You have not registered"
#define ERR_PASSWDMISMATCH(source)                      "464 " + source + " :Password is incorrect"

#define ERR_UNKNOWNCOMMAND(source, command)             "421 " + source + " " + command + " :Unknown command"

#define ERR_TOOMANYCHANNELS(source, channel)            "405 " + source + " " + channel + " :You have joined too many channels"
#define ERR_CHANOPRIVSNEEDED(source, channel)           "482 " + source + " " + channel + " :You're not channel operator"

#define ERR_USERNOTINCHANNEL(source, target, channel) 	"441 " + source + " " + target + " " + channel + " :They aren't on that channel"

#define RPL_NAMREPLY(source, channel, users)            "353 " + source + " = " + channel + " :" + users
#define RPL_ENDOFNAMES(source, channel)                 "366 " + source + " " + channel + " :End of /NAMES list."

#define RPL_PART(source, channel)                       ":" + source + " PART :" + channel
#define RPL_PING(source, token)                         ":" + source + " PONG :" + token
#define RPL_NOTICE(source, target, message)             ":" + source + " NOTICE " + target + " :" + message
#define RPL_QUIT(source, message)                       ":" + source + " QUIT :Quit: " + message
#define RPL_KICK(source, channel, target, reason)       ":" + source + " KICK " + channel + " " + target + " :" + reason
#define RPL_MODE(source, channel, modes, args)          ":" + source + " MODE " + channel + " " + modes + " " + args

// NICK :
#define RPL_NICK(nickname, hostname, newname)			":" + nickname + "!" + hostname + " NICK :" + newname + "\r\n"
#define ERR_NICKNAMEINUSE(server, nickname, newname)            ":" + server + " 433 " + nickname + " " + newname + " :Nickname is already in use\r\n"
#define ERR_ERRONEUSNICKNAME(server, nickname, newname)			":" + server + " 432 " + nickname + " " + newname + " :Erroneous nickname\r\n"
#define ERR_NONICKNAMEGIVEN(server, nickname)					":" + server + " 431 " + nickname + " :No nickname given\r\n"
#define ERR_NICKTOOFAST(server, nickname)						":" + server + " 438 " + nickname + " :Nick change too fast. Please wait\r\n"
#define ERR_UNAVAILRESOURCE(server, nickname, newname)			":" + server + " 437 " + nickname + " " + newname + " :Nick/channel is temporarily unavailable\r\n"

// USER
#define RPL_WELCOME(server, source)						":" + server + " 001 " + source + " :Welcome " + source + " to the ft_irc network\r\n"
#define RPL_YOURHOSTIS(server, source)					":" + server + " 002 " + source + " :Your host is " + server + ", running version 1.0\r\n"
#define RPL_CREATIONDATE(server, source)				":" + server + " 003 " + source + " :This server was created not too long ago eh\r\n"
#define RPL_SERVINFOS(server, source)					":" + server + " 004 " + source + " " + server + " 1.0\r\n"
#define ERR_ALREADYREGISTERED(server, source)           ":" + server + " 462 " + source + " :You may not register\r\n"
#define ERR_NEEDMOREPARAMS(server, source, command)     ":" + server + " 461 " + source + " " + command + " :Not enough parameters\r\n"

//JOIN
#define RPL_JOIN(nickname, hostname, channel)           ":" + nickname + "!" + hostname + " JOIN :#" + channel
#define RPL_TOPIC(server, nickname, channel, topic)		":" + server + " 332 " + nickname + " #" + channel + " :" + topic + "\r\n"
#define ERR_BADCHANNELKEY(server, nickname, channel)    ":" + server + " 475 " + nickname + " #" + channel + " :Cannot join channel (+k)\r\n"
#define ERR_CHANNELISFULL(server, nickname, channel)    ":" + server + " 471 " + nickname + " #" + channel + " :Cannot join channel (+l)\r\n"
#define ERR_INVITEONLYCHAN(server, nickname, channel)	":" + server + " 473 " + nickname + " #" + channel + " :Cannot join channel (+i)\r\n"
#define ERR_BANNEDFROMCHAN(server, nickname, channel)	":" + server + " 474 " + nickname + " #" + channel + " :Cannot join channel (+b)\r\n"
#define ERR_BADCHANMASK(server, nickname, channel)		":" + server + " 474 " + nickname + " #" + channel + " :Bad Channel Mask\r\n"

// PART
#define RPL_USERLEFT(source, username, host, channel)	":" + source + "!" + username + "@" + host + " PART " + channel + "\r\n"
#define ERR_NOSUCHCHANNEL(server, source, channel)		":" + server + " 403 " + source + " " + channel + ":No such channel\r\n"
#define ERR_NOTONCHANNEL(server, source, channel)		":" + server + " 442 " + source + " " + channel + ":You're not on that channel\r\n"

// PRIVMSG
#define RPL_PRIVMSG(source, username, host, receiver, message)	":" + source + "!" + username + "@" + host + " PRIVMSG " + receiver + " :" + message + "\r\n"
#define ERR_NOSUCHNICK(server, source, receiver)				":" + server + " 401 " + source + " " + receiver + " :No such nick/channel\r\n"
#define ERR_CANNOTSENDTOCHAN(server, source, channel)			":" + server + " 404 " + source + " " + channel + " :Cannot send to channel\r\n"
#define ERR_NOTEXTTOSEND(server, source)						":" + server + " 412 " + source + " :No text so send\r\n"
#define ERR_NOTOPLEVEL(server, source, receiver)				":" + server + " 413 " + source + " " + receiver + " :No toplevel domain specified\r\n"
#define ERR_WILDTOPLEVEL(server, source, receiver)				":" + server + " 414 " + source + " " + receiver + " :Wildcard in toplevel domain\r\n"

void						parsing_args(char **argv);
std::string					irc_time();
std::vector<std::string>	splitString(std::string client_input, char spliter);

template<typename T>
void debug(T log) {
	std::cout << "[DEBUG] " << log << std::endl;
}

#endif