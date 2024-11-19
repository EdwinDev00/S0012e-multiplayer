#include "config.h"
#include "server.h"
#include "network.h"

#include <bitset>

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


    void Server::BroadcastPacket(void* data, size_t size)
    {
        /*for(auto& [uuid,client] : clients)
        {
            if (client.isActive)
            {
                ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(client.peer,0,packet);
            }
        }*/
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
                       /* LOG("Server: connection formed with \"" <<
                        IP_STREAM(m_Event.peer->address.host) << ':' << m_Event.peer->address.port << "\"\n");*/
                        static uint32_t count = 1;
                        uint32_t newUUID = count++;
                        LOG("Server: NOTIFY NEW PLAYER JOINED: " << newUUID << "\n"); //newUUID is for laser 
                        
                        Protocol::Player newplayer = { newUUID,Vec3{0,0,0},Vec3{0,0,0},Vec3{0,0,0},Vec4{0,0,0,1} };
                        users.emplace_back(m_Event.peer, newplayer);

                        //SEND A SPAWN PACKET TO ALL THE CONNECTED USER TO SPAWN THIS NEWLY ADDED USER
                        flatbuffers::FlatBufferBuilder builder;
                        auto spawnPacket = Protocol::CreateSpawnPlayerS2C(builder, &newplayer);
                        auto packetWrapper = Protocol::CreatePacketWrapper(builder, Protocol::PacketType_SpawnPlayerS2C, spawnPacket.Union());
                        builder.Finish(packetWrapper);

                        LOG("Server: SEND SPAWN REQUEST TO EVERY CONNECTED USER: " << newUUID << "\n"); //newUUID is for laser 
                        ENetPacket* packet = enet_packet_create(builder.GetBufferPointer(), builder.GetSize(),ENET_PACKET_FLAG_RELIABLE);
                        enet_host_broadcast(m_Server, 0, packet);
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
                        //SERVER DISCONNECT, SEND PACKET TO EVERY CONNECTED USER TO DISCONNECT
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
