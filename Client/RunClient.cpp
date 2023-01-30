#include "RunClient.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>

#include <vector>
#include <algorithm>
#include <type_traits>
#include <string>
#include <limits>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include "Message.hpp"
#include "MessageManager.hpp"
#include "Client.hpp"
#include "../Authenticator.hpp"

void messageSender(std::vector<sf::Packet>& toSend, std::mutex& MToSend, std::condition_variable& VToSend, sf::UdpSocket& socket, std::mutex& MSocket, const sf::IpAddress& serverIp, unsigned short serverPort, std::atomic<bool>& isRunning)
{
    std::unique_lock<std::mutex> lock(MToSend);
    while(isRunning)
    {
        if(toSend.empty())
            VToSend.wait(lock, [&](){ return !toSend.empty(); });
        for(auto&& packet : toSend)
        {
            if (socket.send(packet, serverIp, serverPort) != sf::Socket::Done)
                return;
        }
        toSend.clear();
    }
}

void messageReceiver(MessageManager& messageManager, std::mutex& MMessageManager, std::vector<std::string>& clientNames, std::mutex& MClientNames, std::vector<std::string>& receivedRecently, std::mutex& MReceivedRecently, sf::UdpSocket& socket, std::mutex& MSocket, const sf::IpAddress& serverIp, unsigned short serverPort, std::atomic<bool>& isRunning)
{
    while(isRunning)
    {
        sf::Packet receivePacket;

        while(isRunning)
        {
            std::unique_lock<std::mutex> lock(MSocket);
            sf::IpAddress tempA;
            unsigned short tempP;
            sf::SocketSelector selector;
            selector.add(socket);
            if (selector.wait(sf::seconds(1.f)))
                socket.receive(receivePacket, tempA, tempP);
            else
                continue;
            if(tempA == serverIp && tempP == serverPort)
                break;
        }

        if(!validatePacket(receivePacket))
            continue;

        std::underlying_type_t<ServerMessageType> messageTypeTemp;
            if (!(receivePacket >> messageTypeTemp))
                continue;

            ServerMessageType messageType = static_cast<ServerMessageType>(messageTypeTemp);

            if (messageType == ServerMessageType::MESSAGE_FROM_SOMEONE)
            {
                std::string senderName;

                if (!(receivePacket >> senderName))
                    continue;

                std::string receivedMessage;

                if (!(receivePacket >> receivedMessage))
                    continue;

                std::unique_lock<std::mutex> lock(MMessageManager);

                messageManager.receivedMessage(senderName, receivedMessage);

                std::unique_lock<std::mutex> lock2(MReceivedRecently);

                if(!std::count(receivedRecently.begin(), receivedRecently.end(), senderName))
                {
                    receivedRecently.push_back(senderName);
                }

            }
            else if(messageType == ServerMessageType::CLIENT_LIST)
            {
                unsigned int size = 0;
                if (!(receivePacket >> size))
                    continue;

                std::string name;
                std::vector<std::string> names;

                for (int i = 0; i < size; i++)
                {
                    if (!(receivePacket >> name))
                        continue;
                    names.push_back(name);
                }

                std::sort(names.begin(), names.end());

                std::unique_lock<std::mutex> lock(MClientNames);

                clientNames = names;

                /* for(auto&& n : names)
                {
                    if(std::count(clientNames.begin(), clientNames.end(), n) == 0)
                        clientNames.push_back(n);
                } */
            }

    }

}

void listUpdater(std::vector<sf::Packet>& toSend, std::mutex& MToSend, std::condition_variable& VToSend, const Client& self, std::atomic<bool>& isRunning)
{
    using namespace std::chrono_literals;
    while(isRunning)
    {
        std::this_thread::sleep_for(3s);
        std::unique_lock<std::mutex> lock(MToSend);
        sf::Packet preparePacket;
        preparePacket << appId_ << static_cast<std::underlying_type_t<ClientMessageType>>(ClientMessageType::CLIENTS_LIST_REQUEST) << self.name << self.password;
        toSend.push_back(preparePacket);
        lock.unlock();
        VToSend.notify_one();
    }
}

