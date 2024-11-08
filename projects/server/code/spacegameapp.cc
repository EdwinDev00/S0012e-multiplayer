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

#include <enet/enet.h>
#include "proto.h"

using namespace Display;
using namespace Render;

namespace Game
{
    extern std::vector<Projectile> projInWorld;
    ENetHost* clientW;
    ENetPeer* serverpeerW;

    //TODO: SETTING UP THE SERVER AND UDP SOCKET BETWEEN CLIENT AND SERVER
    //ORGANIZE THE CLIENT SIDE SO it send package to server for testing connection
    //still needs fixing with the projectile hit with asteroids

    //haven't created the socket yet
    flatbuffers::Offset<Protocol::InputC2S> SerializeInput(flatbuffers::FlatBufferBuilder& builder, uint64_t time, uint16_t bitmap)
    {
        //input message
        return Protocol::CreateInputC2S(builder, time, bitmap);
    }

    flatbuffers::Offset<Protocol::TextC2S> SerializeText(flatbuffers::FlatBufferBuilder& builder, const std::string& text)
    {
        //text message
        return Protocol::CreateTextC2S(builder, builder.CreateString(text));
    }

    void SendInputC2S(ENetPeer* peer, uint64_t time, uint16_t bitmap)
    {
        flatbuffers::FlatBufferBuilder builder;
        auto inputOffset = SerializeInput(builder, time, bitmap);
        auto packageWrapper = Protocol::CreatePacketWrapper(builder, Protocol::PacketType::PacketType_InputC2S, inputOffset.Union());
        builder.Finish(packageWrapper);

        ENetPacket* packet = enet_packet_create(builder.GetBufferPointer(), builder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        enet_host_flush(clientW);
    }

    void SendTextC2S(ENetPeer *peer, const std::string &text)
    {
        flatbuffers::FlatBufferBuilder builder;
        auto textOffset = SerializeText(builder, text);
        auto packageWrapper = Protocol::CreatePacketWrapper(builder, Protocol::PacketType::PacketType_TextC2S, textOffset.Union());
        builder.Finish(packageWrapper);

        ENetPacket *packet = enet_packet_create(builder.GetBufferPointer(), builder.GetSize(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        enet_host_flush(clientW);
    }



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

        //Init ENET
        if(enet_initialize() != 0)
        {
            std::cerr << "An error occurred while initialize ENET\n";
            return false;
        }

        ENetHost* client = enet_host_create(nullptr, 1, 2, 0, 0);
        if(client == nullptr)
        {
            std::cerr << "An error occurred while trying to create a ENET Client host \n";
            enet_deinitialize();
            return false;
        }

        //Connect to server 
        ENetAddress address;
        enet_address_set_host(&address, "127.0.0.1"); //server ip (current localhost)
        address.port = 1234; // server port 

        ENetPeer* peer = enet_host_connect(client, &address, 2, 0);
        if(peer == nullptr)
        {
            std::cerr << "No available peers for initiating an ENET connection\n";
            enet_host_destroy(client);
            enet_deinitialize();
            return false;
        }

        clientW = client;
        serverpeerW = peer;
        
        std::cerr << "Succes of initiating an ENET connection\n";


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
    if(clientW)
    {
        //Disconnect from the server
        //enet_peer_disconnect(serverpeerW, 0);
        //ENetEvent event;
        //while(enet_host_service(clientW, &event,3000) > 0)
        //{
        //    if (event.type == ENET_EVENT_TYPE_RECIEVE)
        //        enet_packet_destroy(event.packet);
        //    else if (event.type == ENET_EVENT_TYPE_DISCONECT)
        //        break;
        //}
        enet_peer_reset(serverpeerW);
        enet_host_destroy(clientW);
        enet_deinitialize();
    }

    clientW = nullptr;
    serverpeerW = nullptr;
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
#ifndef DEBUG
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
        
	}
}

} // namespace Game