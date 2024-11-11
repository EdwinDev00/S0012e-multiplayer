//------------------------------------------------------------------------------
// spacegameapp.cc
// (C) 2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "spacegameapp.h"
#include <cstring>
#include "imgui.h"
#include "render/renderdevice.h"
#include "render/shaderresource.h"
#include <vector>
#include "render/textureresource.h"
#include "render/model.h"
#include "render/cameramanager.h"
#include "render/lightserver.h"
#include "render/debugrender.h"
#include "core/random.h"
#include "input/inputserver.h"
#include "core/cvar.h"
#include "physics/physics.h"
#include <chrono>
#include "spaceship.h"

//#include <enet/enet.h>
#include "proto.h"

using namespace Display;
using namespace Render;

namespace Game
{
    extern std::vector<Projectile> projInWorld;
//------------------------------------------------------------------------------
/**
*/
SpaceGameApp::SpaceGameApp()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SpaceGameApp::~SpaceGameApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
SpaceGameApp::Open()
{
	App::Open();
	this->window = new Display::Window;
    this->window->SetSize(2500, 2000);

    if (this->window->Open())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        RenderDevice::Init();

		// set ui rendering function
		this->window->SetUiRender([this]()
		{
			this->RenderUI();
		});
        
        return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::Run()
{
    //CLIENT USER (MAIN JOB IS RENDER THE SCENE )
    /*
    The client's frame loop looks something like the following: (VALVE basic architecture)

        1.Sample clock to find start time
        2.Sample user input (mouse, keyboard, joystick)
        3.Package up and send movement command using simulation time
        4.Read any packets from the server from the network system
        5.Use packets to determine visible objects and their state
        6.Render Scene
        7.Sample clock to find end time
        8.End time minus start time is the simulation time for the next frame
    */

    int w;
    int h;
    this->window->GetSize(w, h);
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(w) / float(h), 0.01f, 1000.f);
    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);
    cam->projection = projection;

    // load all resources
    ModelId models[6] = {
        LoadModel("assets/space/Asteroid_1.glb"),
        LoadModel("assets/space/Asteroid_2.glb"),
        LoadModel("assets/space/Asteroid_3.glb"),
        LoadModel("assets/space/Asteroid_4.glb"),
        LoadModel("assets/space/Asteroid_5.glb"),
        LoadModel("assets/space/Asteroid_6.glb")
    };
    Physics::ColliderMeshId colliderMeshes[6] = {
        Physics::LoadColliderMesh("assets/space/Asteroid_1_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_2_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_3_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_4_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_5_physics.glb"),
        Physics::LoadColliderMesh("assets/space/Asteroid_6_physics.glb")
    };

    std::vector<std::tuple<ModelId, Physics::ColliderId, glm::mat4>> asteroids;
    
    // Setup asteroids near
    for (int i = 0; i < 100; i++)
    {
        std::tuple<ModelId, Physics::ColliderId, glm::mat4> asteroid;
        size_t resourceIndex = (size_t)(Core::FastRandom() % 6);
        std::get<0>(asteroid) = models[resourceIndex];
        float span = 20.0f;
        glm::vec3 translation = glm::vec3(
            Core::RandomFloatNTP() * span,
            Core::RandomFloatNTP() * span,
            Core::RandomFloatNTP() * span
        );
        glm::vec3 rotationAxis = normalize(translation);
        float rotation = translation.x;
        glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
        std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
        std::get<2>(asteroid) = transform;
        asteroids.push_back(asteroid);
    }

    // Setup asteroids far
    for (int i = 0; i < 50; i++)
    {
        std::tuple<ModelId, Physics::ColliderId, glm::mat4> asteroid;
        size_t resourceIndex = (size_t)(Core::FastRandom() % 6);
        std::get<0>(asteroid) = models[resourceIndex];
        float span = 80.0f;
        glm::vec3 translation = glm::vec3(
            Core::RandomFloatNTP() * span,
            Core::RandomFloatNTP() * span,
            Core::RandomFloatNTP() * span
        );
        glm::vec3 rotationAxis = normalize(translation);
        float rotation = translation.x;
        glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
        std::get<1>(asteroid) = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
        std::get<2>(asteroid) = transform;
        asteroids.push_back(asteroid);
    }

