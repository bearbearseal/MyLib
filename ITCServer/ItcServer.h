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
    ItcServer(ItcServer&) = delete;
    ~ItcServer() = default;

    void send_message(ItcClientId clientId, Message2Client &&message)
    {
        server_to_client(clientId.get_id(), std::move(message));
    }
    void send_message(ItcClientId clientId, const Message2Client &message)
    {
        server_to_client(clientId.get_id(), message);
    }
    void wait_message()
    {
        wait_server_message();
    }
    bool wait_message(std::chrono::duration<uint64_t, std::micro> timeout)
    {
        return wait_server_message(timeout);
    }
    std::optional<ServerMessagePair> get_message()
    {
        return get_server_message();
    }
    bool has_message()
    {
        return server_has_message();
    }
    ItcClient<Message2Server, Message2Client> get_client()
    {
        uint32_t nextId;
        {
            std::lock_guard<std::mutex> lockG(clientsData.idMutex);
            if(!clientsData.availableId.empty())
            {
                nextId = clientsData.availableId[clientsData.availableId.size() - 1];
                clientsData.availableId.pop_back();
            }
            else
            {
                nextId = clientsData.nextId;
                ++clientsData.nextId;
                ClientData dummy;
                clientsData.clientData.push_back(dummy);
            }
        }
        return ItcClient<Message2Server, Message2Client>(nextId, this);
    }

private:
    void server_to_client(uint32_t clientId, Message2Client &&message)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::lock_guard<std::mutex> lock(theData.messageMutex);
        theData.messages.push_back(std::move(message));
        theData.waitCondition.notify_one();
    }
    void server_to_client(uint32_t clientId, const Message2Client &message)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::lock_guard<std::mutex> lock(theData.messageMutex);
        theData.messages.push_back(message);
        theData.waitCondition.notify_one();
    }
    void client_to_server(uint32_t clientId, Message2Server &&message)
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        serverData.messages.push_back({clientId, std::move(message)});
        serverData.waitCondition.notify_one();
    }
    void client_to_server(uint32_t clientId, const Message2Server &message)
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        serverData.messages.push_back({clientId, message});
        serverData.waitCondition.notify_one();
    }
    void return_client_socket_to_pool(uint32_t clientId)
    {
        {
            ClientData &theData = clientsData.clientData[clientId];
            std::lock_guard<std::mutex> lock(theData.messageMutex);
            theData.messages.clear();
        }
        {
            std::lock_guard<std::mutex> lock(clientsData.idMutex);
            clientsData.availableId.push_back(clientId);
        }
    }
    bool wait_server_message(std::chrono::duration<uint64_t, std::micro> timeout)
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
    void wait_server_message()
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
    bool wait_client_message(uint32_t clientId, std::chrono::duration<uint64_t, std::micro> timeout)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::unique_lock<std::mutex> waitLock(theData.waitMutex);
        // Check if already got message
        {
            std::lock_guard<std::mutex> mesageLock(theData.messageMutex);
            if (!theData.messages.empty())
                return true;
        }
        auto result = theData.waitCondition.wait_for(waitLock, timeout);
        // Check Result
        if (result == std::cv_status::timeout)
            return false;
        return true;
    }
    void wait_client_message(uint32_t clientId)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::unique_lock<std::mutex> waitLock(theData.waitMutex);
        // Check if already got message.
        {
            std::lock_guard<std::mutex> messageLock(theData.messageMutex);
            if (!theData.messages.empty())
                return;
        }
        theData.waitCondition.wait(waitLock);
    }
    std::optional<ServerMessagePair> get_server_message()
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
    std::optional<Message2Client> get_client_message(uint32_t clientId)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::lock_guard<std::mutex> lock(theData.messageMutex);
        if (theData.messages.empty())
        {
            return {};
        }
        Message2Client retVal = std::move(theData.messages.front());
        theData.messages.pop_front();
        return retVal;
    }
    bool server_has_message()
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        if (serverData.messages.empty())
        {
            return false;
        }
        return true;
    }
    bool client_has_message(uint32_t clientId)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::lock_guard<std::mutex> lock(theData.messageMutex);
        if (theData.messages.empty())
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
    struct ClientsData
    {
        std::mutex idMutex;
        uint32_t nextId = 0;
        std::vector<uint32_t> availableId;
        std::vector<ClientData> clientData;
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
    ItcClient(const ItcClient& theOther) : id(theOther.id), master(theOther.master) {}
    ~ItcClient(){}
    void send_message(Message2Server &&message)
    {
        master.client_to_server(id.get_id(), std::move(message));
    }
    void send_message(const Message2Server &message)
    {
        master.client_to_server(id.get_id(), message);
    }
    void wait_message()
    {
        master.wait_client_message(id.get_id());
    }
    bool wait_message(std::chrono::duration<uint64_t, std::micro> timeout)
    {
        return master.wait_client_message(id.get_id(), timeout);
    }
    std::optional<Message2Client> get_message()
    {
        return master.get_client_message(id.get_id());
    }
    bool has_message()
    {
        return master.client_has_message(id.get_id());
    }

private:
    ItcClientId id;
    ItcServer<Message2Server, Message2Client> &master;
};
#endif

