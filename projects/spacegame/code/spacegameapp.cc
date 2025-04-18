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
#include "core/random.h"
#include "input/inputserver.h"

//#include "render/debugrender.h"

#include "core/cvar.h"

#include <chrono>
//#include "spaceship.h"

#include "network/server.h"
#include "network/client.h"

//#include "proto.h"

using namespace Display;
using namespace Render;

namespace Game
{

   // extern std::vector<Projectile> projInWorld;

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


    /*InitAsteroid();
    InitSkyLight();*/
    
    //WE are going to remove the ssm_instance
    //ssm_instance.AddSpaceship(1,true); //On player connect in the server (add the connected user)

    std::clock_t c_start = std::clock();
    float dt = 0.01667f;
    
    // game loop
    while (this->window->IsOpen())
	{
        //TESTING THE SERVER RUN LOOP
        gameServer.Run();
        gameClient.Update();

          
        auto timeStart = std::chrono::steady_clock::now();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

       // glfwSwapInterval(1);

        
        this->window->Update();

        if (kbd->pressed[Input::Key::Code::End])
        {
            ShaderResource::ReloadShaders();
        }

        // Draw some debug text
        Debug::DrawDebugText("FOOBAR", glm::vec3(0), {1,0,0,1});

        // Store all drawcalls in the render device
        for (auto const& asteroid : asteroids)
        {
            RenderDevice::Draw(std::get<0>(asteroid), std::get<2>(asteroid));
        }


        for(auto& ship : gameClient.spaceships)
        {
            //Make sure only apply camera, client prediction for this controlled client 
            if (ship.first == gameClient.myPlayerID)
            {
                //update this current user controlled avatar
                ship.second.ProcessInput(); //Handle input for local player
                if (ship.second.inputState.bitmap != 0) //might  need to reroute this before using
                    gameClient.SendInput(packet::InputC2S(ship.second.inputState.timeSet, ship.second.inputState.bitmap));
                ship.second.UpdateLocally(dt); //Predict movement
                ship.second.UpdateCamera(dt); // only update the local player's camera
            }
            else
            {
                //update the other connected users movements
                ship.second.UpdateLocally(dt);
            }
            RenderDevice::Draw(ship.second.model, ship.second.transform);
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

        static bool connected = false;
        static char text[10000];
        ImGui::Begin("Network Control");
        ImGui::InputText("Server IP", ipAddress, sizeof(ipAddress));

        if(!connected)
        {
            if (ImGui::Button("Host"))
            {
                gameServer.StartServer(1234);
                gameClient.Create();
                if (gameClient.ConnectToServer(ipAddress, 1234))
                {
                    //On success
                    isHost = true; //This client is the host of the server
                    connected = true;
                }
            }

            if (ImGui::Button("Connect"))
            {
                std::cout << "GAMEAPP: CLIENT REQUESTING A CONNECTION TO THE SERVER. NOTE: CLIENT CALL CONNECT() TO ALREADY INITALIZE SERVER\n";
                gameClient.Create();
                if (gameClient.ConnectToServer(ipAddress, 1234)) 
                    connected = true;
                else
                    std::cout << "GAMEAPP: Failed to send connection request to server, check if gameclient or gameServer is initalized correctly\n";
            }

            //ImGui::InputText(" ", text, 10000);
            //if (ImGui::Button("SEND"))
            //    client.SendPacket(text, 10000);
            //Host button

        }
        else
        {
            for (auto& ship : gameClient.spaceships) {
                ImGui::DragFloat3("Ship position", &ship.second.position[0]);
                ImGui::DragFloat4("Ship orientation", &ship.second.orientation[0]);
            }

            for(auto& sShip : gameServer.GetServerShip())
            {
                ImGui::DragFloat3("SERVER Ship position", &sShip.second.position[0]);
                ImGui::DragFloat4("SERVER Ship orientation", &sShip.second.orientation[0]);
            }

            if (ImGui::Button("Disconnect"))
                std::cout << "GAMEAPP: CLIENT SEND A DISCONNECT REQUEST TO THE SERVER TO HANDLE\n";
        }

        ImGui::End();

        Debug::DispatchDebugTextDrawing();
	}
}

} // namespace Game