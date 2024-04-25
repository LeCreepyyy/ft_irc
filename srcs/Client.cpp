/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:09:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/25 10:09:03 by vpoirot          ###   ########.fr       */
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

// accessor Client Socket

void    Client::setClientSocket(int socket) {
    client_socket = socket;
}

int Client::getSocket() {
    return (client_socket);
}

// accessor Client Nickname

void Client::setNickname(std::string name) {
    nickname = name;
}

std::string Client::getNickname() {
    return (nickname);
}

// accessor Client Username 

void    Client::setUsername(std::string name) {
    username = name;
}

std::string Client::getUsername() {
    return (username);
}

// accessor Client Address

void    Client::setClientAddress(struct sockaddr_in addr) {
    client_address = addr;
}

struct sockaddr_in Client::getAddress() {
    return (client_address);
}

struct sockaddr_in& Client::getAddressREF() {
    return (client_address);
}

// accessor Client Address Len

void    Client::setClientAddressLen(socklen_t addr_len) {
    client_address_len = addr_len;
}

socklen_t Client::getAddressLen() {
    return (client_address_len);
}

socklen_t& Client::getAddressLenREF() {
    return (client_address_len);
}
