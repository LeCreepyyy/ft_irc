/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:05:32 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/25 10:04:51 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../headers/irc.hpp"

	class Client {
		
		private:
			std::string			nickname;
			std::string			username;
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

			void					setClientAddress(struct sockaddr_in addr);
			struct sockaddr_in 		getAddress();
			struct sockaddr_in& 	getAddressREF();

			void					setClientAddressLen(socklen_t addr_len);
			socklen_t				getAddressLen();
			socklen_t& 				getAddressLenREF();
	};

#endif