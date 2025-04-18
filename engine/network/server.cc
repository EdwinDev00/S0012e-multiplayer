#include "config.h"
#include "server.h"
#include "../projects/spacegame/code/spaceship.h"
#include <iostream>
#include <chrono>

#pragma region UTILITY

Player GameServer::BatchShip(const Game::ServerSpaceship& ship) const
{
	const glm::vec3& pos = ship.position;
	const glm::vec3& vel = ship.linearVelocity;
	const glm::quat& orient = ship.orientation;

	Vec3 posVec = Vec3(pos.x, pos.y, pos.z);
	Vec3 velVec = Vec3(vel.x, vel.y, vel.z);
	Vec3 accelVec = Vec3(); // acceleration of the ship
	Vec4 OrientVec = Vec4(orient.w,orient.x, orient.y, orient.z);

	return Player(ship.id,posVec,velVec,accelVec,OrientVec);
}

Laser GameServer::BatchLaser(const Laser& laser) const
{
	//LASER PROPERTIES
	return Laser();
}
#pragma endregion


/*
GAMESERVER RESPONSIBILITY:
	- Manages game state (player, entities)
	- Processes client input and sends authoritative updates
	- Network manager (network) for communication
*/

//TODO: INPUTC2S SENDING AND SERVER RESPOND & UPDATESTATES2C 
// WORKING: SYNC FIX MINOR STUTTERING


//Singelton Gameserver instance
GameServer gameServer = GameServer::instance();
//Convert changes maybe using virtual function for onClientConnect,disconnect,onPackageRecieve is better

GameServer::~GameServer() 
{
	//shutdown server
	if(server != NULL)
	{
		enet_host_destroy(server);
		server = nullptr;
	}

	//Clear all the connect users (peers)
	live = false;
}

void GameServer::StartServer(uint16_t port)
{
	InitNetwork(port);

	live = true; //Set the server into active
	//Setup the collider mesh of spaceship 
	playerMeshColliderID = Physics::LoadColliderMesh("assets/space/spaceship_physics.glb");

	//generate the spawnpoints for the connected user (circular)
	glm::vec3 center(0);
	const float radius = 50;
	for(int i = 0; i < 32; ++i)
	{
		const float angle = glm::two_pi<float>() * (float(i) / 32);
		const float x = center.x + radius * std::cos(angle);
		const float z = center.z + radius * std::sin(angle);
		spawnpoints[i].position = glm::vec3(x, 0.0f, z);
	}

	std::cout << "SERVER: Successful creating ENET server\n";

}

void GameServer::Run()
{
	//Main server run loop

	if(live && server != NULL) //As long the server exist run the server
	{
		PollNetworkEvents();

		// Check for collision

		//Execute all the update function on our spaceship, laser, connect player list
		//	- Remove the marked lasers

		//THIS UNDER COULD BE MOVED TO UPDATE. UPDATE: HANDLES ALL THE EVENT AND UPDATE OF THE PHYSICS OF THE SPACESHIP,LASER STATE
		//player physics update (fixed update at 60 fps)
		for(auto& [uuid, ship] : players)
		{
			auto& peerID = uuid;
			auto& shipState = ship;

			ship.Update(0.01667f); //lets set it to fixed 60 frames
			//Ships collider (playerColliders) transform and matrix also needs to get updated
			Physics::SetTransform(playerColliders[peerID], ship.transform);
		}

		serverTickCounter++;

		//NETWORK STATE SYNC (Every Nth frame) 
		if(serverTickCounter % 5 == 0) //every 5 frame of 60 fps (12 times / s)
		{
			//Start the server time and update the time
			auto now = std::chrono::system_clock::now();
			auto duration = now.time_since_epoch();
			uint64_t epoch_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			s_currentTime = epoch_ms;

			//UPDATE PLAYERS
			for(const auto& player : players)
			{
				auto packPlayer = BatchShip(player.second);
				const auto fbb = packet::UpdatePlayerS2C(s_currentTime, &packPlayer);
				net_instance.Broadcast(server, fbb);
			}
		}
		//std::cout << "[SERVER SYNC] TICK: " << serverTickCounter << " | TIME: " << s_currentTime << "\n";
	}
}

