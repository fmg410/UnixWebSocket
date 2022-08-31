#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <algorithm>

struct Client
{
    const char name[20];
    const sf::IpAddress adress;
    const unsigned short port;
    bool operator==(const Client& client)
    {
        return adress == client.adress && port == client.port;
    }
};


void runUdpServer(unsigned short serverPort)
{
    std::cout << "server run\n";
    std::cin.ignore(10000, '\n');
    std::cout << "server run\n";

    sf::UdpSocket socket;

    if (socket.bind(serverPort) != sf::Socket::Done)
        return;

    char buffer[100];

    sf::IpAddress senderIp;
    unsigned short senderPort;
    std::size_t received;

    if (socket.receive(buffer, sizeof(buffer), received, senderIp, senderPort) != sf::Socket::Done)
        return;

    std::cout << "Received message:\n"
              << buffer;

    const char response[] = "I received message\n";

    if (socket.send(response, sizeof(response), senderIp, senderPort) != sf::Socket::Done)
        return;

    std::vector<Client> clients;

    do
    {
        if (socket.receive(buffer, sizeof(buffer), received, senderIp, senderPort) != sf::Socket::Done)
            return;

        Client temp{"temp", senderIp, senderPort};
        if(std::count(clients.begin(), clients.end(), temp) == 0)
        {
            clients.push_back(temp);
        }

        std::cout << "Received message:\n"
                  << buffer << "\nFrom " << clients.at(0).adress << std::endl;
    } while (true);
}

void runUdpClient(unsigned short clientPort, sf::IpAddress serverIp, unsigned short serverPort)
{
    std::cout << "client run";
    std::cin.ignore(10000, '\n');
    std::cout << "client run";

    sf::UdpSocket socket;

    if (socket.bind(clientPort) != sf::Socket::Done)
        return;

    char message[100] = "I am a client";

    if (socket.send(message, sizeof(message), serverIp, serverPort) != sf::Socket::Done)
        return;

    char buffer[100];
    std::size_t received;

    if (socket.receive(buffer, sizeof(buffer), received, serverIp, serverPort) != sf::Socket::Done)
        return;

    std::cout << "Received response:\n"
              << buffer;

    do
    {
        std::cin >> message;
        if (socket.send(message, sizeof(message), serverIp, serverPort) != sf::Socket::Done)
            return;
    } while (true);
}

int main(int argc, char *argv[])
{
    const int serverPort = 50001;
    if (argc == 1)
        runUdpClient(sf::Socket::AnyPort, sf::IpAddress::getLocalAddress(), serverPort);
    else
        runUdpServer(serverPort);

    std::cin.ignore(10000, '\n');
    std::cin.ignore(10000, '\n');

    return 0;
}