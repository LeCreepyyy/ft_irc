/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:13:34 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/10 13:47:37 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"

# define MAX_WAITLIST 5
# define MAX_CLIENT 10

	class Server {
		
		private :
			int							serv_port;
			int 						serv_socket;
			std::string					serv_name;
			std::string					serv_password;
			std::vector<int>			pass_list;
			struct sockaddr_in			serv_address;
			std::vector<Channel>		all_channels;
			std::vector<Client> 		all_clients;

		public :

			/* Constructors, Destructors */
			Server(std::string name, std::string password, int port);
			~Server();


			/* Methods */
			void	start();
			void	crash(std::string log);

			void	check_password(std::string data_sent, std::vector<Client>::iterator & sender);
			void	handle_client_input(std::string data_sent, std::vector<Client>::iterator &sender);
			void	msg_to_channel(std::string msg, std::string channel_name, std::vector<Client>::iterator &sender);

			void	cmd_join(std::string data_sent, std::vector<Client>::iterator &sender);
			void	cmd_msg(std::vector<Client>::iterator &sender, std::string data_sent);
			
			/* Accessors */
	};

#endif