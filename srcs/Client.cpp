/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:09:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/25 13:26:39 by vpoirot          ###   ########.fr       */
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

void    Client::setAddress(struct sockaddr_in addr) {
    client_address = addr;
}

struct sockaddr_in Client::getAddress() {
    return (client_address);
}

struct sockaddr_in& Client::getAddressREF() {
    return (this->client_address);
}

// accessor Client Address Len

void    Client::setAddressLen(socklen_t addr_len) {
    client_address_len = addr_len;
}

socklen_t Client::getAddressLen() {
    return (client_address_len);
}

socklen_t& Client::getAddressLenREF() {
    return (client_address_len);
}

// accessor Client IP

std::string Client::getIP() {
    return (ip);
}

void    Client::setIP(std::string x) {
    ip = x;
}