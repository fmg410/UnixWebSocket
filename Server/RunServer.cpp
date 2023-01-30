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
#include "../Authenticator.hpp"


void runUdpServer(unsigned short serverPort)
{
    // std::cout << "Server run with local ip: " << sf::IpAddress::getLocalAddress() << "\nand public ip: " << sf::IpAddress::getPublicAddress() << std::endl;

    Server server(serverPort);

    sf::UdpSocket socket;

    if (socket.bind(serverPort) != sf::Socket::Done)
        return;

    sf::Packet receivedPacket;
    sf::IpAddress senderIp;
    unsigned short senderPort;
    std::size_t received;

    do
    {
        if(server.size() > 0)
        {
            clearConsole;
            std::cout << "Client list:\n";
            for(auto& c : server.getClientViewC())
                std::cout << c.name << '\n';
            std::cout << std::flush;
        }

        if (socket.receive(receivedPacket, senderIp, senderPort) != sf::Socket::Done)
            return;

        int32_t appId;

        if (!validatePacket(receivedPacket))
            continue;

        std::underlying_type_t<ClientMessageType> messageTypeTemp;
        if (!(receivedPacket >> messageTypeTemp))
            continue;

        ClientMessageType messageType = static_cast<ClientMessageType>(messageTypeTemp);

        std::string senderName;
        std::string senderPassword;

        if (!(receivedPacket >> senderName))
            continue;
        if (!(receivedPacket >> senderPassword))
            continue;

        Client tempClient{senderName, senderPassword, senderIp, senderPort};
        if(server.isClient(senderName))
        {
            if(server.getClientFromName(senderName).password != senderPassword)
                continue;

            if(!(server.getClientFromName(senderName).adress == senderIp && server.getClientFromName(senderName).port == senderPort))
            {
                server.removeClient(senderName);
                server.addClient(tempClient);
            }
        }
        else
        {
            server.addClient(tempClient);
        }

        if (messageType == ClientMessageType::HANDSHAKE)
        {
            continue;
        }
        else if (messageType == ClientMessageType::CLIENTS_LIST_REQUEST)
        {
            sf::Packet preparedPacket;
            preparePacket(preparedPacket);
            preparedPacket << static_cast<std::underlying_type_t<ServerMessageType>>(ServerMessageType::CLIENT_LIST) << static_cast<unsigned int>(server.size() - 1);
            for (auto &client : server.getClientViewC())
            {
                if (client.name != tempClient.name)
                    preparedPacket << client.name;
            }
            if(preparedPacket.getDataSize() < socket.MaxDatagramSize)
                socket.send(preparedPacket, senderIp, senderPort);
        }
        else if (messageType == ClientMessageType::MESSAGE_TO_CLIENT)
        {
            std::string targetName;
            if (!(receivedPacket >> targetName))
                continue;

            if(!server.isClient(targetName))
                continue;

            std::string message;

            if (!(receivedPacket >> message))
                continue;

            sf::Packet preparedPacket;
            preparePacket(preparedPacket);
            preparedPacket << static_cast<std::underlying_type_t<ServerMessageType>>(ServerMessageType::MESSAGE_FROM_SOMEONE) << tempClient.name << message;

            if(preparedPacket.getDataSize() < socket.MaxDatagramSize)
                socket.send(preparedPacket, server.getClientFromName(targetName).adress, server.getClientFromName(targetName).port);
        }

    } while (true);

}