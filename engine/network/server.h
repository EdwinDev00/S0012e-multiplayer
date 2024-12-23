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

        std::vector<std::pair<ENetPeer*, Protocol::Player>> users; //CONNECTED USERS

    private:
        ENetHost *m_Server = nullptr;
        ENetPeer *m_Peer = nullptr;
        ENetAddress m_Address = {};
        ENetEvent m_Event = {};

        bool m_Active = false;


        uint64_t GetServerTime();
        void AddPeerUser(ENetPeer* peer); // Adds the connected peer into the list
        void RemovePeerUser(ENetPeer* peer); // Adds the connected peer into the list
        void BroadcastGameState();

        void HandlePacket(ENetPeer* peer, ENetPacket* packet);
        void UpdatePlayerPhysics(ENetPeer* peer, const Protocol::InputC2S* input);
    };
}
