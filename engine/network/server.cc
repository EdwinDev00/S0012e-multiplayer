#include "config.h"
#include "server.h"
#include "network.h"

#include <bitset>
#include <chrono>

//SERVER APPLICATION

namespace Net {
    bool
    Server::Create(const uint16 port) {
        m_Address.host = ENET_HOST_ANY;
        m_Address.port = port;

        m_Server = enet_host_create(&m_Address, 32, 1, 0, 0);
        if (!m_Server) {
            std::cout << "Failed to create ENet client host.\n";
            return false;
        }

        LOG("Successfully created ENet server host.\n");
        m_Active = true;
        return true;
    }

    Server::~Server() {
        LOG("Destroyed server.\n");
        enet_host_destroy(m_Server);
    }

    uint64_t Server::GetServerTime() {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return duration.count();
    }

    void Server::AddPeerUser(ENetPeer* peer) //ON PLAYER CONNECTED
    {
        static uint32_t nextUUID = 0;
        uint32_t newUUID = nextUUID++;

        static int32 nextOffsetX = 0;
        int newStartPosX = nextOffsetX;
        nextOffsetX += 5;

        LOG("NEW PLAYER JOINED UUID: " << newUUID <<"\n");

        Protocol::Player newPlayer =
        {
            newUUID,
            Protocol::Vec3(newStartPosX,0,0),  // inital position
            Protocol::Vec3(0,0,0),  // velocity
            Protocol::Vec3(0,0,0),  // acceleration
            Protocol::Vec4(0,0,0,1) // initial direction (quaternion)
        };

        users.emplace_back(peer, newPlayer);

        //Notify all connected clients of the new player
        uint64_t serverTime = GetServerTime();
        flatbuffers::FlatBufferBuilder connectBuilder;
        auto connectPacket = Protocol::CreateClientConnectS2C(connectBuilder, newUUID, serverTime);
        auto connectPackage = CreatePacketWrapper(connectBuilder, Protocol::PacketType_ClientConnectS2C, connectPacket.Union());
        connectBuilder.Finish(connectPacket);
        auto packet = enet_packet_create(connectBuilder.GetBufferPointer(), connectBuilder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(m_Server, 0, packet);
        
       // message::BroadcastPacket_S2C(m_Server, connectBuilder.GetBufferPointer(), connectBuilder.GetSize());


        //Notify all existing clients about the new player
        flatbuffers::FlatBufferBuilder spawnBuilder = message::CompressMessage_Server
        (Protocol::PacketType_SpawnPlayerS2C, &newPlayer);
         message::BroadcastPacket_S2C(m_Server, spawnBuilder.GetBufferPointer(), spawnBuilder.GetSize());
    }

    void Server::RemovePeerUser(ENetPeer* peer) //ON PLAYER DISCONNECT
    {
        auto it = std::remove_if(users.begin(), users.end(), [peer](const auto& pair)
            {
                return pair.first == peer;
            });

        if(it != users.end())
        {
            LOG("PENDING DESPAWN PLAYER PAKET\n");
            uint32_t removedPlayerUUID = it->second.uuid();
            users.erase(it,users.end());
            LOG("REMOVED PLAYER WITH UUID: "<< removedPlayerUUID << "FROM USERS LIST\n");

            flatbuffers::FlatBufferBuilder builder = message::CompressMessage_Server(PacketType_DespawnPlayerS2C, &removedPlayerUUID);
            message::BroadcastPacket_S2C(m_Server, builder.GetBufferPointer(), builder.GetSize());
            LOG("SERVER BROADCAST: DESPAWNED PLAYER WITH UUID: " << removedPlayerUUID << "\n");
        }
    }

    void Server::BroadcastGameState() //UPDATE GAMEPLAY STATE FOR EVERY CONNECTED USER
    {
        //Update the gamestate and broadcast to the connected users
        uint64_t serverTime = GetServerTime();
        for(auto& user : users)
        {
            ENetPeer* peer = user.first;
            Protocol::Player* player = &user.second;

            // Send update packet for the player
            flatbuffers::FlatBufferBuilder updateBuilder;
            auto updatePacket = CreateUpdatePlayerS2C(updateBuilder, serverTime, player);
            updateBuilder.Finish(updatePacket);
            message::BroadcastPacket_S2C(m_Server,updateBuilder.GetBufferPointer(), updateBuilder.GetSize());
        }
    }

    void Server::Poll() {
        if (!m_Active) {
            return;
        }


        while (enet_host_service(m_Server, &m_Event, 0) > 0) {
            for (int i = 0; i < 32; ++i) {
                if (m_Server->peers[i].address.host != 0) {
                    LOG(i << ": " << IP_STREAM(m_Server->peers[i].address.host) << '\n');
                }
            }

            switch (m_Event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    {
                        LOG("Server: connection formed with \"" <<
                        IP_STREAM(m_Event.peer->address.host) << ':' << m_Event.peer->address.port << "\"\n");
                        AddPeerUser(m_Event.peer);
                        //static uint32_t count = 1;
                        //uint32_t newUUID = count++;
                        //LOG("Server: NOTIFY NEW PLAYER JOINED: " << newUUID << "\n"); //newUUID is for laser 
                        //Protocol::Player newplayer = { newUUID,Vec3{0,0,0},Vec3{0,0,0},Vec3{0,0,0},Vec4{0,0,0,1} };
                        //users.emplace_back(m_Event.peer, newplayer);
                        //SEND A SPAWN PACKET TO ALL THE CONNECTED USER TO SPAWN THIS NEWLY ADDED USER
                        //flatbuffers::FlatBufferBuilder builder;
                        //auto spawnPacket = Protocol::CreateSpawnPlayerS2C(builder, &newplayer);
                        //auto packetWrapper = Protocol::CreatePacketWrapper(builder, Protocol::PacketType_SpawnPlayerS2C, spawnPacket.Union());
                        //builder.Finish(packetWrapper);
                        //LOG("Server: SEND SPAWN REQUEST TO EVERY CONNECTED USER: " << newUUID << "\n"); //newUUID is for laser 
                        //ENetPacket* packet = enet_packet_create(builder.GetBufferPointer(), builder.GetSize(),ENET_PACKET_FLAG_RELIABLE);
                        //enet_host_broadcast(m_Server, 0, packet);
                        break;
                    }
                    


                case ENET_EVENT_TYPE_RECEIVE:
                    LOG("Server: recieved packet of size " <<
                        m_Event.packet->dataLength << " \"" << m_Event.packet->data << "\"\n");
                    {
                     //   const PacketWrapper* packetWrapper = GetPacketWrapper(m_Event.packet->data);
                     //   processPacket(packetWrapper);
                     //   //UPDATE THE GAME STATE (BASED ON THE PACKET CONETEN)
                	    //enet_packet_destroy(m_Event.packet);
                        
                    }
                   /* m_Event.packet->data;
                    enet_packet_destroy(m_Event.packet);*/
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    LOG("Server: disconnected \"" << m_Event.peer->data << "\"\n");
                    {
                        //Remove players who disconnect and broadcast the change to all clients.
                        RemovePeerUser(m_Event.peer);

                      /*  for(auto it = clients.begin(); it != clients.end(); it++)
                        {
                            if(it->second.peer == m_Event.peer)
                            {
                                LOG("SERVER: DISCONNECTED USER: " << m_Event.peer->data << "\n");
                                flatbuffers::FlatBufferBuilder builder;
                                auto despawnPacket = Protocol::CreateDespawnPlayerS2C(builder, it->first);
                                auto wrapper = Protocol::CreatePacketWrapper(builder, Protocol::PacketType_DespawnPlayerS2C, despawnPacket.Union());
                                builder.Finish(wrapper);
                                BroadcastPacket(builder.GetBufferPointer(), builder.GetSize());
                                clients.erase(it);
                                break;
                            }
                        }*/
                    }

                    enet_peer_reset(m_Event.peer);
                    break;

                default: ; //LOG("BALLS\n");
            }
        }
    }
}
