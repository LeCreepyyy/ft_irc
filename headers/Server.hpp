/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:13:34 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/06 13:16:29 by vpoirot          ###   ########.fr       */
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
			struct sockaddr_in			serv_address;
			std::vector<Channel>		channels;
			std::vector<Client> 		clients;

		public :
			Server(std::string name, std::string password, int port);
			~Server();

			void	start();
			void	end(std::string log);
			void	handle_client(std::string data_sent, std::vector<Client>::iterator & client);
			void	send_message(std::string msg, std::string canal, std::vector<Client>::iterator & client);
			// Accessors
	};

#endif