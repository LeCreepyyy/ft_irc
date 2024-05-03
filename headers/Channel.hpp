/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 10:31:29 by vpoirot           #+#    #+#             */
/*   Updated: 2024/05/03 11:41:17 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <stdexcept>
# include <list>

	class Channel {
		
		private :
			std::list<int>		channel_ops;	
			std::string			channel_name;
			std::string			channel_mode;

		public :
			Channel(std::string name);
			~Channel();

		// Accessors

			void			setName(std::string name);
			std::string		getName();

			void			setMode(std::string mode);
			std::string		getMode();

			std::list<int>&	getChannelOps();
			void			addToChannelOps(int socket_to_op);
	};

#endif