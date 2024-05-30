/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 10:31:29 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/30 10:40:36 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "irc.hpp"

	class Client;

	class Channel {
		
		private :
			std::string			name;
			std::vector<Client>	all_operators;
			std::vector<int>	all_users;
			bool				on_whitelist;
			std::vector<int>	whitelist;
			bool				topic_restricted;
			std::string			topic;
			bool				password_protected;
			std::string			password;
			int					user_limit;

		public :

			/* Constructors, Destructors */
			Channel(std::string name, Client oper);
			~Channel();

			/* Methods */
			bool			operator==(const Channel& other) const;
			bool			operator==(size_t size) const;

			/* Accessors */
			void			setName(std::string name);
			std::string		getName();

			void			setTopic(std::string newTopic, int client_socket);
			std::string		getTopic();
			void			setTopicLimit(bool status, int client_socket);
			
			void				setWhitelist(bool status, int client_socket);
			std::vector<int>	getWhitelist();
			bool				addToWhitelist(int client_socket);
			bool				removeToWhitelist(int client_socket);
			bool				getWhitelistStatus();

			void				setPassword(bool status, std::string password, int client_socket);
			std::string			getPassword();
			bool				getPasswordStatus();

			std::vector<Client>&	getAllOperators();
			void				addClientToOperators(Client client);
			void				removeClientFromOperators(Client client);
			void				opUser(bool status, Client target, int sender_socket);

			std::vector<int>&	getAllUsers();
			void				addClientToChannel(int client_socket);
			void				removeClientFromChannel(int client_socket);

			int					getUserLimit();
			void				setUserLimit(int limit);

	};

#endif