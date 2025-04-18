#include "config.h"
#include "client.h"

#include "../projects/spacegame/code/spaceship.h"

GameClient gameClient = GameClient::Instance();

void GameClient::Create()
{
	//Create the client host
	client = enet_host_create(nullptr, 1, 1, 0, 0);
	if (!client)
	{
		std::cout << "Failed to create ENet Client!\n";
		return;
	}

	isActive = true;
}

bool GameClient::ConnectToServer(const char* ip, const uint16_t port )
{
	//Connect to the server 
	if (!isActive) return false; //Attempt to connect before client creation

	//Assign the address -> connect to server address
	enet_address_set_host(&address, ip);
	address.port = port;
	peer = enet_host_connect(client, &address, 1, 0);
	if(!peer)
	{
		std::cout << "CLIENT: Failed to establish connection request to peer at: " << address.host
			<< "; " << port << "\n";
		return false;
	}
	
	std::cout << "CLIENT: Successful initiate connection to ENET Server: Awaiting for server respond to request\n";
	return true; //Successful connecting to server
}

//ONCLIENTCONNECT SHOULD DO SOMETHING ABOUT CONNECTION

void GameClient::Update()
{
	if (!isActive) return;
	
	ENetEvent event;
	while (enet_host_service(client, &event, 0) > 0) //Pool
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT: {
			std::cout << "CLIENT TYPE CONNECT WAS GENERATED\n";
			//Call the respective callback
			//if (OnClientConnect)
		//	OnClientConnect(event.peer);
			break;
		}

		case ENET_EVENT_TYPE_RECEIVE: {
			OnRecievepacket(event.packet);
			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT: {
			break;
		}
		}
	}

	//Update the physics of the player based on the server update package
}

void GameClient::SendInput(const FlatBufferBuilder& builder)
{
	net_instance.SendToServer(this->peer, builder);
	//Client user input send to the server
}

void GameClient::RecieveServerUpdate(ENetPacket* packet)
{
	//Handle packet recieved by server
}

void GameClient::OnRecievepacket(ENetPacket* packet)
{
	//On packet recieved from the server
	auto wrapper = GetPacketWrapper(packet->data)->UnPack()->packet;
	switch (wrapper.type)
	{
		case PacketType_ClientConnectS2C:{
			std::cout << "CLIENT: Recieved Connect package\n";
			const auto clientConnectS2C = wrapper.AsClientConnectS2C();
			this->myPlayerID = clientConnectS2C->uuid;
			this->serverTime = clientConnectS2C->time;
			std::cout << "CLIENT: Connect package with uuid " << clientConnectS2C->uuid << "\n";
			std::cout << "CLIENT: Player ID " << myPlayerID << "\n";
			break;
		}
		case PacketType_GameStateS2C: {

			std::cout << "CLIENT: Recieved GAME STATE update\n";
			auto gameState = wrapper.AsGameStateS2C();
			//std::cout << "Client spaceship list count before adding: " << spaceships->size() << "\n";
			for(const auto& player : gameState->players)
			{
				spaceships.emplace(player.uuid(), Game::ClientSpaceship());
				Game::ClientSpaceship& ship = spaceships.at(player.uuid());
				ship.id = player.uuid();
				const Vec3& pos = player.position();
				const Vec3& vel = player.velocity();
				const Vec4& orient = player.direction();
				ship.position = glm::vec3(pos.x(), pos.y(), pos.z());
				ship.linearVelocity = glm::vec3(vel.x(), vel.y(), vel.z());
				ship.orientation = glm::quat(orient.x(), orient.y(), orient.z(), orient.w());
				ship.InitSpaceship();
				if(spaceships.count(player.uuid()))
				{
					std::cout << "Client spaceship with this ID: " << player.uuid() << " is present in the spaceshipMap\n";
				}
				else
					std::cout << "Client spaceship with this ID: " << player.uuid() << " is absent in the spaceshipMap\n";
			}
			break;
		}

		case PacketType_SpawnPlayerS2C:
		{
			//Spawn the player we initalized 
			std::cout << "CLIENT: RECIEVED SPAWNPLAYER PACKAGE\n";
			auto& player = wrapper.AsSpawnPlayerS2C()->player;
			spaceships.emplace(player->uuid(), Game::ClientSpaceship(player->uuid()));
			Game::ClientSpaceship& spaceship = spaceships.at(player->uuid());
			const Vec3& pos = player->position();
			const Vec4& orient = player->direction();
			spaceship.position = glm::vec3(pos.x(), pos.y(), pos.z());
			spaceship.orientation = glm::quat(orient.x(), orient.y(), orient.z(), orient.w());
			spaceship.InitSpaceship();
			std::cout << "CLIENT: spaceships count " << spaceships.size() << "\n";
			break;
		}

		case PacketType_UpdatePlayerS2C:
		{
			std::cout << "CLIENT: RECIEVED UpdatePlayerS2C PACKAGE\n";
			const auto updatePlayer = wrapper.AsUpdatePlayerS2C();
			auto& player = updatePlayer->player;
			if (!spaceships.contains(player->uuid())) return;
			Game::ClientSpaceship& ship = spaceships.at(player->uuid()); 
			glm::vec3 serverPs(player->position().x(), player->position().y(), player->position().z());
			glm::quat serverOr(player->direction().x(), player->direction().y(), player->direction().z(), player->direction().w());
			glm::vec3 serverVe(player->velocity().x(), player->velocity().y(), player->velocity().z());
			ship.CorrectFromServer(serverPs, serverOr, serverVe,updatePlayer->time);
			break;
		}
		default:
			break;
	}
}

