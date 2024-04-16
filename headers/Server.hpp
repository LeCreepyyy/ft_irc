/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:13:34 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/16 13:16:22 by vpoirot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "irc.hpp"

# define MAX_WAITLIST 5

	class Server {
		private :
			int			_port;
			int 		_socket;
			std::string	_name;
			std::string	_password;
			struct sockaddr_in _adress;
		public :
			Server(std::string name, std::string password, int port);
			~Server();

			void	config();
			void	start();
			void	end();
	};

#endif