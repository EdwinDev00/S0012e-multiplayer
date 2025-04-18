#pragma once
//------------------------------------------------------------------------------
/**
	Space game application

	(C) 20222 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"

#include "render/model.h"
#include "physics/physics.h"

namespace Game
{
class SpaceGameApp : public Core::App
{
public:
	/// constructor
	SpaceGameApp() {};
	/// destructor
	~SpaceGameApp(){};

	/// open app
	bool Open();
	/// run app
	void Run();
	/// exit app
	void Exit();
private:

	/// show some ui things
	void RenderUI();

	void SetupScene() {};
	void InitAsteroid() {};
	void InitSkyLight() {};

	//list of objects
	std::vector<std::tuple<Render::ModelId, Physics::ColliderId, glm::mat4>> asteroids;
	

	Display::Window* window;
	char ipAddress[16] = "127.0.0.1";  // Default IP for localhost
	bool isConnected = false;          // Track connection status
	bool isHost = false;
	//Net::Client client;
	//Net::Server clientHost;
	//Client2 client2;

	//testing the ENet connection

};
} // namespace Game