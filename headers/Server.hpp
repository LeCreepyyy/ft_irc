/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:13:34 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/27 13:13:01 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP


# define MAX_WAITLIST 5
# define MAX_CLIENT 10

# include "irc.hpp"

	class Channel;
	class Client;

	class Server {
		
		private :
			int							serv_port;
			int 						serv_socket;
			std::string					serv_name;
			std::string					serv_password;
			std::vector<Client>			pass_list;
			struct sockaddr_in			serv_address;
			std::vector<Channel>		all_channels;
			std::vector<Client> 		all_clients;
			fd_set monitored;


		public :

			/* Constructors, Destructors */
			Server(std::string name, std::string password, int port);
			~Server();


			/* Methods */
			void	start();
			void	crash(std::string log);
			void	quit(Client& iter_client);

			void	check_password(std::string data_sent, Client& sender);
			void	handle_client_input(std::string data_sent, Client& sender);
			void	msg_to_channel(std::string msg, Channel target, Client& sender);
			void	cmd_to_channel(std::string msg, Channel target, Client& sender);

			void	cmd_join(std::string data_sent, Client& sender);
			void	cmd_privmsg(std::string data_sent, Client& sender);
			void	cmd_part(std::string data_sent, Client& sender);
			void	cmd_kick(std::string data_sent, Client& sender);
			void	cmd_mode(std::string data_sent, Client& sender);
			void	cmd_invite(std::string data_sent, Client& sender);
			void	cmd_topic(std::string data_sent, Client& sender);
			void	cmd_CAP(std::string data_sent, Client& sender);
			void	cmd_help(Client& sender);
			void	cmd_ping(std::string data_sent, Client& sender);
			void	cmd_who(std::string data_sent, Client& sender);
			void	cmd_quit(std::string data_sent, Client& sender);
			void	cmd_notice(std::string data_sent, Client& sender);
			void	cmd_nick(std::string data_sent, Client& sender);

			/* Accessors */

			Channel&	getChannel(std::string channel_name, Client& sender);
			Client&		getClient(std::string client_name, Client& sender);
	};

#endif