void GameServer::Update(float dt)
{
	//Update all the event happening in the server
	//Spaceship physics
	//Game state (spawn player, despawn, laser update, respawn etc)

	for(auto& [id,spaceship] : players)
	{
		std::cout << "ORIGINAL DATA: Position " << spaceship.position.x << ", "
			<< spaceship.position.y << ", " << spaceship.position.z << "\n Linear Velocity " <<
			spaceship.linearVelocity.x << spaceship.linearVelocity.y << ", " << spaceship.linearVelocity.z << "\n";
		spaceship.Update(dt);
		std::cout << "UPDATED DATA: Position " << spaceship.position.x << ", "
			<< spaceship.position.y << ", " << spaceship.position.z << "\n Linear Velocity " <<
			spaceship.linearVelocity.x << spaceship.linearVelocity.y << ", " << spaceship.linearVelocity.z << "\n";
	}
	//Send the update to all user
	//SendWorldState();
} 

#pragma region ENET / NETWORK

void GameServer::InitNetwork(uint16_t port)
{
	//Creation of the ENet Server 
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	server = enet_host_create(&address, 32, 2, 0, 0);
	if (server == NULL)
	{
		std::cout << "SERVER: Failed to create ENET server\n";
	}
	//Successful creating the ENet server
}

void GameServer::PollNetworkEvents()
{
	ENetEvent event;
	while (enet_host_service(server, &event, 0) > 0) //Pool
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT: {
				std::cout << "SERVER:TYPE CONNECT WAS GENERATED\n";
				//Call the respective callback
				//if (OnClientConnect)
				OnClientConnect(event.peer);
				break;
			}

			case ENET_EVENT_TYPE_RECEIVE: {
				std::cout << "SERVER: RECIEVED INCOMING PACKET FROM CLIENT WITH ID: " << event.peer->incomingPeerID << "\n";
				OnPacketRecieved(event.peer->incomingPeerID, event.packet);
				break;
			}

			case ENET_EVENT_TYPE_DISCONNECT: {
				break;
			}
		}
	}
}

void GameServer::OnClientConnect(ENetPeer* peer)
{
	/*
	*  incomingPeerID
		Purpose: Represents the ID assigned to the remote peer (i.e., the ID that the local host assigned to this connection).
		use: Identifying a specific connected client from the server's side.

		has drawback for soley using incomingPeerID, use self assinged ID When client successful render out the peer
	*/
	
	//uint32_t uuid = nextClientID++; //assign the user with this GameServer unique identifier
	connections[peer] = peer->incomingPeerID; //insert the new element into the list

	auto fbb = packet::ClienConnectsS2C(peer->incomingPeerID, s_currentTime);
	net_instance.SendToClient(peer, fbb); //Send the packet to the connected peer

	//Change in game state (apply the change of new player joined) 

	//GAME STATE CONFIG (APPLY THE NECESSITY FOR THE ADDING THE NEW PLAYER TO THE STATE)
	std::vector<Player> playerVec;
	playerVec.reserve(players.size());

	//Reserve the all existing players so the new player get the data of the already connected peers
	//Below should function the same as BatchShip
	std::transform(players.begin(), players.end(), std::back_inserter(playerVec),
		[](const std::pair<const uint32_t, Game::ServerSpaceship>& entry)
		{
			//Pack our custom server side spaceship into a protocol player
			auto& ship = entry.second;
			const glm::vec3& pos = ship.position;
			const glm::vec3& vel = ship.linearVelocity;
			const glm::quat& orient = ship.orientation;

			Vec3 posVec = Vec3(pos.x, pos.y, pos.z);
			Vec3 velVec = Vec3(vel.x, vel.y, vel.z);
			Vec3 accelVec = Vec3(); // acceleration of the ship
			Vec4 OrientVec = Vec4(orient.w,orient.x, orient.y, orient.z);

			return Player(ship.id, posVec, velVec, accelVec, OrientVec);
		}
	);
	
	std::cout << "PlayerVec count " << playerVec.size() << "\n";
	std::vector<Laser> laserVec;
	laserVec.reserve(lasers.size()); //We let it be zero (laser functionality and properties yet implemented)
	//Do the same for lasers (pack the existing in lasers in the game)

	fbb.Clear();
	fbb = packet::GameStateS2C(playerVec, laserVec);
	net_instance.SendToClient(peer, fbb); //Send back to connected user about the current game state 

	////Claim a spawn point 
	//SpawnPoint* userSpawnPoint = nullptr;
	//for(auto& sp : spawnpoints)
	//{
	//	if(sp.GetAvailableSpawnpoint())
	//	{
	//		sp.occupied = true;
	//		sp.ownerID = peer->incomingPeerID; // player ID
	//		userSpawnPoint = &sp;
	//		break;
	//	}
	//}

	SpawnPlayer(peer->incomingPeerID);

	std::cout << "SERVER: Client " << peer->incomingPeerID << " connected.\n";
	std::cout << "SERVER SPACESHIP COUNT " << players.size() << "\n";
	std::cout << "SERVER: Connected USER COUNT " << connections.size() << "\n";
}