    // Setup skybox
    std::vector<const char*> skybox
    {
        "assets/space/bg.png",
        "assets/space/bg.png",
        "assets/space/bg.png",
        "assets/space/bg.png",
        "assets/space/bg.png",
        "assets/space/bg.png"
    };
    TextureResourceId skyboxId = TextureResource::LoadCubemap("skybox", skybox, true);
    RenderDevice::SetSkybox(skyboxId);
    
    Input::Keyboard* kbd = Input::GetDefaultKeyboard();

    const int numLights = 40;
    Render::PointLightId lights[numLights];
    // Setup lights
    for (int i = 0; i < numLights; i++)
    {
        glm::vec3 translation = glm::vec3(
            Core::RandomFloatNTP() * 20.0f,
            Core::RandomFloatNTP() * 20.0f,
            Core::RandomFloatNTP() * 20.0f
        );
        glm::vec3 color = glm::vec3(
            Core::RandomFloat(),
            Core::RandomFloat(),
            Core::RandomFloat()
        );
        lights[i] = Render::LightServer::CreatePointLight(translation, color, Core::RandomFloat() * 4.0f, 1.0f + (15 + Core::RandomFloat() * 10.0f));
    }


    //GAME MANAGER DATA STORAGE
    /*
    * SPACESHIP VECTOR which stores all the spaceship in the game (server)
    * 
    */
    SpaceShip ship;

    SpaceShip ship2;
    //ship2.model = LoadModel("assets/space/spaceship.glb");

    std::clock_t c_start = std::clock();
    double dt = 0.01667f;

    std::vector<int> markForDelete;

