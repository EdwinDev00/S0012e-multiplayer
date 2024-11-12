#pragma once
#include "enet/enet.h"

namespace Net {
    class Client {
    public:
        Client() = default;

        ~Client();

        bool Create();

        bool Connect(const char *ip, uint16 port);

        void Disconnect();

        void SendPacket(void *data, size_t size);

        void Poll();

    private:
        ENetHost *m_Client = nullptr;
        ENetPeer *m_Peer = nullptr;
        ENetAddress m_Address = {};
        ENetEvent m_Event = {};

        bool m_Active = false;
    };
}
