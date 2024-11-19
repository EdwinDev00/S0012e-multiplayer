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

    void Client::SendPacket(void* data, size_t size) {
        ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(m_Peer, 0, packet);
        //enet_host_flush(m_Client);
    }

    void Client::Poll() {
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
            {
                const PacketWrapper* packet = GetPacketWrapper(m_Event.packet->data);
                switch (packet->packet_type()) {
                    case Protocol::PacketType_SpawnPlayerS2C: {
                        //DESERIALIZE THE PACKET AND SET THE VALUES ACCORDINGLY
                        auto spawnPlayer = packet->packet_as_SpawnPlayerS2C();
                        LOG("Client: RECIEVED SPAWN PACKET OF: " << spawnPlayer->player()->uuid());
                        auto player = spawnPlayer->player();
                        LOG("Client: DESERIALIZE SPAWN PACKET: " << spawnPlayer->UnPack()->player->uuid());
                        //USE THE DATA FOR RENDERING the player out
                        glm::vec3 pos = { player->position().x(),player->position().y(),player->position().z() };
                        glm::quat orientation = glm::identity<glm::quat>();
                        glm::mat4 T = translate(pos) * (glm::mat4) orientation;
                        //shipTransforms->insert({player->uuid(), T});
                        // 
                       //TESTING storing whole spaceship object
                        Game::SpaceShip* newplayer = new Game::SpaceShip();
                        newplayer->id = player->uuid();
                        newplayer->transform = T;
                        ships->insert({ player->uuid(),newplayer });


                        break;
                    }
                    case Protocol::PacketType_DespawnPlayerS2C: {
                        auto despawnPlayer = packet->packet_as_DespawnPlayerS2C();
                        uint32_t uuid = despawnPlayer->uuid();
                        //players.erase(uuid);
                        LOG("Client: Player despawned: " << uuid);
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