void runUdpClient(unsigned short clientPort, const sf::IpAddress serverIp, unsigned short serverPort)
{
    sf::UdpSocket socket;
    std::mutex MSocket;

    if (socket.bind(clientPort) != sf::Socket::Done)
        return;

    std::vector<std::string> receivedRecently;
    std::mutex MReceivedRecently;

    std::vector<sf::Packet> toSend;
    std::mutex MToSend;
    std::condition_variable VToSend;

    MessageManager messageManager;
    std::mutex MMessageManager;

    std::vector<std::string> clientNames;
    std::mutex MClientNames;

    std::atomic<bool> isRunning;
    isRunning = true;

    std::string tempName;
    std::string tempPassword;
    std::cout << "Podaj nazwe uzytkownika: " << std::flush;
    std::getline(std::cin, tempName);
    std::cout << "Podaj haslo: " << std::flush;
    std::getline(std::cin, tempPassword);

    const Client self{tempName, tempPassword, sf::IpAddress::Any, clientPort};

    std::thread sender(messageSender, std::ref(toSend), std::ref(MToSend), std::ref(VToSend), std::ref(socket), std::ref(MSocket), std::cref(serverIp), serverPort, std::ref(isRunning));
    std::thread receiver(messageReceiver, std::ref(messageManager), std::ref(MMessageManager), std::ref(clientNames), std::ref(MClientNames), std::ref(receivedRecently), std::ref(MReceivedRecently), std::ref(socket), std::ref(MSocket), std::cref(serverIp), serverPort, std::ref(isRunning));
    std::thread updater(listUpdater, std::ref(toSend), std::ref(MToSend), std::ref(VToSend), std::cref(self), std::ref(isRunning));

    {
        sf::Packet packet;
        preparePacket(packet);
        packet << static_cast<std::underlying_type_t<ClientMessageType>>(ClientMessageType::HANDSHAKE) << self.name << self.password;
        if (socket.send(packet, serverIp, serverPort) != sf::Socket::Done)
            return;
    }


    do
    {
        int choice = 0;
        while (true)
        {
            clearConsole;
            std::cout << "Nazwa uzytkownika: " << self.name << "\n\n" << "1. Pokaz liste uzytkownikow\n2. Wyslij wiadomosc\n3. Sprawdz wiadomosci\n4. Wyswietl ostatnie otrzymane wiadomosci\n5. Wyjdz z programu" << std::endl;
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
            clearConsole;
            std::cout << "Dostepne osoby:" << std::endl;
            std::unique_lock<std::mutex> lock (MClientNames);
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
            std::unique_lock<std::mutex> lockN (MClientNames);
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

            std::unique_lock<std::mutex> Zlock(MMessageManager);
            messageManager.sentMessage(targetName, message);
            Zlock.unlock();

            sf::Packet preparePacket;
            preparePacket << appId_ << static_cast<std::underlying_type_t<ClientMessageType>>(ClientMessageType::MESSAGE_TO_CLIENT) << self.name << self.password << targetName << message;

            std::unique_lock<std::mutex> lock(MToSend);
            toSend.push_back(preparePacket);
            lock.unlock();
            VToSend.notify_one();

        }
        else if (choice == 3)
        {
            clearConsole;
            std::cout << "Wpisz numer osoby, ktorej rozmowe chcesz wyswietlic:" << std::endl;
            int i = 1;

            std::vector<std::string> conversants;
            std::unique_lock<std::mutex> lockN (MMessageManager);
            for (auto &conversation : messageManager.getConversationViewC())
            {
                conversants.push_back(conversation.getPerson());
            }
            lockN.unlock();

            for(auto& person : conversants)
            {
                std::cout << i++ << ". " << person << '\n';
            }
            std::cout << std::flush;

            std::string chosenClientNumber;
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

            if(!(num >= 0 && num < conversants.size()))
                continue;

            std::string targetName = conversants.at(num);

            lockN.lock();
            for(auto& message : messageManager.getMessageViewC(targetName))
            {
                if(message.sendBySelf)
                    std::cout << self.name << ": " << message.contents << "\n\n";
                else
                    std::cout << targetName << ": " << message.contents << "\n\n";
            }
            std::cout << std::flush;

            lockN.unlock();

            std::unique_lock<std::mutex> lockL(MReceivedRecently);
            auto itr = std::find_if(receivedRecently.begin(), receivedRecently.end(), [&](const std::string& name){ return name == targetName; });
            if(itr != receivedRecently.end())
            {
                receivedRecently.erase(itr);
            }
            lockL.unlock();

            std::cin.ignore(10000, '\n');
        }
        else if (choice == 4)
        {
            clearConsole;

            if(!receivedRecently.size())
            {
                std::cout << "Brak nowych wiadomosci" << std::endl;
                std::cin.ignore(10000, '\n');
                continue;
            }

            std::cout << "Ostatnie osoby, ktore wyslaly ci wiadomosc, wpisz numer ktora rozmowe chcesz wyswietlic:" << std::endl;
            int i = 1;

            std::unique_lock<std::mutex> lockL(MReceivedRecently);;

            for(auto& r : receivedRecently)
            {
                std::cout << i++ << ". " << r << '\n';
            }
            std::cout << std::flush;

            std::string chosenClientNumber;
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

            if(!(num >= 0 && num < receivedRecently.size()))
                continue;

            std::string targetName = receivedRecently.at(num);
            auto itrr = receivedRecently.begin() + num;
            receivedRecently.erase(itrr);
            lockL.unlock();

            std::unique_lock<std::mutex> lock(MMessageManager);
            for(auto& message : messageManager.getMessageViewC(targetName))
            {
                if(message.sendBySelf)
                    std::cout << self.name << ": " << message.contents << "\n\n";
                else
                    std::cout << targetName << ": " << message.contents << "\n\n";
            }
            lock.unlock();
            std::cin.ignore(10000, '\n');
        }
        else if (choice == 5)
        {
            isRunning = false;
        }

    } while (isRunning);

    sender.join();
    receiver.join();
    updater.join();
}