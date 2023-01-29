#ifndef CONFIG
#define CONFIG

#ifdef _WIN32
#define clearConsole system("cls")
#elif __linux__
#define clearConsole system("clear")
#elif __APPLE__
#define clearConsole system("clear")
#else
#define clearConsole printf("\n\n\n")
#endif

static const int appId_ = 518906;

enum class ClientMessageType
{
    HANDSHAKE = 1,
    CLIENTS_LIST_REQUEST = 2,
    MESSAGE_TO_CLIENT = 3
};

enum class ServerMessageType
{
    CLIENT_LIST = 1,
    MESSAGE_FROM_SOMEONE = 2
};

#endif