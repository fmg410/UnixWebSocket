#include "RunServer.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <string>
#include <limits>
#include "../Common.hpp"
#include "Server.hpp"
#include "../Client/Client.hpp"

void runUdpServer(unsigned short serverPort)
{
    // std::cout << "Server run with local ip: " << sf::IpAddress::getLocalAddress() << "\nand public ip: " << sf::IpAddress::getPublicAddress() << std::endl;

    sf::UdpSocket socket;

    if (socket.bind(serverPort) != sf::Socket::Done)
        return;

    std::vector<Client> clients;

    sf::Packet receivedPacket;
    sf::IpAddress senderIp;
    unsigned short senderPort;
    std::size_t received;
    std::cout << "dzialam" << std::endl;

    do
    {
        std::cout << clients.size() << std::endl;
        if(clients.size() > 0)
            std::cout << clients.at(0).name << std::endl;
        if (socket.receive(receivedPacket, senderIp, senderPort) != sf::Socket::Done)
            return;

        int32_t appId;

        if (!(receivedPacket >> appId && appId == appId_))
            continue;

        std::underlying_type_t<ClientMessageType> messageTypeTemp;
        if (!(receivedPacket >> messageTypeTemp))
            continue;

        ClientMessageType messageType = static_cast<ClientMessageType>(messageTypeTemp);
        std::cout << "dzialam1" << std::endl;
        std::string senderName;
        std::string senderPassword;

        if (!(receivedPacket >> senderName))
            continue;
        if (!(receivedPacket >> senderPassword))
            continue;
        std::cout << "dzialam12" << std::endl;
        Client tempClient{senderName, senderPassword, senderIp, senderPort};
        if (std::count(clients.begin(), clients.end(), tempClient) == 0)
        {
            clients.push_back(tempClient);
        }
        std::cout << "dzialam2" << std::endl;
        if (messageType == ClientMessageType::HANDSHAKE)
        {
            continue;
        }
        else if (messageType == ClientMessageType::CLIENTS_LIST_REQUEST)
        {
            sf::Packet preparedPacket;
            preparedPacket << appId_ << static_cast<std::underlying_type_t<ServerMessageType>>(ServerMessageType::CLIENT_LIST) << static_cast<unsigned int>(clients.size() - 1);
            for (auto &client : clients)
            {
                if (client != tempClient)
                    preparedPacket << client.name;
            }        std::cout << "dzialam3" << std::endl;
            socket.send(preparedPacket, senderIp, senderPort);
        }
        else if (messageType == ClientMessageType::MESSAGE_TO_CLIENT)
        {
            std::string targetName;
            if (!(receivedPacket >> targetName))
                continue;

            const auto messageTarget = std::find_if(clients.begin(), clients.end(), [targetName](Client &c)
                                                    { return c.name == targetName; });
            if (messageTarget == clients.end())
                continue;
        std::cout << "dzialam4" << std::endl;
            std::string message;

            if (!(receivedPacket >> message))
                continue;

            sf::Packet preparedPacket;
            preparedPacket << appId_ << static_cast<std::underlying_type_t<ServerMessageType>>(ServerMessageType::MESSAGE_FROM_SOMEONE) << tempClient.name << message;

            socket.send(preparedPacket, (*messageTarget).adress, (*messageTarget).port);
        }

    } while (true);

}