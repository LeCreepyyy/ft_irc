/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgaertne <bgaertne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:09:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/24 14:38:46 by bgaertne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"

Client::Client() {
    nickname = "/";
    username = "/";
}

Client::~Client() {
    ;
}

void    Client::setClientSocket(int socket) {
    client_socket = socket;
}

std::string Client::getNickname() {
    return (nickname);
}

std::string Client::getUsername() {
    return (username);
}

int Client::getSocket() {
    return (client_socket);
}

struct sockaddr_in Client::getAddress() {
    return (client_address);
}

socklen_t Client::getAddressLen() {
    return (client_address_len);
}

struct sockaddr_in& Client::getAddressREF() {
    return (client_address);
}

socklen_t& Client::getAddressLenREF() {
    return (client_address_len);
}
