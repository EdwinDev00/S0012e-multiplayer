#pragma once
#include "enet/enet.h"
#include <vector>
#include <unordered_map>
#include "proto.h"

namespace Net {
    class Server {
    public:
        Server() = default;

        ~Server();

        bool Create(uint16 port);

        void Poll();

        std::vector<std::pair<ENetPeer*, Protocol::Player>> users;

    private:
        ENetHost *m_Server = nullptr;
        ENetPeer *m_Peer = nullptr;
        ENetAddress m_Address = {};
        ENetEvent m_Event = {};

        bool m_Active = false;

        void AddPeerUser(ENetPeer* user); // Adds the connected peer into the list
        void BroadcastGameState();
        void BroadcastPacket(void* data, size_t size);
    };
}
