/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:05:32 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/24 13:33:50 by vpoirot          ###   ########.fr       */
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

			/**
			 * Set attribute's value
			 * @param attribute NICKNAME, USERNAME, CLIENT_SOCKET, CLIENT_ADDRESS or CLIENT_ADDRESS_LEN
			 * @param value The value to set
			*/
			template<typename T>
			void set(int attribute, T value) {
				switch (attribute) {
					case NICKNAME:
						nickname = value;
						break;
					case USERNAME:
						username = value;
						break;
					case CLIENT_SOCKET:
						client_socket = value;
						break;
					case CLIENT_ADDRESS:
						client_address = value;
						break;
					case CLIENT_ADDRESS_LEN:
						client_address_len = value;
						break;
					default:
						break;
				}
			}

			/**
			 * Get attribute's content
			 * @param attribute  NICKNAME, USERNAME, CLIENT_SOCKET, CLIENT_ADDRESS or CLIENT_ADDRESS_LEN
			*/
			template<typename T>
			T& get(int attribute) {
				switch (attribute) {
					case NICKNAME:
						return (nickname);
					case USERNAME:
						return (username);
					case CLIENT_SOCKET:
						return (client_socket);
					case CLIENT_ADDRESS:
						return (client_address);
					case CLIENT_ADDRESS_LEN:
						return (client_address_len);
					default:
						break;
				}
			}
	};

#endif