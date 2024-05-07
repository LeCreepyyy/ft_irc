/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 10:31:29 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/07 01:47:14 by bgaertne         ###   ########.fr       */
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
			std::string			mode;

		public :

			/* Constructors, Destructors */
			Channel(std::string name, int op_socket);
			~Channel();

			/* Methods */

			/* Accessors */
			void			setName(std::string name);
			std::string		getName();

			void			setMode(std::string mode);
			std::string		getMode();

			std::vector<int>&	getAllOperators();
			void				addClientToOperators(int client_socket);
			void				removeClientFromOperators(int client_socket);

			std::vector<int>&	getAllUsers();
			void				addClientToChannel(int client_socket);
			void				removeClientFromChannel(int client_socket);
	};

#endif