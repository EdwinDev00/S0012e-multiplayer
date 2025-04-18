#pragma once
//#include "server.h"
//#include "client.h"
//#include "message.h" //NETWORK MESSAGING PAKET 

#include <iostream>
#include "proto.h"
#include "enet/enet.h"
#include "flatbuffers/flatbuffers.h"

using namespace flatbuffers;

using namespace Protocol;


//NEW NETWORK MANAGER
/*
* NETWORK MAIN PURPOSE
*	- PACKAGING THE FLATBUFFER FUNCTIONALITY ALL THE DIFFERENT KIND
*	- ESTABLISH THE ENET INITALIZE 
*/

class NetworkManager
{
public:

	/*
	* NETWORK MAIN PURPOSE
	*	- PACKAGING THE FLATBUFFER FUNCTIONALITY ALL THE DIFFERENT KIND
	*	- ESTABLISH THE ENET INITALIZE
	*/
	//Should be a singleton instance (no need for multiple instance of these )
	//THIS SHOULD ONLY ACTS AS A PLATFORM OF CREATING THE PACKET OF (Protocol::ClientConnectS2C(..), Protocol::GameStateS2C(..) etc)
	//Do not handles any logic behind usage of the packet (that should be handled in respective GAMESERVER & CLIENT)
	//helper function for deserialize the packet so we get the data

	//network helper function (packaging send  )

	static NetworkManager& Instance()
	{
		static NetworkManager instance;
		return instance;
	}

	NetworkManager(); //initalize ENET
	~NetworkManager(); //Destroy ENET

	//REMOVE THESE (HANDLES THE SERVER INITALIZATION INSIDE GAMESERVER NOT HERE)
	//bool StartServer(uint16_t port = 1234);
	//bool ConnectToServer(const char* ip = "localhost", uint16_t port = 1234);
	//void Shutdown();
	//void ProcessPacket();
	//-------------------

	void SendToServer(ENetPeer* peer, const FlatBufferBuilder& builder);
	void SendToClient(ENetPeer*, const FlatBufferBuilder& builder); //In server find the client with the ID we want to send to
	void Broadcast(ENetHost* serverHost, const FlatBufferBuilder& builder); //Only server broadcast to all connected players
	
	void HandlePacket(ENetPacket* packet, uint32_t senderID); //deserialize the packet data into return variable for usage

	std::function<void(ENetPeer*)> OnClientConnect; //On client requesting connecting to server
	std::function<void(uint32_t)> OnClientDisconnect; // On client requesting disconnecting from server
	std::function<void(uint32_t, const PacketWrapper*)> OnPacketRecieved;

private:
	//ENetHost* host = nullptr;
	//std::unordered_map<uint32_t, ENetPeer*> clients; //track cconnected users
	//ENetPeer* serverPeer = nullptr;

};

extern NetworkManager net_instance;

namespace packet {
	//Server To Client packet
	FlatBufferBuilder ClienConnectsS2C(const uint32_t senderID, unsigned long long timeMs); //server time ms
	FlatBufferBuilder GameStateS2C(const std::vector<Player>& players, const std::vector<Laser>& lasers); //const vector of laser should be implemented here also
	FlatBufferBuilder SpawnPlayerS2C(const Player* player);
	FlatBufferBuilder DespawnPlayerS2C(const uint32_t playerID);
	FlatBufferBuilder UpdatePlayerS2C(const uint64_t timeMs, const Player* player); //server time when it was sent back to client
	FlatBufferBuilder TeleportPlayerS2C(const uint64_t timeMs, const Player* player); //server time when it was sent back
	FlatBufferBuilder SpawnLaserS2C(const Laser* laser);
	FlatBufferBuilder DespawnLaserS2C(const uint32_t laserID);
	FlatBufferBuilder CollisionS2C(uint32_t entity1ID, uint32_t entity2ID);
	FlatBufferBuilder TextS2C(const std::string& text);

	// Client to server.
	FlatBufferBuilder InputC2S(uint64 timeMs, uint16 bitmap);

	FlatBufferBuilder TextC2S(const std::string& text);
}
