#include "config.h"
#include "client.h"

#include "network.h"
#include "../projects/spacegame/code/spaceship.h"

namespace Net {
    Client::~Client() {
        LOG("Destroyed client.\n");
        Disconnect();
        enet_host_destroy(m_Client);
    }

    bool Client::Create() {
        if (m_Client != nullptr)
            return true;

        m_Client = enet_host_create(nullptr, 1, 1, 0, 0);
        if (!m_Client) {
            LOG("Failed to create ENet client host.\n");
            return false;
        }

        LOG("Successfully created ENet client host.\n");
        m_Active = true;
        return true;
    }

    bool
        Client::Connect(const char* ip, const uint16 port) {
        enet_address_set_host(&m_Address, ip);
        m_Address.port = port;
        LOG("Connecting to " << IP_STREAM(m_Address.host) << ':' << port << '\n');

        m_Peer = enet_host_connect(m_Client, &m_Address, 1, 0);
        if (!m_Peer) {
            LOG("No available peers for initializing connection.\n");
            return false;
        }
        return true;
        //if (enet_host_service(m_Client, &m_Event, 10000) > 0 && m_Event.type == ENET_EVENT_TYPE_CONNECT) {
        //    LOG("Connection to " << ip << ':' << port << " succeeded.\n");
        //    return true;
        //}
        //enet_peer_reset(m_Peer);
        //LOG("Connection to " << ip << ':' << port << " failed.\n");
        //return false;
    }

    void Client::Disconnect() {
        if (m_Peer != nullptr) {
            enet_peer_disconnect(m_Peer, 0);
        }
    }

    //REMOVE THIS USE MESSENGER
    void Client::SendPacket(void* data, size_t size) {
        ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(m_Peer, 0, packet);
        //enet_host_flush(m_Client);
    }

    void Client::Poll()
    {
        if (!m_Active) {
            return;
        }

        enet_host_service(m_Client, &m_Event, 0);

        switch (m_Event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            LOG("Client: connection formed with \"" <<
                IP_STREAM(m_Event.peer->address.host) << ':' << m_Event.peer->address.port << "\"\n");
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            LOG("Client: recieved packet of size " <<
                m_Event.packet->dataLength << " \"" << m_Event.packet->data << "\"\n");
  
            const PacketWrapper* packet = GetPacketWrapper(m_Event.packet->data);
            LOG("Client: packet type " <<
                packet->packet_type() << "\n");
            switch (packet->packet_type())
            {
            case Protocol::PacketType_ClientConnectS2C: {
                auto connectPacket = packet->packet_as_ClientConnectS2C();
                clientControlledID = connectPacket->uuid();
                LOG("Client: ASSIGNED CONTROLLED ID: " << clientControlledID << "\n");
                break;
            }
            case Protocol::PacketType_SpawnPlayerS2C: {
                //DESERIALIZE THE PACKET AND SET THE VALUES ACCORDINGLY
                auto spawnPlayer = packet->packet_as_SpawnPlayerS2C();
                LOG("Client: RECIEVED SPAWN PACKET OF: " << spawnPlayer->player()->uuid() << "\n");
                auto player = spawnPlayer->player();
                //USE THE DATA FOR RENDERING the player out
                glm::vec3 pos = { player->position().x(),player->position().y(),player->position().z() };
                glm::quat orientation = glm::identity<glm::quat>();
                glm::mat4 T = translate(pos) * (glm::mat4)orientation;

                //CREATE CLIENT CONTROLLED AVATAR 
                Game::SpaceShip* newplayer = new Game::SpaceShip();
                newplayer->id = player->uuid();
                newplayer->transform = T;

                if (player->uuid() == clientControlledID) {
                    LOG("Client: Spawning controlled spaceship UUID: " << player->uuid() << "\n");
                    // Add input control or special rendering logic here
                }
                else {
                    LOG("Client: Spawning other player spaceship UUID: " << player->uuid() << "\n");
                }

                clientControlledID = player->uuid();
                ships->insert({ player->uuid(),newplayer });
                break;
            }
            case Protocol::PacketType_DespawnPlayerS2C: {
                // Handle player despawn logic
                auto despawnPlayer = packet->packet_as_DespawnPlayerS2C();
                uint32_t uuid = despawnPlayer->uuid();
                if (ships->erase(uuid) > 0) {
                    LOG("Client: Removed spaceship with UUID: " << uuid << "\n");
                }
                else {
                    LOG("Client: Failed to find spaceship with UUID: " << uuid << " for despawn\n");
                }
                break;
            }

            case Protocol::PacketType_GameStateS2C:
            {
                // Process game state update (positions, velocities, orientation, etc.)
                auto gameState = packet->packet_as_GameStateS2C();
                for (const auto& player : *gameState->players()) {
                    uint32_t uuid = player->uuid();
                    auto it = ships->find(uuid);
                    if (it != ships->end()) {
                        // Update player spaceship transform
                        glm::vec3 pos = { player->position().x(), player->position().y(), player->position().z() };
                        glm::quat orientation = glm::identity<glm::quat>();
                        glm::mat4 T = glm::translate(pos) * glm::mat4(orientation);
                        it->second->transform = T;
                        break;
                    }

                }
            }

            break;
            case ENET_EVENT_TYPE_DISCONNECT:
                LOG("Client: disconnected \"" << m_Event.peer->data << "\"\n");
                break;
            default:;
            }
        }
    }
}