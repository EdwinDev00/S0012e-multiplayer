#pragma once
#include "enet/enet.h"
#include "proto.h"

#include <iostream> //Debug 

using namespace Protocol;

namespace message
{
	//Handles the network communication messages between client and server
	//- SEND MESSAGE (PAKET) FROM C2S wiseversa
	//- UNPACK MESSAGE (UNPACK THE MESSAGE DATA FOR MODIFYING)
	// - COMPRESS TO FLATBUFFER FORMAT ACTION/EVENTS

	inline void SendPacket_C2S(ENetPeer* clientPeer, void* data, size_t size)
	{
		// CLIENT SENDING A PACKET TO SERVER
		auto packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(clientPeer, 0, packet);
	}

	inline void BroadcastPacket_S2C(ENetHost* server, void* data, size_t size)
	{
		//SERVER SENDING PACKET TO EVERY CONNECTED CLIENT
		auto packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(server, 0, packet); //Send the message to every connected client
	}

	inline const Protocol::PacketWrapper* UnpackPacket(ENetPacket* packetData)
	{
		//BOTH SERVER AND CLIENT (UNPACK THE DATA)
		return GetPacketWrapper(packetData->data);	 
	}

	inline flatbuffers::FlatBufferBuilder CompressMessage_Server(const PacketType type, const void* data)
	{
		flatbuffers::FlatBufferBuilder builder;
		
		//ERROR HANDLING
		if (!data) {
			std::cout << "SERVER: FAILED ATTEMPT TO COMPRESS DATA OF " << type << "\n";
			return builder; // Return an empty builder
		}

		//NOTE CLIENTCOnnectS2C notifyies the client which spaceship he controlls (during gamestate updates it only affects the associated id)
		switch (type)
		{
		case PacketType_SpawnPlayerS2C:
		{
			const auto* playerData = static_cast<const Protocol::Player*>(data);
			auto paketData = CreateSpawnPlayerS2C(builder, playerData);
			auto package = CreatePacketWrapper(builder, type, paketData.Union());
			builder.Finish(package);
			break;
		}

		case PacketType_DespawnPlayerS2C:
		{
			const auto* uuidData = static_cast<const uint32_t*>(data);
			auto paketData = CreateDespawnPlayerS2C(builder,*uuidData);
			auto package = CreatePacketWrapper(builder, type, paketData.Union());
			builder.Finish(package);
			break;
		}

		// OTHER CASES
		//case PacketType_ClientConnectS2C: break; //NOTIFY THE CONNECTED PLAYER ABOUT THEIR CONTROLLED SPACESHIP ID
		//case PacketType_GameStateS2C: break; //PACKAGE FOR UPDATED GAME STATE FORM SERVER


			default:
			break;
		}
		return builder; //RETURN THE PACKAGE BUILDER
	}
}