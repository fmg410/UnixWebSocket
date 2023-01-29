#include "MessageManager.hpp"
#include <algorithm>

void MessageManager::addMessage(std::string person, std::string contents, bool sentBySelf)
{
    auto itr = std::find_if(conversations.begin(), conversations.end(), [&](const Conversation& c){return c.getPerson() == person;});
    if(itr == conversations.end())
        throw 1;
    itr->addMessage({contents, sentBySelf});
}

void MessageManager::receivedMessage(std::string person, std::string contents)
{
    addMessage(person, contents, false);
}

void MessageManager::sentMessage(std::string person, std::string contents)
{
    addMessage(person, contents, true);
}

const std::vector<Conversation>& MessageManager::getConversationViewC()
{
    return conversations;
}

const std::vector<Message>& MessageManager::getMessageViewC(std::string person)
{
    auto itr = std::find_if(conversations.begin(), conversations.end(), [&](const Conversation& c){return c.getPerson() == person;});
    if(itr == conversations.end())
        throw 1;
    return itr->getMessageViewC();
}

bool MessageManager::isConversation(std::string person)
{
    auto itr = std::find_if(conversations.begin(), conversations.end(), [&](const Conversation& c){return c.getPerson() == person;});
    if(itr == conversations.end())
        return false;
    return true;
}