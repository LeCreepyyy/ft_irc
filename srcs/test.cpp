#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <arpa/inet.h>

const int PORT = 6667;
const int BACKLOG = 10;
const int MAX_CLIENTS = 100;
const int BUFFER_SIZE = 1024;

int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow multiple connections
    int option = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to an address and port
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Initialize client sockets array
    std::vector<int> clientSockets;

    // Set of socket descriptors
    fd_set readfds;
    int maxFd;

    while (true) {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add server socket to set
        FD_SET(serverSocket, &readfds);
        maxFd = serverSocket;

        // Add client sockets to set
        for (const auto& clientSocket : clientSockets) {
            FD_SET(clientSocket, &readfds);
            maxFd = std::max(maxFd, clientSocket);
        }

        // Wait for activity on any of the sockets
        int activity = select(maxFd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            perror("select");
        }

        // If activity is on the server socket, it's a new connection
        if (FD_ISSET(serverSocket, &readfds)) {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
            if (clientSocket == -1) {
                perror("accept");
                continue;
            }

            std::cout << "New connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;

            // Add the new client socket to the array
            clientSockets.push_back(clientSocket);
        }

        // Handle activity on client sockets
        for (auto it = clientSockets.begin(); it != clientSockets.end();) {
            int clientSocket = *it;
            if (FD_ISSET(clientSocket, &readfds)) {
                char buffer[BUFFER_SIZE];
                int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
                if (bytesReceived <= 0) {
                    std::cout << "Client disconnected" << std::endl;
                    close(clientSocket);
                    it = clientSockets.erase(it);
                    continue;
                }

                std::string message(buffer, bytesReceived);
                std::cout << "Received message from client: " << message;
                send(clientSocket, buffer, bytesReceived, 0);
            }
            ++it;
        }
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}