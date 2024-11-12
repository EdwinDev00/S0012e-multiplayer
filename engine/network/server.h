#pragma once
#include "enet/enet.h"

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

        bool m_Active = false;
    };
}