    // game loop
    while (this->window->IsOpen())
	{

        if (client && !isConnected) {
            ENetEvent event;
            if (enet_host_service(client, &event, 1000) > 0) {
                switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT:
                    {
                        isConnected = true;
                        std::cout << "Successfully connected to server on port: "
                            << peer->address.port << std::endl;
                        //Send a connection package to the server flatbuffer
                        flatbuffers::FlatBufferBuilder builder; 
                        auto clientConnectpacket = Protocol::CreateClientConnectS2C(builder, /* uuid */ 1, /* time */ static_cast<uint64_t>(time(nullptr) * 1000));
                        auto packetWrapper = Protocol::CreatePacketWrapper(builder, Protocol::PacketType_ClientConnectS2C, clientConnectpacket.Union());
                        builder.Finish(packetWrapper);

                        //send the clientConnectpackage to the server
                        ENetPacket* packet = enet_packet_create(builder.GetBufferPointer(), builder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
                        enet_peer_send(peer, 0, packet);
                        enet_host_flush(client);
                        break;
                    }
                
                case ENET_EVENT_TYPE_RECEIVE:
                    // Process the incoming packet from the server
                    std::cout << "Received packet from server.\n";
                    enet_packet_destroy(event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Disconnected from server." << std::endl;
                    isConnected = false;
                    break;

                default:
                    break;
                }
            }
        }

        auto timeStart = std::chrono::steady_clock::now();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
        
        this->window->Update();

        if (kbd->pressed[Input::Key::Code::End])
        {
            ShaderResource::ReloadShaders();
        }


        // Store all drawcalls in the render device
        for (auto const& asteroid : asteroids)
        {
            RenderDevice::Draw(std::get<0>(asteroid), std::get<2>(asteroid));
        }

        ship.Update(dt);

        for (auto& proj : projInWorld)
        {
            proj.Update(dt);
            RenderDevice::Draw(proj.model, proj.transform);
            //NEED TO FIND A SOLUTION OF CHECKING SHIPS COLLISION FIRST AFTERWARDS ASTEROIDS/other collision //server handling
        }

        ship.CheckCollisions();
        ship2.CheckCollisions();

        RenderDevice::Draw(ship.model, ship.transform);
        RenderDevice::Draw(ship2.model, ship2.transform);

        // Execute the entire rendering pipeline
        RenderDevice::Render(this->window, dt);

		// transfer new frame to window
		this->window->SwapBuffers();

        auto timeEnd = std::chrono::steady_clock::now();
        dt = std::min(0.04, std::chrono::duration<double>(timeEnd - timeStart).count());

        if (kbd->pressed[Input::Key::Code::Escape])
            this->Exit();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::Exit()
{
    if(client)
    {
        enet_peer_reset(peer);
        enet_host_destroy(client);
        enet_deinitialize();
    }
    if (server)
        delete server;

    delete client, peer;
    client = nullptr, peer = nullptr, server = nullptr;
    this->window->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
SpaceGameApp::RenderUI()
{
	if (this->window->IsOpen())
	{
#ifndef NDEBUG
        ImGui::Begin("Debug");
        Core::CVar* r_draw_light_spheres = Core::CVarGet("r_draw_light_spheres");
        int drawLightSpheres = Core::CVarReadInt(r_draw_light_spheres);
        if (ImGui::Checkbox("Draw Light Spheres", (bool*)&drawLightSpheres))
            Core::CVarWriteInt(r_draw_light_spheres, drawLightSpheres);
        
        Core::CVar* r_draw_light_sphere_id = Core::CVarGet("r_draw_light_sphere_id");
        int lightSphereId = Core::CVarReadInt(r_draw_light_sphere_id);
        if (ImGui::InputInt("LightSphereId", (int*)&lightSphereId))
            Core::CVarWriteInt(r_draw_light_sphere_id, lightSphereId);
        
        ImGui::End();

        Debug::DispatchDebugTextDrawing();
#endif // !DEBUG


        ImGui::Begin("Network Control");
        ImGui::InputText("Server IP", ipAddress, sizeof(ipAddress));

        if (ImGui::Button("Connect") && client == nullptr)
        {
            //Initialize ENET if not already done
            if (enet_initialize() != 0)
            {
                std::cerr << "CLIENT: FAILED TO INITALIZE CLIENT ENET\n";
                return;
            }
            
            client = enet_host_create(nullptr, 1, 2, 0, 0); // Create client host for one connection
            
            if (client == nullptr) { std::cerr << "CLIENT: FAILED TO CREATE CLIENT\n";  return; }
            
            //Connected to the server with the entered IP ADDRESS
            ENetAddress address;
            enet_address_set_host_ip(&address, ipAddress); //IP FROM IMGUI TEXT
            address.port = 1234; // server port
            peer = enet_host_connect(client, &address, 2, 0); //ATTEMPT TO CONNECT
            if (peer == nullptr)
                std::cout << "NO AVAILABLE PEERS FOR INITIATING CONNECTION.\n";
            else
                std::cout << "ATTEMPTING TO CONNECT TO SERVER...\n"; 
        }

        //Host button
        if(ImGui::Button("Host"))
        {
            //CREATE THE SERVER HOST
            if(!server)
            {
                ENetAddress address;
                address.host = ENET_HOST_ANY;
                address.port = 1234;
                server = enet_host_create(&address, 32, 2, 0, 0);
                if(!server)
                    std::cout << "ERROR: FAILED TO CREATE SERVER\n";
                else
                    std::cout << "SERVER RUNNING, LISTENING ON PORT: "  << address.port << "\n";
            }

            // Initialize client to connect to the hosted server
            if (!client)
            {
                client = enet_host_create(nullptr, 1, 2, 0, 0);
            }
            if (client)
            {
                ENetAddress address;
                enet_address_set_host(&address, "127.0.0.1");  // Connect to localhost
                address.port = 1234;

                peer = enet_host_connect(client, &address, 2, 0);
                if (peer)
                {
                    std::cout << "Client connected to hosted server.\n";
                }
            }
        }

        // Display connection status
        if (peer && peer->state == ENET_PEER_STATE_CONNECTED)
        {
            ImGui::Text("Status: Connected");
            isConnected = true;
        }
        else
        {
            ImGui::Text("Status: Disconnected");
            isConnected = false;
        }

        ImGui::End();
	}
}

} // namespace Game