void GameServer::SpawnPlayer(uint32_t clientID)
{
	//Check if the player already owns a spawn point
	SpawnPoint* assignedSpawn = nullptr;
	for(auto& sp : spawnpoints)
	{
		if(sp.occupied && sp.ownerID == clientID)
		{
			assignedSpawn = &sp;
			break;
		}
	}

	//if not assigned find the next available spawnPoint
	if(!assignedSpawn)
	{
		for (auto& sp : spawnpoints)
		{
			if (sp.GetAvailableSpawnpoint())
			{
				sp.occupied = true;
				sp.ownerID = clientID; // player ID
				assignedSpawn = &sp;
				break;
			}
		}
	}

	// Fallback: no spawn points available
	if (!assignedSpawn)
	{
		std::cerr << "SERVER: No available spawn point for client " << clientID << "!\n";
		return;
	}

	//Create and initalize the player spaceship
	auto& ship = players[clientID];
	ship.id = clientID;
	ship.position = assignedSpawn->position;
	ship.orientation = assignedSpawn->calcOrientationToOrigin();
	ship.transform = glm::translate(ship.position) * glm::mat4_cast(ship.orientation) * glm::scale(glm::vec3(1.0f));

	//Setup the spaceship collider
	if (!playerColliders.contains(clientID))
		playerColliders[clientID] = Physics::CreateCollider(playerMeshColliderID, ship.transform);
	else
		Physics::SetTransform(playerColliders[clientID], ship.transform);

	//SEND THE BROADCAST PACKAGE TO ALL CLIENT ABOUT A NEW USER CONNECTED
	auto playerData = BatchShip(ship);
	auto fbb = packet::SpawnPlayerS2C(&playerData);
	net_instance.Broadcast(server, fbb);
}

void GameServer::OnPacketRecieved(uint32_t senderID, const ENetPacket* packet)
{
	//if (packet == NULL) return; //NO PACKET
	auto wrapper = GetPacketWrapper(packet->data);
	switch(wrapper->packet_type())
	{
		case PacketType_InputC2S:{
			std::cout << "SERVER: RECIEVES A INPUT REQUEST FROM CLIENT\n";
			auto inputData = wrapper->packet_as_InputC2S();
			if (!inputData) return;
			auto& player = players[senderID]; //maybe need to navigate through the connections to find
			//apply the input (valid data)
			if (inputData->time() < player.lastInputTimeStamp) return;
			player.lastInputBitmap = inputData->bitmap();
			player.lastInputTimeStamp = inputData->time();
			player.inputCooldown = 0;
			//RECIEVES A INPUT EVENT
			std::cout << "Player input bitmap " << player.lastInputBitmap << "\n";
			std::cout << "Player input timestamp " << player.lastInputTimeStamp << "\n";

			break;
		}
		case PacketType_TextS2C:
			break;
		default:
			break;
	}
	
}

void GameServer::OnClientDisconnect(uint32_t clientID) {
	players.erase(clientID);
	std::cout << "SERVER: Client " << clientID << " disconnected.\n";
}

#pragma endregion

//REMOVE THESE TWO
//void GameServer::HandleIncomingPacket(uint32_t senderID, const PacketWrapper* packet)
//{
//	//Incoming package from client to server
//	if (!packet) return;
//
//	switch (packet->packet_type())
//	{
//	case PacketType::PacketType_InputC2S:
//	{
//		auto inputPacket = packet->packet_as_InputC2S();
//		if (inputPacket)
//			ProcessPlayerInput(senderID, inputPacket);
//		break;
//	}
//	default:
//		std::cout << "Unknown packet type from " << senderID << "\n";
//		break;
//	}
//}
//
//void GameServer::ProcessPlayerInput(uint32_t clientID, const InputC2S* packet)
//{
//	//PROCESS THE DATA OF THE PACKET FOR UPDATING THE SPACESHIP position,velocity etc
//	//IN UPDATE APPLY THE UPDATED VALUE TO THE SPACESHIP AND SEND IT BACK NEW WORLD STATE
//}


