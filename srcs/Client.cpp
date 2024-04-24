/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpoirot <vpoirot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:09:21 by vpoirot           #+#    #+#             */
/*   Updated: 2024/04/24 14:25:02 by vpoirot          ###   ########.fr       */
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
