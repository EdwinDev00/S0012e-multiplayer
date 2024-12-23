#pragma once
#include "enet/enet.h"
#include <unordered_map>

namespace Game
{
    struct SpaceShip;
}

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

        const bool isActive() { return m_Active; }

        //void setShipTranforms(std::unordered_map<uint32_t, glm::mat4>* transforms)
        //{
        //    this->shipTransforms = transforms;
        //}

        void setShip(std::unordered_map<uint32_t, Game::SpaceShip*>* ships)
        {
            this->ships = ships;
        }

        uint32_t clientControlledID; //TESTING

    private:
        ENetHost *m_Client = nullptr;
        ENetPeer *m_Peer = nullptr;
        ENetAddress m_Address = {};
        ENetEvent m_Event = {};


        //BETTER PERFORMANCE 
        //std::unordered_map<uint32_t, glm::mat4>* shipTransforms;
        std::unordered_map<uint32_t, Game::SpaceShip*>* ships;

        //std::vector<glm::mat4>* shiptransforms;

        bool m_Active = false;
    };
}
