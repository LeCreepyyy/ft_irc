/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:05:32 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/02 14:11:25 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Channel.hpp"
# include "irc.hpp"

	class Client {
		
		private:
			std::string				nickname;
			std::string				username;
			std::string				ip;
			int						client_socket;
			struct sockaddr_in		client_address;
			socklen_t				client_address_len;
			std::map<Channel, bool>	current_channels; // bool = operator
		
		public:
			Client();
			~Client();

		// Accessors
			void					setNickname(std::string cmd);
			std::string				getNickname();

			void					setUsername(std::string username);
			std::string				getUsername();

			void					setClientSocket(int socket);
			int						getSocket();

			void					setAddress(struct sockaddr_in addr);
			struct sockaddr_in		getAddress();
			struct sockaddr_in&		getAddressREF();

			void					setAddressLen(socklen_t addr_len);
			socklen_t				getAddressLen();
			socklen_t& 				getAddressLenREF();

			/*void					setCurrentChannel(Channel &channel);
			std::map<Channels&>		getCurrentChannel();*/
			
			std::string				getIP();
			void					setIP(std::string x);
	};

# endif