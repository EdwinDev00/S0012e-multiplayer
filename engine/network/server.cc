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
                    LOG("Server: connection formed with \"" <<
                        IP_STREAM(m_Event.peer->address.host) << ':' << m_Event.peer->address.port << "\"\n");
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    LOG("Server: recieved packet of size " <<
                        m_Event.packet->dataLength << " \"" << m_Event.packet->data << "\"\n");

                    m_Event.packet->data;
                    enet_packet_destroy(m_Event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    LOG("Server: disconnected \"" << m_Event.peer->data << "\"\n");
                    enet_peer_reset(m_Event.peer);
                    break;
                default: ; //LOG("BALLS\n");
            }
        }
    }
}
