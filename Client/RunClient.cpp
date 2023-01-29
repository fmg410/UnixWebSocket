#include "RunClient.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>

#include <vector>
#include <algorithm>
#include <type_traits>
#include <string>
#include <limits>


void runUdpClient(unsigned short clientPort, sf::IpAddress serverIp, unsigned short serverPort)
{
    std::cout << "Client run" << std::endl;

    sf::UdpSocket socket;

    if (socket.bind(clientPort) != sf::Socket::Done)
        return;

    std::string userName;
    std::string userPassword = "123";
    std::cout << "Podaj nazwe uzytkownika: ";
    std::getline(std::cin, userName);
    {
        sf::Packet preparePacket;
        preparePacket << appId_ << static_cast<std::underlying_type_t<ClientMessageType>>(ClientMessageType::HANDSHAKE) << userName << userPassword;
        if (socket.send(preparePacket, serverIp, serverPort) != sf::Socket::Done)
            return;
    }

    std::vector<std::string> clientNames;

    do
    {
        int choice = 0;
        while (true)
        {
            clearConsole;
            std::cout << "1. Odswiez i pokaz liste uzytkownikow\n2. Wyslij wiadomosc\n3. Sprawdz wiadomosci" << std::endl;
            std::string temp;
            std::getline(std::cin, temp);

            try
            {
                choice = std::stoi(temp);
            }
            catch (...)
            {
                continue;
            }

            break;
        }

        if (choice == 1)
        {
            sf::Packet preparePacket;
            preparePacket << appId_ << static_cast<std::underlying_type_t<ClientMessageType>>(ClientMessageType::CLIENTS_LIST_REQUEST) << userName;
            socket.send(preparePacket, serverIp, serverPort);
            sf::Packet clientListPacket;
            sf::SocketSelector selector;
            selector.add(socket);
            if (selector.wait(sf::seconds(10.f)))
            {
                socket.receive(clientListPacket, serverIp, serverPort);
            }
            else
            {
                std::cout << "Nie udalo sie pobrac listy uzytkownikow z serwera" << std::endl;
                continue;
            }

            int32_t appId;

            if (!(clientListPacket >> appId && appId == appId_))
                continue;

            std::underlying_type_t<ServerMessageType> messageTypeTemp;
            if (!(clientListPacket >> messageTypeTemp))
                continue;

            ServerMessageType messageType = static_cast<ServerMessageType>(messageTypeTemp);

            if (messageType != ServerMessageType::CLIENT_LIST)
                continue;

            unsigned int clientNumber = 0;

            if (!(clientListPacket >> clientNumber))
                continue;

            bool success = true;
            std::vector<std::string> tempClientNames;

            for (int i = 0; i < clientNumber; i++)
            {
                std::string name;
                if (!(clientListPacket >> name))
                {
                    success = false;
                    break;
                }
                tempClientNames.push_back(name);
            }
            if (success)
                clientNames = tempClientNames;

            clearConsole;
            std::cout << "Dostepne osoby:" << std::endl;
            for (auto &clientName : clientNames)
            {
                std::cout << clientName << std::endl;
            }
            std::cin.ignore(10000, '\n');
        }
        else if (choice == 2)
        {
            clearConsole;
            std::cout << "Wpisz numer osoby, do ktorej chcesz wyslac wiadomosc:" << std::endl;
            int i = 1;
            for (auto &clientName : clientNames)
            {
                std::cout << i++ << ". " << clientName << std::endl;
            }

            std::string chosenClientNumber;
            std::string targetName;
            int num;

            std::getline(std::cin, chosenClientNumber);
            try
            {
                num = std::stoi(chosenClientNumber);
                num--;
            }
            catch (...)
            {
                continue;
            }

            if (num >= 0 && num < clientNames.size())
                targetName = clientNames.at(num);
            else
                continue;

            clearConsole;
            std::cout << "Napisz wiadomosc:" << std::endl;

            std::string message;
            std::getline(std::cin, message);

            sf::Packet preparePacket;
            preparePacket << appId_ << static_cast<std::underlying_type_t<ClientMessageType>>(ClientMessageType::MESSAGE_TO_CLIENT) << userName << targetName << message;

            if (socket.send(preparePacket, serverIp, serverPort) != sf::Socket::Done)
                return;

            std::cout << "Wyslano wiadomosc pomyslnie" << std::endl;
            std::cin.ignore(10000, '\n');
        }
        else if (choice == 3)
        {
            std::string messages;
            while (true)
            {
                sf::Packet receivePacket;
                sf::SocketSelector selector;
                selector.add(socket);
                if (selector.wait(sf::seconds(4.f)))
                {
                    socket.receive(receivePacket, serverIp, serverPort);
                }
                else
                {
                    std::cout << "Nie udalo sie pobrac listy uzytkownikow z serwera" << std::endl;
                    break;
                }

                int32_t appId;

                if (!(receivePacket >> appId && appId == appId_))
                    continue;

                std::underlying_type_t<ServerMessageType> messageTypeTemp;
                if (!(receivePacket >> messageTypeTemp))
                    continue;

                ServerMessageType messageType = static_cast<ServerMessageType>(messageTypeTemp);

                if (messageType != ServerMessageType::MESSAGE_FROM_SOMEONE)
                    continue;

                std::string senderName;

                if (!(receivePacket >> senderName))
                    continue;

                std::string receivedMessage;

                if (!(receivePacket >> receivedMessage))
                    continue;

                messages.append(senderName);
                messages.append(": ");
                messages.append(receivedMessage);
                messages.append("\n");
            }

            if (messages.length() > 0)
            {
                clearConsole;
                std::cout << messages;
                std::cin.ignore(10000, '\n');
            }
        }
        else if (choice == 4)
        {
            std::cout << "Server run with local ip: " << sf::IpAddress::getLocalAddress() << "\nand public ip: " << sf::IpAddress::getPublicAddress() << "\nclient port: " << socket.getLocalPort() << std::endl;
            std::cin.ignore(10000, '\n');
        }

    } while (true);
}