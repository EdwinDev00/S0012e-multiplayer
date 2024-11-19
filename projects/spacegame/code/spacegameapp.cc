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
//#include <vector>
#include "render/textureresource.h"
//#include "render/model.h"
#include "render/cameramanager.h"
#include "render/lightserver.h"
#include "render/debugrender.h"
#include "core/random.h"
#include "input/inputserver.h"
#include "core/cvar.h"
//#include "physics/physics.h"
#include <chrono>
#include "spaceship.h"

//#include "proto.h"


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
    this->window->SetSize(640, 360);

    if (this->window->Open())
	{
        Net::Initialize();
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
void SpaceGameApp::InitAsteroid()
{
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
        this->asteroids.push_back(asteroid);
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
}

void SpaceGameApp::InitSkyLight()
{
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
}

void
SpaceGameApp::Run()
{
    int w;
    int h;
    this->window->GetSize(w, h);
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), float(w) / float(h), 0.01f, 1000.f);
    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);
    cam->projection = projection;
    Input::Keyboard* kbd = Input::GetDefaultKeyboard();

    InitAsteroid();
    InitSkyLight();
    

    //TODO is Despawn player when quitting
    //implement the connects2c then make spawnplayer request
    // make sure the despawn player works
    // Gamestate update implement it
    // key action packet handler

    //Refactor code Sending packet calls communication
    //Network contains processing packet data
    //client and server only withdraw the packet data struct

    Render::ModelId shipid = LoadModel("assets/space/spaceship.glb");
    SpaceShip currentplayership;
    currentplayership.id = 1;
    //test this with spaceships*
    std::unordered_map<uint32_t, glm::mat4> shiptransforms;
    std::unordered_map<uint32_t, Game::SpaceShip*> ships;

    //client.setShipTranforms(&shiptransforms);
    client.setShip(&ships);
    std::clock_t c_start = std::clock();
    double dt = 0.01667f;

    // game loop
    while (this->window->IsOpen())
	{
        client.Poll();
        clientHost.Poll();
          
        auto timeStart = std::chrono::steady_clock::now();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
        glfwSwapInterval(1);
        
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

        currentplayership.Update(dt);

        for (auto& proj : projInWorld)
        {
            proj.Update(dt);
            RenderDevice::Draw(proj.model, proj.transform);
            //NEED TO FIND A SOLUTION OF CHECKING SHIPS COLLISION FIRST AFTERWARDS ASTEROIDS/other collision //server handling
        }

        //ship.CheckCollisions();
        for ( auto& [uuid,ship] : ships) //drawout the players spaceship
        {
            if (uuid == currentplayership.id)
            {
                //Particle still spawn need to look at
                std::cout << "SAME ID SKIP SKIP RENDERING THIS ONE\n";
                continue;
            }
            //if uuid = currentplayership skip the rendering because it always render out self
            RenderDevice::Draw(shipid, ship->transform);
        }
        
        RenderDevice::Draw(shipid, currentplayership.transform);
       

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
        //ImGui::Begin("Debug");
        //Core::CVar* r_draw_light_spheres = Core::CVarGet("r_draw_light_spheres");
        //int drawLightSpheres = Core::CVarReadInt(r_draw_light_spheres);
        //if (ImGui::Checkbox("Draw Light Spheres", (bool*)&drawLightSpheres))
        //    Core::CVarWriteInt(r_draw_light_spheres, drawLightSpheres);
        //
        //Core::CVar* r_draw_light_sphere_id = Core::CVarGet("r_draw_light_sphere_id");
        //int lightSphereId = Core::CVarReadInt(r_draw_light_sphere_id);
        //if (ImGui::InputInt("LightSphereId", (int*)&lightSphereId))
        //    Core::CVarWriteInt(r_draw_light_sphere_id, lightSphereId);
        //
        //ImGui::End();

        //Debug::DispatchDebugTextDrawing();
#endif // !DEBUG

        static char text[10000];
        ImGui::Begin("Network Control");
        ImGui::InputText("Server IP", ipAddress, sizeof(ipAddress));

        if (ImGui::Button("Connect"))
        {
            client.Create();

            if (client.Connect(ipAddress, 1234))
                std::cout << "CLIENT: CONNECTED TO SERVER\n";
            else
                std::cout << "CLIENT: FAILED TO CONNECT SERVER\n";
        }

        ImGui::InputText(" ", text, 10000);
        if (ImGui::Button("SEND"))
            client.SendPacket(text, 10000);
        //Host button
        if(ImGui::Button("Host"))
        {
            clientHost.Create(1234);
            client.Create();
            client.Connect(ipAddress, 1234);
        }

        //// Display connection status
        //if (peer && peer->state == ENET_PEER_STATE_CONNECTED)
        //{
        //    ImGui::Text("Status: Connected");
        //    isConnected = true;
        //}
        //else
        //{
        //    ImGui::Text("Status: Disconnected");
        //    isConnected = false;
        //}

        ImGui::End();
	}
}

} // namespace Game