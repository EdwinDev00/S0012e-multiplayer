#pragma once
//#include <vector>
//#include "proto.h"

//NEW INCLUDE
#include "enet/enet.h"
#include "network.h"
#include <unordered_map>

#include "../projects/spacegame/code/spaceship.h"

namespace Game
{
   struct ServerSpaceship;
}

//namespace Net {
//    class Server {
//    public:
//        Server() = default;
//
//        ~Server();
//
//        bool Create(uint16 port);
//
//        void Poll();
//
//        std::vector<std::pair<ENetPeer*, Protocol::Player>> users; //CONNECTED USERS
//
//    private:
//        ENetHost *m_Server = nullptr;
//        ENetPeer *m_Peer = nullptr;
//        ENetAddress m_Address = {};
//        ENetEvent m_Event = {};
//
//        bool m_Active = false;
//
//
//        uint64_t GetServerTime();
//        void AddPeerUser(ENetPeer* peer); // Adds the connected peer into the list
//        void RemovePeerUser(ENetPeer* peer); // Adds the connected peer into the list
//        void BroadcastGameState();
//
//        void HandlePacket(ENetPeer* peer, ENetPacket* packet);
//        void UpdatePlayerPhysics(ENetPeer* peer, const Protocol::InputC2S* input);
//    };
//}

struct SpawnPoint
{
    glm::vec3 position = glm::vec3(0);
    bool occupied = false;
    uint32_t ownerID = 0; //who is using this spawnpoint

    SpawnPoint* GetAvailableSpawnpoint()
    {
        if (!occupied)
            return this; //if not occupied return this spawnpoint
        return nullptr; //otherwise not available spawnpoint 
    }

    glm::quat calcOrientationToOrigin()
    {
        glm::vec3 directionToOrigin = normalize(-position);
        return glm::quat(glm::vec3(0, 0, 1),directionToOrigin);
    }
};

//Converter serverspaceship into a player

using namespace Protocol;

class GameServer
{
public:

    static GameServer& instance()
    {
        static GameServer instance;
        return instance;
    }

    void StartServer(uint16_t port = 1234);
    void Run();
    void Update(float dt = 0.016f);

    GameServer() = default;
    ~GameServer();

    //FOR CHECKING IF THE SERVER POSITION OF SPACESHIP UPDATES
    
    std::unordered_map<uint32_t, Game::ServerSpaceship> GetServerShip() const { return players; }

private:
   
    //ENET / NETWORKING
    void InitNetwork(uint16_t port);
    void PollNetworkEvents();
    void OnClientConnect(ENetPeer* peer);
    void OnClientDisconnect(uint32_t clientID);
    void OnPacketRecieved(uint32_t senderID, const ENetPacket* packet);

    //GAMEPLAY
    void ProcessPlayerInput(uint32_t clientID, const InputC2S* packet);
    void SpawnPlayer(uint32_t clientID);
    void RemovePlayer(uint32_t clientID);

    //UTILITIY
    Player BatchShip(const Game::ServerSpaceship& ship) const;
    Laser BatchLaser(const Laser& laser) const;

    //SERVER STATE
    ENetHost* server;
    uint32_t serverPort;
    bool live = false;

    uint32_t nextClientID = 1; //Auto increment ID for new player
    uint64_t s_currentTime = 0; //current server time (ms)
    int serverTickCounter = 0;
    const int sendRate = 5; // every 5 physics tick

    //Server time related  (general time related)
    
    //queue system for managing the incoming packages

    //CONNECTED USERS (CLIENTS)
    std::unordered_map<ENetPeer*, uint32_t> connections;

    //GAME STATE
    Physics::ColliderMeshId playerMeshColliderID;
    std::unordered_map<uint32_t, Game::ServerSpaceship> players; //AMount of player ship is registered in the server (for handling updates and changes)
    std::unordered_map<uint32_t, Physics::ColliderId> playerColliders; //Colliders for the players spaceship
    std::unordered_map<uint32_t, Laser> lasers; // All the registered laser in the server
    //std::queue<uint32_t> laserToRemove; //Mark for the laser to be removed

    //Game related
    //std::vector<Physics::ColliderId> asteroidsColliders; //collider ID of the asterroids in the game
    std::array<SpawnPoint, 32> spawnpoints; //SpawnPoints of player spawn (ENet set to max 32)

    //ADD PREVENT COPY/MOVE OPERATOR FOR OUR INSTANCE(ENSURE ONLY SINGLE INSTANCE EXIST)




    //void HandleIncomingPacket(uint32_t senderID, const PacketWrapper* packet);

};

extern GameServer gameServer;