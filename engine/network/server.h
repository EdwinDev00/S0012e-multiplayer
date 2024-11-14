#pragma once
#include "enet/enet.h"
#include <vector>
#include <unordered_map>
#include "../projects/spacegame/code/spaceship.h" //move that spaceship h file closer in bin or engine network

namespace Net {
    class Server {
    public:
        Server() = default;

        ~Server();

        bool Create(uint16 port);

        void Poll();

    private:
        ENetHost *m_Server = nullptr;
        ENetPeer *m_Peer = nullptr;
        ENetAddress m_Address = {};
        ENetEvent m_Event = {};

        std::vector<ENetPeer*> peers;
        std::unordered_map<uint32_t, Game::SpaceShip> spaceships;

        bool m_Active = false;

        void AddPeerUser(ENetPeer* user); // Adds the connected peer into the list
        void BroadcastGameState();
    };
}
