#include "config.h"
#include "network.h"

using namespace Protocol;

//namespace Net {
//    void Initialize() {
//        if (int res = enet_initialize()) {
//            std::cout << res << " Failed to initialize ENet.\n";
//            exit(res);
//        }
//
//        atexit(enet_deinitialize);
//    }
//}

NetworkManager net_instance = NetworkManager::Instance();

NetworkManager::NetworkManager()
{
	if(enet_initialize() != 0){
		std::cout << "Failed to initalize ENET!\n";
	}
	std::cout << "Successful initalize ENET NETWORK\n";

}

NetworkManager::~NetworkManager()
{
	enet_deinitialize();
}

////Initalize the ENET server
//bool NetworkManager::StartServer(uint16_t port)
//{
//	ENetAddress address;
//	address.host = ENET_HOST_ANY;
//	address.port = port;
//	host = enet_host_create(&address, 32, 2, 0, 0);
//	if (!host) {
//		std::cout << "Failed to create ENet Server!\n";
//		return false;
//	}
//
//	return true;
//}
//
////Client establishing connection to server
//bool NetworkManager::ConnectToServer(const char* ip, uint16_t port)
//{
//	//Create a client host
//	host = enet_host_create(nullptr, 1, 2, 0, 0);
//	if(!host)
//	{
//		std::cout << "Failed to create ENet Client!\n";
//		return false;
//	}
//
//	ENetAddress address;
//	enet_address_set_host(&address, ip);
//	address.port = port;
//
//	serverPeer = enet_host_connect(host, &address, 2, 0);
//	if (!serverPeer)
//	{
//		std::cerr << "Failed to connect to server! (NO ESTABLISH CONNECTION BETWEEN CLIENT & SERVER)" << std::endl;
//		return false;
//	}
//
//	return true;
//}
//
////Shutdown the network
//void NetworkManager::Shutdown()
//{
//	if(host)
//	{
//		enet_host_destroy(host);
//		host == nullptr;
//	}
//	clients.clear();
//	serverPeer = nullptr;
//}

//void NetworkManager::ProcessPacket()
//{
//	ENetEvent event;
//	/*
//	*  enet_host_service()
//	* Parameters
//		host	host to service
//		event	an event structure where event details will be placed if one occurs if event == NULL then no events will be delivered
//		timeout	number of milliseconds that ENet should wait for events
//		Return values
//			>	0 if an event occurred within the specified time limit
//			0	if no event occurred
//<			0 on failure
//	*/
//	while(enet_host_service(host,&event,0) > 0) 
//	{
//		switch(event.type)
//		{
//			case ENET_EVENT_TYPE_CONNECT:
//				if(serverPeer == nullptr) // Server handling 
//				{
//					uint32_t clientID = event.peer->incomingPeerID;
//					clients[clientID] = event.peer;
//					if(OnClientConnect)
//					{
//						OnClientConnect(clientID);
//					}
//				}
//				break;
//
//			case ENET_EVENT_TYPE_RECEIVE:
//				HandlePacket(event.packet, event.peer->incomingPeerID);
//				enet_packet_destroy(event.packet);
//				break;
//
//			case ENET_EVENT_TYPE_DISCONNECT:
//				if(serverPeer == nullptr)
//				{
//					uint32_t clientID = event.peer->incomingPeerID;
//					clients.erase(clientID);
//					if(OnClientDisconnect)
//					{
//						OnClientDisconnect(clientID);
//					}
//				}
//				break;
//			
//			default:
//				break;
//		}
//	}
//}

//Process recieved packet
void NetworkManager::HandlePacket(ENetPacket* packet, uint32_t senderID)
{
	auto wrapper = GetPacketWrapper(packet->data);
	if (OnPacketRecieved)
		OnPacketRecieved(senderID, wrapper);
}

void NetworkManager::SendToServer(ENetPeer* peer,const FlatBufferBuilder& builder)
{
	if (peer == nullptr) return; //No connection to server
	ENetPacket* packet = enet_packet_create(builder.GetBufferPointer(), builder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer,0,packet);
}

