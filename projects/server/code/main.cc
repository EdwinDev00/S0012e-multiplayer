//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "spacegameapp.h"

//TODO: TEST CLIENT CONNECT TO THE SERVER BASED ON PORT AND ADDRESS

#include "proto.h"
#include "enet/enet.h"
#include <iostream>
#include "glm.hpp"
#include <chrono>

//SERVER SIDE
using namespace Protocol;
std::vector<Player> playersV;
std::vector<Laser> lasersV;


void processClientPacket(const PacketWrapper* packetWrapper)
{
	//Process each type of incoming message
	switch(packetWrapper->packet_type())
	{
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

void sendGameState(ENetHost* server, ENetPeer* clientPeer)
{
	flatbuffers::FlatBufferBuilder builder;

	//Demonstration empty game state
	std::vector<flatbuffers::Offset<Player*>> players; // Add players if necessary
	std::vector<flatbuffers::Offset<Laser*>> lasers;   // Add lasers if necessary

	auto playersVector = builder.CreateVector(players);
	auto laserVector = builder.CreateVector(lasers);
	auto gamestate = CreateGameStateS2C(builder,playersVector.o,laserVector.o);
	auto packetWrapper = CreatePacketWrapper(builder, PacketType_GameStateS2C, gamestate.Union());
	builder.Finish(packetWrapper);

	//Send the packet to the client
	ENetPacket* packet = enet_packet_create(builder.GetBufferPointer(), builder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(clientPeer, 0, packet);
	enet_host_flush(server);

	std::cout << "Sent GameStateS2C to client." << std::endl;
}

int
main(int argc, const char** argv)
{

//------------------------------------------------------------------------------------
	//Server ()
/*
The server has a somewhat similar loop:

	1.Sample clock to find start time
	2.Read client user input messages from network
	3.Execute client user input messages
	4.Simulate server-controlled objects using simulation time from last full pass
	5.For each connected client, package up visible objects/world state and send to client
	6.Sample clock to find end time
	7.End time minus start time is the simulation time for the next frame
	In this model, non-player objects run purely on the server, while player objects drive their movements based on incoming packets. Of course, this is not the only possible way to accomplish this task, but it does make sense.
*/

/*
	UDP Server :

	1.Create a UDP socket.
	2.Bind the socket to the server address.
	3.Wait until the datagram packet arrives from the client.
	4.Process the datagram packet and send a reply to the client.
	5.Go back to Step 3.
*/
//----------------------------------------------------------------------------------


	if(enet_initialize() != 0)
	{
		std::cerr << "SERVER: FAILED TO INITALIZING ENET\n";
		return EXIT_FAILURE;
	}
	
	//Create sever UDP Connection
	ENetAddress address;
	address.host = ENET_HOST_ANY; //bind to any host
	address.port = 1234; // server port

	ENetHost* server = enet_host_create(&address, 32, 2, 0, 0); //max 32 client 2 channels
	if(server == nullptr)
	{
		std::cerr << "SERVER: AN ERROR OCCURED WHILE TYRING TO CREATE SERVER HOST\n";
		enet_deinitialize();
		return EXIT_FAILURE;
	}

	std::cout << "SERVER: STARTED AND LISTENING TO PORT " << address.port << "\n";

	//MAIN SERVER LOOP
	while(true)
	{
		ENetEvent event;
		while(enet_host_service(server,&event, 1000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "A client connected from "
						<< (event.peer->address.host & 0xff) << "."
						<< ((event.peer->address.host >> 8) & 0xff) << "."
						<< ((event.peer->address.host >> 16) & 0xff) << "."
						<< ((event.peer->address.host >> 24) & 0xff)
						<< ":" << event.peer->address.port << std::endl;
					break;

				case ENET_EVENT_TYPE_RECEIVE: 
				{
					// Deserialize the incoming packet with FlatBuffers
					const PacketWrapper* packetWrapper = GetPacketWrapper(event.packet->data);
					processClientPacket(packetWrapper);

					// Optionally, send a game state update back to the client
					sendGameState(server, event.peer);

					enet_packet_destroy(event.packet);
					break;
				}

				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "Client disconnected." << std::endl;
					event.peer->data = nullptr;
					break;

				default:
					break;
			}
		}		
	}

	enet_host_destroy(server);
	enet_deinitialize();
	return EXIT_SUCCESS;


	/*Game::SpaceGameApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();*/
	
}