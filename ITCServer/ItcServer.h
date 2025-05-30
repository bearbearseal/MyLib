#ifndef _ITCSERVER_H_
#define _ITCSERVER_H_
#include <vector>
#include <memory>
#include <cstdint>
#include <mutex>
#include <optional>
#include <list>
#include <condition_variable>

class ItcClientId
{
    template <typename, typename>
    friend class ItcServer;

public:
    inline uint32_t get_id() { return id; }

private:
    ItcClientId(uint32_t id_) : id(id_) {}
    uint32_t id;
};

template <typename Message2Server, typename Message2Client>
class ItcClient;

template <typename Message2Server, typename Message2Client>
class ItcServer
{
    template <typename, typename>
    friend class ItcClient;

public:
    struct ServerMessagePair
    {
        ServerMessagePair(const ItcClientId newId, const Message2Server &inMessage) : sourceId(newId), message(inMessage) {}
        ServerMessagePair(const ItcClientId newId, Message2Server &&inMessage) : sourceId(newId)
        {
            message = std::move(inMessage);
        }
        ServerMessagePair(const ServerMessagePair &other) : sourceId(other.sourceId)
        {
            message = other.message;
        }
        ServerMessagePair(ServerMessagePair &&other) : sourceId(other.sourceId)
        {
            message = std::move(other.message);
        }
        ServerMessagePair &operator=(const ServerMessagePair &other) = default;
        ServerMessagePair &operator=(ServerMessagePair &&other) = default;
        ItcClientId sourceId;
        Message2Server message;
    };

    ItcServer() = default;
    ItcServer(ItcServer &) = delete;
    ~ItcServer() = default;

    void send_message(ItcClientId clientId, Message2Client &&message)
    {
        std::unique_ptr<ClientData> &theData = clientsData.clientData[clientId.get_id()];
        std::lock_guard<std::mutex> lock(theData->messageMutex);
        theData->messages.push_back(std::move(message));
        theData->waitCondition.notify_one();
    }
    void send_message(ItcClientId clientId, const Message2Client &message)
    {
        std::unique_ptr<ClientData> &theData = clientsData.clientData[clientId.get_id()];
        std::lock_guard<std::mutex> lock(theData->messageMutex);
        theData->messages.push_back(message);
        theData->waitCondition.notify_one();
    }
    void wait_message()
    {
        // Lock the wait mutex, so that it could be signaled
        std::unique_lock<std::mutex> waitLock(serverData.waitMutex);
        // Check if already got message.
        {
            std::lock_guard<std::mutex> messageLock(serverData.messageMutex);
            if (!serverData.messages.empty())
                return;
        }
        serverData.waitCondition.wait(waitLock);
    }
    bool wait_message(std::chrono::duration<uint64_t, std::micro> timeout)
    {
        // Lock the wait mutex, so that it could be signaled
        std::unique_lock<std::mutex> waitLock(serverData.waitMutex);
        // Check if already got message.
        {
            std::lock_guard<std::mutex> messageLock(serverData.messageMutex);
            if (!serverData.messages.empty())
                return true;
        }
        auto result = serverData.waitCondition.wait_for(waitLock, timeout);
        // Check Result
        if (result == std::cv_status::timeout)
            return false;
        return true;
    }
    std::optional<ServerMessagePair> get_message()
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        if (serverData.messages.empty())
        {
            return {};
        }
        ServerMessagePair retVal = std::move(serverData.messages.front());
        serverData.messages.pop_front();
        return retVal;
    }
    bool has_message()
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        if (serverData.messages.empty())
        {
            return false;
        }
        return true;
    }
    ItcClient<Message2Server, Message2Client> get_client()
    {
        uint32_t nextId = 0;
        {
            std::lock_guard<std::mutex> lockG(clientsData.idMutex);
            if (!clientsData.availableId.empty())
            {
                nextId = clientsData.availableId[clientsData.availableId.size() - 1];
                clientsData.availableId.pop_back();
            }
            else
            {
                nextId = clientsData.nextId;
                ++clientsData.nextId;
                clientsData.clientData.emplace_back(std::move(std::make_unique<ClientData>()));
            }
        }
        return ItcClient<Message2Server, Message2Client>(nextId, *this);
    }

