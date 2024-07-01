/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 10:31:29 by vpoirot           #+#    #+#             */
/*   Updated: 2024/07/01 11:06:31 by vpoirot          ###   ########.fr       */
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
			std::vector<Client>	all_users;
			bool				on_whitelist;
			std::vector<Client>	whitelist;
			bool				topic_restricted;
			std::string			topic;
			bool				password_protected;
			std::string			password;
			int					user_limit;

		public :

			/* Constructors, Destructors */
			Channel();
			Channel(std::string name, Client oper);
			~Channel();


			/* Methods */
			bool			operator==(const Channel& other) const;
			bool			operator!=(const Channel& other) const;
			bool			operator==(size_t size) const;
			bool			operator!=(size_t size) const;
			Channel&		operator=(const Channel& other);

			/* Accessors */
			std::string		getModes();
			
			void			setName(std::string name);
			std::string		getName();

			void			setTopic(std::string newTopic, Client& sender);
			std::string		getTopic();
			void			setTopicRestriction(bool status, Client& sender);
			bool			getTopicRestriction();
			std::string		rplTopic(Client& sender);

			std::vector<Client>&	getAllOperators();
			void				addClientToOperators(Client& target);
			void				removeClientFromOperators(Client& client);
			void				opUser(bool status, Client& target, Client& sender);
			bool				isUserOp(Client& target);

			std::vector<Client>&	getAllUsers();
			void				addClientToChannel(Client& target);
			void				removeClientFromChannel(Client& target);
			bool				isUserInChannel(Client& client);	

			void				setWhitelist(bool status, Client& sender);
			std::vector<Client>	getWhitelist();
			bool				addClientToWhitelist(Client& target);
			bool				removeClientFromWhitelist(Client& target);
			bool				getWhitelistStatus();
			bool				isUserOnWhitelist(Client& target);

			void				setPassword(bool status, std::string password, Client& sender);
			std::string			getPassword();
			bool				getPasswordStatus();
			
			int					getUserLimit();
			void				setUserLimit(int limit, Client& sender);

			std::string			getListOfNames();
			bool				isOP(Client& client);
	};

#endif