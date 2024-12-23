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


//CLIENT AND HOSTING CLIENT APPLICATION
//APPLY GAME SYNCRONIZATION BETWEEN GAME STATE AND THE SERVER LOGIC

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
    
    Render::ModelId shipid = LoadModel("assets/space/spaceship.glb");

    std::unordered_map<uint32_t, glm::mat4> shiptransforms;
    std::unordered_map<uint32_t, Game::SpaceShip*> ships;

    //client.setShipTranforms(&shiptransforms);
    client.setShip(&ships);
    std::clock_t c_start = std::clock();
    double dt = 0.01667f;


    /*
    TASK LIST GUIDELINE:
        - ESTABLISH CONNECTION FOR CLIENT HOST ARCHITECTURE (COMPLETE)
            - CONNECTS TO SERVER  (DONE)
            - OTHER CLIENT ABLE TO CONNECT TO SAME SERVER (DONE)

        CURRENT PROBLEM: CLIENT CANNOT RECIEVE SEVERAL PACKAGE AT ONCE (NOT RECIEVING CONNECTS2C PACKAGE FOR PROCESS)

        - HANDLE CREATING PLAYER AVATAR IN THE GAME (CURRENT WORKING TASK)
            - SUCCESSFUL CONNECTION REQUEST CREATE THE PLAYER STRUCT FOR STORING DATA ABOUT THE CONNECTED USER
            - SERVER SUCCESSFUL CREATED USER NOTIFY ALL THE OTHER PLAYER TO PERFORM SPAWN REQUEST FOR SPAWNING THE NEW PLAYER (STILL NEED SOME WORK)
                - WHEN A NEW PLAYER JOINS IS NOT CONTROLLING THE INITAL SPACESHIP (IMPORTANT)
            - THE NEWLY ADDED PLAYER GET THE LIST OF THE CURRENT PLAYER CONNECTED LIST (WORKING ON)
            - PLAYER RENDERER OUT ALL THE CONNECTED CLIENT AVATARS IN THE LIST
            - HANDLE DISCONNECTED USER (UPDATE GAME STATE AND REMOVE THAT USER FROM ALL THE CONNECTED USER)
    */


    // game loop
    while (this->window->IsOpen())
	{
        client.Poll();
        clientHost.Poll();

        //INPUT LISTENING (only if the client is connected)
        if(client.isActive())
        {
            if (kbd->pressed[Input::Key::W])
                std::cout << "CLIENT CALLING IN THE LOOP SENDING INPUT PACKET\n";
        }
          
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

        for (auto& proj : projInWorld)
        {
            proj.Update(dt);
            RenderDevice::Draw(proj.model, proj.transform);
            //NEED TO FIND A SOLUTION OF CHECKING SHIPS COLLISION FIRST AFTERWARDS ASTEROIDS/other collision //server handling
        }

        for ( auto& [uuid,ship] : ships) //drawout the players spaceship
        {            
            //IT SHOULD AWAIT FOR GAMESTATAE UPDATE HERE
            RenderDevice::Draw(shipid, ships[uuid]->transform);
           //ships[uuid]->Update(dt);
           // 
        //THIS UPDATE LOOP SHOULD BE INSIDE GAMESTATE UPDATE
           ships[client.clientControlledID]->Update(dt); //TESTING CURRENTLY
        }

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