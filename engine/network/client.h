#pragma once

//NEW includes
#include "enet/enet.h"
#include "network.h"
#include <unordered_map>

namespace Game
{
   // struct SpaceShip; //REMOVE

    struct ClientSpaceship;
}

//namespace Net {
//    class Client {
//    public:
//        Client() = default;
//
//        ~Client();
//
//        bool Create();
//
//        bool Connect(const char *ip, uint16 port);
//
//        void Disconnect();
//
//        void SendPacket(void *data, size_t size);
//
//        void Poll();
//
//        const bool isActive() { return m_Active; }
//
//        //void setShipTranforms(std::unordered_map<uint32_t, glm::mat4>* transforms)
//        //{
//        //    this->shipTransforms = transforms;
//        //}
//
//        void setShip(std::unordered_map<uint32_t, Game::SpaceShip*>* ships)
//        {
//            this->ships = ships;
//        }
//
//        uint32_t clientControlledID; //TESTING
//
//    private:
//        ENetHost *m_Client = nullptr;
//        ENetPeer *m_Peer = nullptr;
//        ENetAddress m_Address = {};
//        ENetEvent m_Event = {};
//
//
//        //BETTER PERFORMANCE 
//        //std::unordered_map<uint32_t, glm::mat4>* shipTransforms;
//        std::unordered_map<uint32_t, Game::SpaceShip*>* ships;
//
//        //std::vector<glm::mat4>* shiptransforms;
//
//        bool m_Active = false;
//    };
//}
//
////REMOVE
//class Client2 
//{
//public:
//    Client2() = default;
//    ~Client2();
//
//    bool Create();
//    bool Connect(const char* ipAddress, const uint16_t port);
//    void Disconnect();
//    void Poll();
//
//    void Render(); //FOR RENDER OUT THE AVATAR MODEL (SPACESHIP MODEL FOR ALL CONNECTED USERS)
//    //EVENT FOR CLIENT TO SERVER
//    //SEND INPUT PACKET
//    //SEND TEXT PACKET
//    //CLIENT PREDICTION
//
//    bool isHost = false;
//private:
//    ENetHost *m_Client = nullptr;
//    ENetPeer *m_Peer = nullptr;
//
//    bool isActive = false;
//    bool isConnected = false;
//    uint16_t controlledUUID;
//};


//RENEWED CLIENT 
class GameClient
{
public:
    static GameClient& Instance()
    {
        static GameClient instance;
        return instance;
    }

    void Create();
    bool ConnectToServer(const char* ip, const uint16_t port);
    void Update();
    void SendInput(const FlatBufferBuilder& builder); //NETWORK HAS SENDTOSERVER REPRESENT CLIENT SEND PACKAGE TO SERVER
    void RecieveServerUpdate(ENetPacket* packet);
   // void InitalizeScene(std::unordered_map<uint32_t, Game::ClientSpaceship>* spaceShips, std::unordered_map<uint32_t, Laser>* lasers);

    std::unordered_map<uint32_t, Game::ClientSpaceship> spaceships; //all spaceships
    uint32_t myPlayerID = 0; //Player controlled spaceship indentifier
    ENetPeer* GetPeer() const { return peer; }
private:
   // NetworkManager network;
    ENetHost* client;
    ENetPeer* peer; //server peer
    ENetAddress address; //address to the server
    bool isActive = false;

    std::unordered_map<uint32_t, Laser>* lasers = nullptr; // all the lasers fired in the game

    //time 
    uint64_t currentTime = 0;
    uint64_t serverTime = 0;
    uint64_t lastUpdate = 0;

    void OnRecievepacket(ENetPacket* packet);

};

extern GameClient gameClient;