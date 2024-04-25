/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:05:32 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/25 13:26:05 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../headers/irc.hpp"

	class Client {
		
		private:
			std::string			nickname;
			std::string			username;
			std::string			ip;
			int					client_socket;
			struct sockaddr_in	client_address;
			socklen_t			client_address_len;
		
		public:
			Client();
			~Client();

			void					setNickname(std::string nickname);
			std::string				getNickname();

			void					setUsername(std::string username);
			std::string				getUsername();

			void					setClientSocket(int socket);
			int						getSocket();

			void					setAddress(struct sockaddr_in addr);
			struct sockaddr_in 		getAddress();
			struct sockaddr_in& 	getAddressREF();

			void					setAddressLen(socklen_t addr_len);
			socklen_t				getAddressLen();
			socklen_t& 				getAddressLenREF();

			std::string				getIP();
			void					setIP(std::string x);			
	};

#endif