void NetworkManager::SendToClient(ENetPeer* peer, const FlatBufferBuilder& builder)
{
	if (peer == nullptr) return;
	ENetPacket* packet = enet_packet_create(builder.GetBufferPointer(), builder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void NetworkManager::Broadcast(ENetHost* serverHost, const FlatBufferBuilder& builder)
{
	ENetPacket* packet = enet_packet_create(builder.GetBufferPointer(), builder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(serverHost, 0, packet);
}


namespace packet
{
	//Server to client
	FlatBufferBuilder ClienConnectsS2C(const uint32_t senderID, unsigned long long serverTime)
	{
		FlatBufferBuilder fbb;
		const auto clientConnect = CreateClientConnectS2C(fbb, senderID, serverTime);
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_ClientConnectS2C, clientConnect.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	FlatBufferBuilder GameStateS2C(const std::vector<Player>& players, const std::vector<Laser>& lasers)
	{
		FlatBufferBuilder fbb;
		const auto gameState = CreateGameStateS2CDirect(fbb, &players, &lasers);
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_GameStateS2C, gameState.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	FlatBufferBuilder SpawnPlayerS2C(const Player* player)
	{
		FlatBufferBuilder fbb;
		const auto spawnP = CreateSpawnPlayerS2C(fbb, player);
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_SpawnPlayerS2C, spawnP.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	FlatBufferBuilder DespawnPlayerS2C(const uint32_t playerID)
	{
		FlatBufferBuilder fbb;
		const auto despawnP = CreateDespawnPlayerS2C(fbb, playerID);
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_DespawnPlayerS2C, despawnP.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	FlatBufferBuilder UpdatePlayerS2C(const uint64_t timeMs, const Player* player)
	{
		FlatBufferBuilder fbb;
		const auto updateP = CreateUpdatePlayerS2C(fbb, timeMs, player);
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_UpdatePlayerS2C, updateP.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	FlatBufferBuilder TeleportPlayerS2C(const uint64_t timeMs, const Player* player)
	{
		FlatBufferBuilder fbb;
		const auto teleP = CreateTeleportPlayerS2C(fbb, timeMs, player);
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_TeleportPlayerS2C, teleP.Union());
		fbb.Finish(wrapper);
		return fbb; 
	}

	FlatBufferBuilder SpawnLaserS2C(const Laser* laser)
	{
		FlatBufferBuilder fbb;
		const auto spawnL = CreateSpawnLaserS2C(fbb, laser);
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_SpawnLaserS2C, spawnL.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	FlatBufferBuilder DespawnLaserS2C(const uint32_t laserID)
	{
		FlatBufferBuilder fbb;
		const auto despawnL = CreateDespawnLaserS2C(fbb, laserID );
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_DespawnLaserS2C, despawnL.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	FlatBufferBuilder CollisionS2C(const uint32_t entity1ID, const uint32_t entity2ID)
	{
		FlatBufferBuilder fbb;
		const auto collision = CreateCollisionS2C(fbb,entity1ID, entity2ID);
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_CollisionS2C, collision.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	FlatBufferBuilder TextS2C(const std::string& text)
	{
		FlatBufferBuilder fbb;
		const auto textS2C = CreateTextS2CDirect(fbb, text.c_str());
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_TextS2C, textS2C.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	//Client to Server
	FlatBufferBuilder InputC2S(uint64 timeMs, uint16 bitmap)
	{
		FlatBufferBuilder fbb;
		const auto input = CreateInputC2S(fbb,timeMs,bitmap);
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_InputC2S, input.Union());
		fbb.Finish(wrapper);
		return fbb;
	}

	FlatBufferBuilder TextC2S(const std::string& text)
	{
		FlatBufferBuilder fbb;
		const auto textC2S = CreateTextC2SDirect(fbb, text.c_str());
		const auto wrapper = CreatePacketWrapper(fbb, PacketType_TextC2S, textC2S.Union());
		fbb.Finish(wrapper);
		return fbb;
	}
}