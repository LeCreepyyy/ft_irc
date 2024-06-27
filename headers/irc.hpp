/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:09:45 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/27 14:16:10 by vpoirot          ###   ########.fr       */
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
# include <sys/select.h>
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

// GLOBAL
/**
 * @param source <nickname> + <command>
 */
#define ERR_NEEDMOREPARAMS(server, source)     		":" + server + " 461 " + source + " :Not enough parameters\r\n"
#define ERR_UNKNOWERROR(server, nickname, error)	":" + server + " 400 " + nickname + " :" + error + "\r\n"

/// RPL COMMAND

// invite
#define RPL_INVITING(server, nickname, target, channel) ":" + server + " 341 " + nickname + " " + target + " " + channel + "\r\n"

// pong
#define PONG(server, message)							":" + server + " PONG :" + message + "\r\n"

// notice
#define RPL_NOTICE(server, nickname, target, message)	":" + server + " 301 " + nickname + " " + target + " :" + message + "\r\n"

//nick
#define RPL_NICK(nickname, hostname, newname)			":" + nickname + "!" + hostname + " NICK :" + newname + "\r\n"

//connection
#define RPL_WELCOME(server, source)						":" + server + " 001 " + source + " :Welcome " + source + " to the ft_irc network\r\n"
#define RPL_YOURHOSTIS(server, source)					":" + server + " 002 " + source + " :Your host is " + server + ", running version 1.0\r\n"
#define RPL_CREATIONDATE(server, source)				":" + server + " 003 " + source + " :This server was created not too long ago eh\r\n"
#define RPL_SERVINFOS(server, source)					":" + server + " 004 " + source + " " + server + " 1.0\r\n"

//join
#define RPL_JOIN(nickname, username, hostname, channel) ":" + nickname + "!" + username + "@" + hostname + " JOIN :#" + channel + "\r\n"
#define RPL_NAMREPLY(server, source, channel, names)	":" + server + " 353 " + source + " = #" + channel + " :" + names + "\r\n";
#define RPL_ENDOFNAMES(server, source, channel)			":" + server + " 366 " + source + " #" + channel + " :End of /NAMES list.\r\n"

//topic
#define RPL_TOPIC(nickname, username, hostname, channel, topic)			":" + nickname + "!" + username + "@" + hostname + " 332 " + nickname + " " + channel + " :" + topic + "\r\n"
#define RPL_NOTOPIC(server, nickname, channel)							":" + server + " 331 " + nickname + " " + channel + " :No topic is set\r\n"
#define RPL_NEWTOPICSET(nickname, username, hostname, channel, topic)	":" + nickname + "!" + username + "@" + hostname + " TOPIC #" + channel + " :" + topic + "\r\n"

//part
#define RPL_USERLEFT(source, username, host, channel, message)	":" + source + "!" + username + "@" + host + " PART #" + channel + " " + message + "\r\n"

//quit
#define RPL_QUIT(nickname, username, host, msg)				":" + nickname + "!" + username + "@" + host + " QUIT " + msg + "\r\n"

//privmsg
#define RPL_PRIVMSG(source, username, host, receiver, message)	":" + source + "!" + username + "@" + host + " PRIVMSG " + receiver + " :" + message + "\r\n"

//mode
#define RPL_SETMODE(nickname, hostname, channel, mode)			":" + nickname + "!" + hostname + " MODE #" + channel + " " + mode + "\r\n"
#define RPL_GETMODE(server, nickname, channel, mode_list)		":" + server + " 324 " + nickname + " #" + channel + " " + mode_list + "\r\n"
#define RPL_BANLIST(server, nickname, channel)					":" + server + " 368 " + nickname + " #" + channel + " :End of channel ban list.\r\n"
#define RPL_COUNTMODE(server, nickname, channel, size)			":" + server + " 329 " + nickname + " #" + channel + " " + size + "\r\n"

//who
#define RPL_WHOREPLY(server, nickname, channel, username, hostname, isOp, realname) ":" + server + " 352 " + nickname + " " + channel + " " + username + " " + hostname + " " + server + " " + nickname + " H" + isOp + " :0 " +  realname + "\r\n"
#define RPL_ENDOFWHO(server, nickname, channel)										":" + server + " 315 " + nickname + " " + channel + " :End of /WHO list.\r\n"

