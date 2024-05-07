/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:05:32 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/07 11:52:17 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Channel.hpp"

	class Client {
		
		private:
			std::string				nickname;
			std::string				username;
			std::string				ip;
			int						client_socket;
			struct sockaddr_in		client_address;
			std::vector<Channel>	current_channels;
			socklen_t				client_address_len;
			std::string				last_interaction;
		
		public:

			/* Constructors, Destructors */
			Client();
			~Client();

			/* Methods */

			/* Accessors */
			void					setNickname(std::string cmd);
			std::string				getNickname();

			void					setUsername(std::string username);
			std::string				getUsername();

			void					setSocket(int socket);
			int						getSocket();

			void					setAddress(struct sockaddr_in addr);
			struct sockaddr_in		getAddress();
			struct sockaddr_in&		getAddressREF();

			void					setAddressLen(socklen_t addr_len);
			socklen_t				getAddressLen();
			socklen_t& 				getAddressLenREF();
			
			std::string				getIP();
			void					setIP(std::string x);

			std::vector<Channel>	getCurrentChannels();
			void					addToCurrentChannels(Channel Channel);

			void					setLastInteraction(std::string channel_name);
			std::string				getLastInteraction();
	};

# endif