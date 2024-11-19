#pragma once
#include "server.h"
#include "client.h"
#include <iostream>
#include "proto.h"

#define LOG(msg)std::cout << msg
using namespace Protocol;

struct PlayerState
{
	uint32_t uuid;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::quat direction;
};

//std::unordered_map<uint32_t, Player> players; // Store players by UUID //Should be in the server side not in here

namespace Net {
#define IP_STREAM(IP) (IP & 0xFF) << '.' << ((IP >> 8) & 0xFF) << '.' << ((IP >> 16) & 0xFF) << '.' << ((IP >> 24) & 0xFF)

    void Initialize();

	inline void processPacket(const PacketWrapper* packetWrapper)
	{
		//Process each type of incoming message
		switch (packetWrapper->packet_type())
		{
		case PacketType_ClientConnectS2C:
		{
			auto connectPacket = packetWrapper->packet_as_ClientConnectS2C();
			uint32 uuid = connectPacket->uuid();
			uint32 time = connectPacket->time();
			std::cout << "Client connected with UUID: " << uuid << ", at time: " << time << std::endl;

			break;
		}

		case PacketType_InputC2S:
		{
			auto inputPacket = packetWrapper->packet_as_InputC2S();
			uint32 time = inputPacket->time();
			uint32 bitmap = inputPacket->bitmap();
			std::cout << "Received InputC2S: time = " << time << ", bitmap = " << bitmap << std::endl;
			// Use the time and bitmap to drive player actions on the server

			break;
		}

		case PacketType_TextC2S:
		{
			auto textPacket = packetWrapper->packet_as_TextC2S();
			std::cout << "Received TextC2S: text = " << textPacket->text()->str() << std::endl;
			// Process text message (e.g., chat)
			break;
		}

		default:
			std::cerr << "Unknown packet type received" << std::endl;
			break;
		}
	}
}