private:
    void client_to_server(ItcClientId clientId, Message2Server &&message)
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        serverData.messages.push_back({clientId, std::move(message)});
        serverData.waitCondition.notify_one();
    }
    void client_to_server(ItcClientId clientId, const Message2Server &message)
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        serverData.messages.push_back({clientId, message});
        serverData.waitCondition.notify_one();
    }
    void return_client_socket_to_pool(ItcClientId clientId)
    {
        {
            std::unique_ptr<ClientData> &theData = clientsData.clientData[clientId.get_id()];
            std::lock_guard<std::mutex> lock(theData->messageMutex);
            theData->messages.clear();
        }
        {
            std::lock_guard<std::mutex> lock(clientsData.idMutex);
            clientsData.availableId.push_back(clientId.get_id());
        }
    }
    bool wait_client_message(ItcClientId clientId, std::chrono::duration<uint64_t, std::micro> timeout)
    {
        std::unique_ptr<ClientData> &theData = clientsData.clientData[clientId.get_id()];
        std::unique_lock<std::mutex> waitLock(theData->waitMutex);
        // Check if already got message
        {
            std::lock_guard<std::mutex> mesageLock(theData->messageMutex);
            if (!theData->messages.empty())
                return true;
        }
        auto result = theData->waitCondition.wait_for(waitLock, timeout);
        // Check Result
        if (result == std::cv_status::timeout)
            return false;
        return true;
    }
    void wait_client_message(ItcClientId clientId)
    {
        std::unique_ptr<ClientData> &theData = clientsData.clientData[clientId.get_id()];
        std::unique_lock<std::mutex> waitLock(theData->waitMutex);
        // Check if already got message.
        {
            std::lock_guard<std::mutex> messageLock(theData->messageMutex);
            if (!theData->messages.empty())
                return;
        }
        theData->waitCondition.wait(waitLock);
    }
    std::optional<Message2Client> get_client_message(ItcClientId clientId)
    {
        std::unique_ptr<ClientData> &theData = clientsData.clientData[clientId.get_id()];
        std::lock_guard<std::mutex> lock(theData->messageMutex);
        if (theData->messages.empty())
        {
            return {};
        }
        Message2Client retVal = std::move(theData->messages.front());
        theData->messages.pop_front();
        return retVal;
    }
    bool client_has_message(ItcClientId clientId)
    {
        std::unique_ptr<ClientData> &theData = clientsData.clientData[clientId.get_id()];
        std::lock_guard<std::mutex> lock(theData->messageMutex);
        if (theData->messages.empty())
        {
            return false;
        }
        return true;
    }

    struct ClientData
    {
        ClientData() {}
        ClientData(const ClientData &) = delete;
        ClientData &operator=(const ClientData &) = delete;
        std::mutex messageMutex;
        std::mutex waitMutex;
        std::list<Message2Client> messages;
        std::condition_variable waitCondition;
    };
    struct
    {
        std::mutex messageMutex;
        std::mutex waitMutex;
        std::list<ServerMessagePair> messages;
        std::condition_variable waitCondition;
    } serverData;
    struct
    {
        std::mutex idMutex;
        uint32_t nextId = 0;
        std::vector<uint32_t> availableId;
        std::vector<std::unique_ptr<ClientData>> clientData;
    } clientsData;
};

template <typename Message2Server, typename Message2Client>
class ItcClient
{
    template <typename, typename>
    friend class ItcServer;

private:
    ItcClient(ItcClientId id_, ItcServer<Message2Server, Message2Client> &master_) : id(id_), master(master_) {}

public:
    ItcClient(const ItcClient &theOther) : id(theOther.id), master(theOther.master) {}
    ~ItcClient() {}
    void send_message(Message2Server &&message)
    {
        master.client_to_server(id, std::move(message));
    }
    void send_message(const Message2Server &message)
    {
        master.client_to_server(id, message);
    }
    void wait_message()
    {
        master.wait_client_message(id);
    }
    bool wait_message(std::chrono::duration<uint64_t, std::micro> timeout)
    {
        return master.wait_client_message(id, timeout);
    }
    std::optional<Message2Client> get_message()
    {
        return master.get_client_message(id);
    }
    bool has_message()
    {
        return master.client_has_message(id);
    }

private:
    ItcClientId id;
    ItcServer<Message2Server, Message2Client> &master;
};
#endif