/*
template <typename Message2Server, typename Message2Client>
class ItcServer
{
    friend ItcClient;
public:
    class ClientId
    {
        friend class ItcServer;

    public:
        inline uint32_t get_id() { return id; }

    private:
        ClientId(uint32_t id_) : id(id_) {}
        uint32_t id;
    };
    struct ServerMessagePair
    {
        ServerMessagePair(const ClientId newId, const Message2Server &inMessage) : sourceId(newId), message(inMessage) {}
        ServerMessagePair(const ClientId newId, Message2Server &&inMessage) : sourceId(newId)
        {
            message = std::move(inMessage);
        }
        ServerMessagePair(const ServerMessagePair &other) : sourceId(other.sourceId.get_id())
        {
            message = other.message;
        }
        ServerMessagePair(ServerMessagePair &&other) : sourceId(other.sourceId.get_id())
        {
            message = std::move(other.message);
        }
        ServerMessagePair& operator=(const ServerMessagePair &other)
        {
            sourceId = other.sourceId;
            message = other.message;
            return *this;
        }
        ServerMessagePair& operator=(ServerMessagePair&& other)
        {
            sourceId = other.sourceId;
            message = move(other.message);
            return *this;
        }
        ClientId sourceId;
        Message2Server message;
    };

    void send_message(ClientId clientId, Message2Client &&message)
    {
        server_to_client(clientId.get_id(), std::move(message));
    }
    void send_message(ClientId clientId, const Message2Client &message)
    {
        server_to_client(clientId.get_id(), message);
    }
    void wait_message()
    {
        wait_server_message();
    }
    bool wait_message(std::chrono::duration<uint64_t, std::micro> timeout)
    {
        return wait_server_message(timeout);
    }
    std::optional<ServerMessagePair> get_message()
    {
        return get_server_message();
    }
    bool has_message()
    {
        return has_server_message();
    }
    ItcClient<Message2Server, Message2Client> get_client()
    {
        uint32_t nextId;
        {
            lock_guard<mutex> lockG(clientsData.idMutex);
            nextId = clientsData.nextId;
            ++clientsData.nextId;
        }
        return ItcClient<Message2Server, Message2Client>(nextId, this);
    }

private:
    void server_to_client(uint32_t clientId, Message2Client &&message)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::lock_guard<std::mutex> lock(theData.messageMutex);
        theData.messages.push_back(std::move(message));
        theData.waitCondition.notify_one();
    }
    void server_to_client(uint32_t clientId, const Message2Client &message)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::lock_guard<std::mutex> lock(theData.messageMutex);
        theData.messages.push_back(message);
        theData.waitCondition.notify_one();
    }
    void client_to_server(uint32_t clientId, Message2Server &&message)
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        serverData.messages.push_back({clientId, std::move(message)});
        serverData.waitCondition.notify_one();
    }
    void client_to_server(uint32_t clientId, const Message2Server &message)
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        serverData.messages.push_back({clientId, message});
        serverData.waitCondition.notify_one();
    }
    void return_client_socket_to_pool(uint32_t clientId)
    {
        {
            ClientData &theData = clientsData.clientData[clientId];
            std::lock_guard<std::mutex> lock(theData.messageMutex);
            theData.messages.clear();
        }
        {
            std::lock_guard<std::mutex> lock(clientsData.idMutex);
            clientsData.availableId.push_back(clientId);
        }
    }
    bool wait_server_message(std::chrono::duration<uint64_t, std::micro> timeout)
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
    void wait_server_message()
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
    bool wait_client_message(uint32_t clientId, std::chrono::duration<uint64_t, std::micro> timeout)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::unique_lock<std::mutex> waitLock(theData.waitMutex);
        // Check if already got message
        {
            std::lock_guard<std::mutex> mesageLock(theData.messageMutex);
            if (!theData.messages.empty())
                return true;
        }
        auto result = theData.waitCondition.wait_for(waitLock, timeout);
        // Check Result
        if (result == std::cv_status::timeout)
            return false;
        return true;
    }
    void wait_client_message(uint32_t clientId)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::unique_lock<std::mutex> waitLock(theData.waitMutex);
        // Check if already got message.
        {
            std::lock_guard<std::mutex> messageLock(theData.messageMutex);
            if (!theData.messages.empty())
                return;
        }
        theData.waitCondition.wait(waitLock);
    }
    std::optional<ServerMessagePair> get_server_message()
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
    std::optional<Message2Client> get_client_message(uint32_t clientId)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::lock_guard<std::mutex> lock(theData.messageMutex);
        if (theData.messages.empty())
        {
            return {};
        }
        Message2Client retVal = std::move(theData.messages.front());
        theData.messages.pop_front();
        return retVal;
    }
    bool server_has_message()
    {
        std::lock_guard<std::mutex> lock(serverData.messageMutex);
        if (serverData.messages.empty())
        {
            return false;
        }
        return true;
    }
    bool client_has_message(uint32_t clientId)
    {
        ClientData &theData = clientsData.clientData[clientId];
        std::lock_guard<std::mutex> lock(theData.messageMutex);
        if (theData.messages.empty())
        {
            return false;
        }
        return true;
    }

    struct ClientData
    {
        ClientData() {}
        ClientData(const ClientData&) = delete;
        ClientData& operator=(const ClientData&) = delete;
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
    struct ClientsData
    {
        std::mutex idMutex;
        uint32_t nextId = 0;
        std::vector<uint32_t> availableId;
        std::vector<ClientData> clientData;
    } clientsData;

};

template <typename Message2Server, typename Message2Client>
class ItcClient
{
    friend class ItcServer;
private:
    ItcClient(ItcServer::ClientId id_, ItcServer* master_) : id(id_), master(*master_) {}

public:
    ItcClient(const ItcClient& theOther) : id(theOther.id), master(theOther.master) {}
    ~ItcClient(){}
    void send_message(Message2Server &&message)
    {
        master.client_to_server(id.get_id(), std::move(message));
    }
    void send_message(const Message2Server &message)
    {
        master.client_to_server(id.get_id(), message);
    }
    void wait_message()
    {
        master.wait_client_message(id.get_id());
    }
    bool wait_message(std::chrono::duration<uint64_t, std::micro> timeout)
    {
        return master.wait_client_message(id.get_id(), timeout);
    }
    std::optional<Message2Client> get_message()
    {
        return master.get_client_message(id.get_id());
    }
    bool has_message()
    {
        return master.client_has_message(id.get_id());
    }

private:
    ClientId id;
    ItcServer& master;
};
*/
