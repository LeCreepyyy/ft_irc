/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:05:32 by vpoirot           #+#    #+#             */
/*   Updated: 2024/06/26 14:39:55 by bgaertne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "irc.hpp"

	class Channel;

	class Client {
		
		private:
			std::string					nickname;
			std::vector<std::string>	username;
			std::string					serv_name;
			std::string					ip;
			int							client_socket;
			struct sockaddr_in			client_address;
			socklen_t					client_address_len;
			std::vector<Channel>		last_interaction;
		
		public:

			/* Constructors, Destructors */
			Client();
			~Client();

			/* Methods */
			bool					operator==(const Client& other) const;
			bool					operator!=(const Client& other) const;
			
			/* Accessors */
			void					setServName(std::string servname);
			std::string				getServName();
			
			std::string				setNickname(std::string cmd, std::vector<Client> &all_clients);
			std::string				getNickname();

			void					setUsername(std::string username);
			std::vector<std::string>	getUsername();

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

			void					setLastInteraction(Channel& target);
			Channel&				getLastInteraction();
			std::vector<Channel>	getAllInteractions();
			void					removeFromLastInteraction(Channel& target);
	};

# endif