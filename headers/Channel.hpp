/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 10:31:29 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/03 14:12:05 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "irc.hpp"

	class Channel {
		
		private :
			std::string			channel_name;
			std::vector<int>		channel_ops;
			std::vector<int>		channel_users;
			std::string			channel_mode;

		public :
			Channel(std::string name, int op_socket);
			~Channel();

		// Accessors

			void			setName(std::string name);
			std::string		getName();

			void			setMode(std::string mode);
			std::string		getMode();

			std::vector<int>&	getChannelOps();
			void			addClientToChannelOps(int client_socket);
			void			removeClientFromChannelOps(int client_socket);

			std::vector<int>&	getChannelUsers();
			void			addClientToChannel(int client_socket);
			void			removeClientFromChannel(int client_socket);
	};

#endif