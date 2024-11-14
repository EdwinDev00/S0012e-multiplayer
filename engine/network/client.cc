#include "config.h"
#include "client.h"

#include "network.h"

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
    Client::Connect(const char *ip, const uint16 port) {
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

    void Client::SendPacket(void *data, size_t size) {
        ENetPacket *packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
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
                {
                      //Send a connection package to the server flatbuffer
                      flatbuffers::FlatBufferBuilder builder; 
                      auto clientConnectpacket = Protocol::CreateClientConnectS2C(builder, /* uuid */ 1, /* time */ static_cast<uint64_t>(time(nullptr) * 1000));
                      auto packetWrapper = Protocol::CreatePacketWrapper(builder, Protocol::PacketType_ClientConnectS2C, clientConnectpacket.Union());
                      builder.Finish(packetWrapper);
                      SendPacket(builder.GetBufferPointer(), builder.GetSize());    
                      LOG("CLIENT: Send Connection Packet Request \n");
                }
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                LOG("Client: recieved packet of size " <<
                    m_Event.packet->dataLength << " \"" << m_Event.packet->data << "\"\n");
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                LOG("Client: disconnected \"" << m_Event.peer->data << "\"\n");
                break;
            default: ;
        }
    }
}