//help
#define RPL_HELPSTART(nickname, username, hostname)	":" + nickname + "!" + username + "@" + hostname + " 704 " + nickname + " IRC :List of commands:\r\n"
#define RPL_HELPTXT(nickname, username, hostname, subject, text)	":" + nickname + "!" + username + "@" + hostname + " 705 " + nickname + " " + subject + " :" + text + "\r\n"
#define RPL_ENDOFHELP(nickname, username, hostname)	":" + nickname + "!" + username + "@" + hostname + " 706 " + nickname + " IRC :End of /help.\r\n"

//kick
#define RPL_KICK(nickname, username, hostname, channel, target, comment)	":" + nickname + "!" + username + "@" + hostname + " KICK " + channel + " " + target + " :" + comment + "\r\n"

/// ERROR

// invite
#define ERR_USERONCHANNEL(server, nickname, target, channel)	":" + server + " 443 " + nickname + " " + target + " " + channel + " :is already on channel\r\n"

//nickname
#define ERR_NICKNAMEINUSE(server, nickname, newname)            ":" + server + " 433 " + nickname + " " + newname + " :Nickname is already in use\r\n"
#define ERR_ERRONEUSNICKNAME(server, nickname, newname)			":" + server + " 432 " + nickname + " " + newname + " :Erroneous nickname\r\n"
#define ERR_NONICKNAMEGIVEN(server, nickname)					":" + server + " 431 " + nickname + " :No nickname given\r\n"
#define ERR_NICKTOOFAST(server, nickname)						":" + server + " 438 " + nickname + " :Nick change too fast. Please wait\r\n"
#define ERR_UNAVAILRESOURCE(server, nickname, newname)			":" + server + " 437 " + nickname + " " + newname + " :Nick/channel is temporarily unavailable\r\n"

//username
#define ERR_ALREADYREGISTERED(server, source)           ":" + server + " 462 " + source + " :You may not register\r\n"

//connection to channel
#define ERR_BADCHANNELKEY(server, nickname, channel)    ":" + server + " 475 " + nickname + " #" + channel + " :Cannot join channel (+k)\r\n"
#define ERR_CHANNELISFULL(server, nickname, channel)    ":" + server + " 471 " + nickname + " #" + channel + " :Cannot join channel (+l)\r\n"
#define ERR_INVITEONLYCHAN(server, nickname, channel)	":" + server + " 473 " + nickname + " #" + channel + " :Cannot join channel (+i)\r\n"
#define ERR_BANNEDFROMCHAN(server, nickname, channel)	":" + server + " 474 " + nickname + " #" + channel + " :Cannot join channel (+b)\r\n"
#define ERR_BADCHANMASK(server, nickname, channel)		":" + server + " 476 " + nickname + " #" + channel + " :Bad Channel Mask\r\n"
#define ERR_NOSUCHCHANNEL(server, source, channel)		":" + server + " 403 " + source + " " + channel + " :No such channel\r\n"
#define ERR_NOTONCHANNEL(server, source, channel)		":" + server + " 442 " + source + " #" + channel + " :You're not on that channel\r\n"


//message to channel
#define ERR_NOSUCHNICK(server, source, receiver)				":" + server + " 401 " + source + " " + receiver + " :No such nick/channel\r\n"
#define ERR_CANNOTSENDTOCHAN(server, source, channel)			":" + server + " 404 " + source + " " + channel + " :Cannot send to channel\r\n"
#define ERR_NOTEXTTOSEND(server, source)						":" + server + " 412 " + source + " :No text so send\r\n"
#define ERR_NOTOPLEVEL(server, source, receiver)				":" + server + " 413 " + source + " " + receiver + " :No toplevel domain specified\r\n"
#define ERR_WILDTOPLEVEL(server, source, receiver)				":" + server + " 414 " + source + " " + receiver + " :Wildcard in toplevel domain\r\n"
#define ERR_USERNOTINCHANNEL(server, nickname, target, channel) ":" + server + " 441 " + nickname + " " + target + " " + channel + " :They aren't on that channel\r\n"

//commande op channel
#define ERR_UNKNOWNMODE(server, nickname) 						":" + server + " 472 " + nickname + " & :is unknown mode char to me for #canal\r\n"
#define ERR_CHANOPRIVSNEEDED(server, nickname, channel)			":" + server + " 482 " + nickname + " #" + channel + " :You're not channel operator\r\n"
#define ERR_USERSDONTMATCH(server, nickname)					":" + server + " 502 " + nickname + " :Cannot change mode for other users\r\n"



void						parsing_args(char **argv);
std::string					irc_time();
std::vector<std::string>	splitString(std::string client_input, char spliter);
void						d_send(Client& receiver, std::string message);
std::string					clean_message(const std::string& message);

template<typename T>
void debug(T log) {
	std::cout << "<< " << log << std::endl;
}

#endif