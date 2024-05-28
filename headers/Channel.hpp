/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 10:31:29 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/28 13:55:45 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "irc.hpp"

	class Channel {
		
		private :
			std::string			name;
			std::vector<int>	all_operators;
			std::vector<int>	all_users;
			bool				on_whitelist;
			std::vector<int>	whitelist;
			bool				topic_restricted;
			std::string			topic;
			bool				password_protected;
			std::string			password;
			int					user_limit;
			//std::vector<Client> temp;

		public :

			/* Constructors, Destructors */
			Channel(std::string name, int op_socket);
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

			std::vector<int>&	getAllOperators();
			void				addClientToOperators(int client_socket);
			void				removeClientFromOperators(int client_socket);
			void				opUser(bool status, int target_socket, int sender_socket);

			std::vector<int>&	getAllUsers();
			void				addClientToChannel(int client_socket);
			void				removeClientFromChannel(int client_socket);

			int					getUserLimit();
			void				setUserLimit(int limit);

	};

#endif