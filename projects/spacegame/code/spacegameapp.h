#pragma once
//------------------------------------------------------------------------------
/**
	Space game application

	(C) 20222 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"

#include <enet/enet.h>

namespace Game
{
class SpaceGameApp : public Core::App
{
public:
	/// constructor
	SpaceGameApp();
	/// destructor
	~SpaceGameApp();

	/// open app
	bool Open();
	/// run app
	void Run();
	/// exit app
	void Exit();
private:

	/// show some ui things
	void RenderUI();

	Display::Window* window;
	char ipAddress[16] = "127.0.0.1";  // Default IP for localhost
	bool isConnected = false;          // Track connection status
	ENetHost* client = nullptr;         // Client connection host
	ENetPeer* peer = nullptr;           // Client's peer connection
	ENetHost* server = nullptr;         // Server host when hosting
};
} // namespace Game