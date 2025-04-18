#pragma once
#include "render/model.h"
<<<<<<< Updated upstream
=======
#include "physics/physics.h"
#include "render/debugrender.h"

#include <iostream>
#include <vec3.hpp>

>>>>>>> Stashed changes

namespace Render
{
    struct ParticleEmitter;
    struct Camera;
}

//namespace Physics
//{
//    struct ColliderMeshId;
//    struct ColliderId;
//}

namespace Game
{

<<<<<<< Updated upstream
struct SpaceShip
{
    SpaceShip();
    
    glm::vec3 position = glm::vec3(0);
    glm::quat orientation = glm::identity<glm::quat>();
    glm::vec3 camPos = glm::vec3(0, 1.0f, -2.0f);
    glm::mat4 transform = glm::mat4(1);
    glm::vec3 linearVelocity = glm::vec3(0);

    const float normalSpeed = 1.0f;
    const float boostSpeed = normalSpeed * 2.0f;
    const float accelerationFactor = 1.0f;
=======
//struct Projectile
//{
//    glm::vec3 position;
//    glm::vec3 direction;
//    glm::mat4 transform;
//    Render::ModelId model;
//    bool hit = false;
//    float speed = 25.0f;
//    float lifeTime = 2;
//    
//    int hitColliderINDEX = -1; //store the obj index (currently keeping track of everything including asteroids) 
//
//    Projectile(glm::vec3 startPos, glm::vec3 dir, glm::quat orientation) : position(startPos), direction(dir)
//    {
//        model = Render::LoadModel("assets/space/laser.glb");
//        transform = glm::translate(glm::mat4(1), startPos) * glm::mat4_cast(orientation);
//    }
//
//    Projectile(const glm::mat4 initalTransform) : transform(initalTransform)
//    {
//        model = Render::LoadModel("assets/space/laser.glb");
//        position = transform[3]; // fetch translate (posiiton) from matrix
//        direction = glm::normalize(glm::vec3(transform[2]));
//    }
//
//    void Update(float deltaTime)
//    {
//        position += direction * speed * deltaTime;
//        lifeTime -= deltaTime;
//        // Update the transform based on the new position, keeping the initial orientation
//        transform = glm::translate(glm::mat4(1), position) * glm::mat4_cast(glm::quatLookAt(direction, glm::vec3(0, 1, 0)));
//
//        if(CheckCollision())
//        {
//            //PACKAGE THE LASER AND SEND THE HITCOLLIDER INDEX AS MESSAGE
//            //SERVER CHECK THE HITCOLLIDER INDEX AND THE LASER OWNER (server resolve skip checking against the owner of the laser)
//            //Collider list need to be accessed inside the server
//             Destroy();
//        }
//
//        //Debug::DrawBox(this->transform, glm::vec4(1, 0, 0, 1));
//        if (lifeTime <= 0) Destroy();
//    }
//
//    bool CheckCollision()
//    {
//       /* Physics::RaycastPayload payload = Physics::Raycast(position, direction, glm::length(direction));
//        if (payload.hit)
//        {
//            hitColliderINDEX = payload.collider.index;
//            std::cout << hitColliderINDEX << "\n";
//            Debug::DrawDebugText("HIT LASER", payload.hitPoint, glm::vec4(1, 1, 1, 1));
//            hit = true;
//        }*/
//        return hit;
//    }
//
//    void Destroy()
//    {
//        //mark for delete
//        hit = true;
//    }; //Destroy the projectile (by time or collided)
//};

//Original remove before handing in
//struct SpaceShip
//{
//    SpaceShip();
//    
//    //uuid id for the current client id
//    uint32_t id;
//
//    glm::vec3 position = glm::vec3(0);
//    glm::quat orientation = glm::identity<glm::quat>();
//    glm::vec3 camPos = glm::vec3(0, 1.0f, -2.0f);
//    glm::mat4 transform = glm::mat4(1);
//    glm::vec3 linearVelocity = glm::vec3(0);
//
//    //keep track of the projectiles fired by this unit
//    //std::vector<Projectile> projectiles;
//
//    const float normalSpeed = 1.0f;
//    const float boostSpeed = normalSpeed * 2.0f;
//    const float accelerationFactor = 1.0f;
//    const float camOffsetY = 1.0f;
//    const float cameraSmoothFactor = 10.0f;
//
//    float currentSpeed = 0.0f;
//
//    float rotationZ = 0;
//    float rotXSmooth = 0;
//    float rotYSmooth = 0;
//    float rotZSmooth = 0;
//
//    Render::ModelId model;
//    Render::ParticleEmitter* particleEmitterLeft;
//    Render::ParticleEmitter* particleEmitterRight;
//    float emitterOffset = -0.5f;
//    
//    //Create collider mesh ID for (collision)
//    Physics::ColliderMeshId colliderMesh;
//    Physics::ColliderId colliderID;
//
//    void Update(float dt);
//
//    bool CheckCollisions();
//
//    
//    const glm::vec3 colliderEndPoints[8] = {
//        glm::vec3(-1.10657, -0.480347, -0.346542),  // right wing
//        glm::vec3(1.10657, -0.480347, -0.346542),  // left wing
//        glm::vec3(-0.342382, 0.25109, -0.010299),   // right top
//        glm::vec3(0.342382, 0.25109, -0.010299),   // left top
//        glm::vec3(-0.285614, -0.10917, 0.869609), // right front
//        glm::vec3(0.285614, -0.10917, 0.869609), // left front
//        glm::vec3(-0.279064, -0.10917, -0.98846),   // right back
//        glm::vec3(0.279064, -0.10917, -0.98846)   // right back
//    };
//
//    //void OnFire();
//}; 


//Complete: basic setup network manager
//TODO BUILD THE NETWORK MANAGER, CLIENT & SERVER LOGIC SIDE
// Setup GameClient & GameServer logic


//CLIENT STATES

struct InputState //PLayer input Data client to server (input updates)
{
    bool moveForward = false;
    bool boost = false;
    bool fire = false;
    float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

    void ResetInputHistory()
    {
        moveForward = false;
        boost = false;
        fire = false;
        rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
    }

    uint64_t timeSet; //The time this was pressed and send
    unsigned short bitmap = 0;
};

struct ClientSpaceship
{
    //Unique spaceship ID for networking
    uint32_t id = 0;

    //Rendering
    Render::ModelId model; //there's only one model (init it directly)
    Render::ParticleEmitter* particleEmitterLeft;
    Render::ParticleEmitter* particleEmitterRight;
    float emitterOffset = -0.5f;

    //Camera 
    Render::Camera* cam;
    glm::vec3 camPos = glm::vec3(0, 1.0f, -2.0f);
>>>>>>> Stashed changes
    const float camOffsetY = 1.0f;
    const float cameraSmoothFactor = 10.0f;

    //Player input
    InputState inputState;

    //Rotation values
    float rotationZ = 0;
    float rotXSmooth = 0;
    float rotYSmooth = 0;
    float rotZSmooth = 0;

<<<<<<< Updated upstream
    Render::ModelId model;
    Render::ParticleEmitter* particleEmitterLeft;
    Render::ParticleEmitter* particleEmitterRight;
    float emitterOffset = -0.5f;
=======
    // Predicted Movement (locally physics & movement)
    glm::vec3 position = glm::vec3(0);
    glm::quat orientation = glm::identity<glm::quat>();
    glm::vec3 linearVelocity = glm::vec3(0);
    glm::mat4 transform = glm::mat4(1);
>>>>>>> Stashed changes

    const float normalSpeed = 10.0f;
    const float boostSpeed = normalSpeed * 2.0f;
    const float accelerationFactor = 1.0f;
    float currentSpeed = 0.0f;

<<<<<<< Updated upstream
    bool CheckCollisions();
    
=======

    ClientSpaceship() = default; //Default constructor

    ClientSpaceship(uint32_t uniqueID); //Multiplayer constructor

    void ProcessInput();  // Handles input from player
    void UpdateCamera(float dt);  // Updates camera position
    void Render();        // Handles rendering spaceship
    void UpdateLocally(float dt); // Handles local client prediction
    void CorrectFromServer(glm::vec3 newPos, glm::quat newOrient, glm::vec3 newVel,uint64_t timestamp);  // Fixes desync

    void InitSpaceship();
private:
};

//SERVER STATES
struct SpaceShipState { //Server to client (state updates)
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 velocity;
    float speed;
};

struct ServerSpaceship
{
    uint32_t id; //UNique spaceship id for networking (synchronize with associated client ID)

    //InputState input; //Input Data information which recieves from the client to server
    uint16_t lastInputBitmap = 0;
    uint64_t lastInputTimeStamp = 0;
    float inputCooldown = 0;

    //Physics & movement
    glm::vec3 position = glm::vec3(0);
    glm::quat orientation = glm::identity<glm::quat>();
    glm::vec3 linearVelocity = glm::vec3(0);
    glm::mat4 transform = glm::mat4(1);

    float normalSpeed = 1.0f;
    float boostSpeed = normalSpeed * 2.0f;
    float accelerationFactor = 1.0f;
    float currentSpeed = 0.0f;

    //Rotation values
    float rotationZ = 0;
    float rotXSmooth = 0;
    float rotYSmooth = 0;
    float rotZSmooth = 0;

    //Collidermesh should be generate in the server when initalize the player to the game

    // Collision
>>>>>>> Stashed changes
    const glm::vec3 colliderEndPoints[8] = {
        glm::vec3(-1.10657, -0.480347, -0.346542),  // right wing
        glm::vec3(1.10657, -0.480347, -0.346542),  // left wing
        glm::vec3(-0.342382, 0.25109, -0.010299),   // right top
        glm::vec3(0.342382, 0.25109, -0.010299),   // left top
        glm::vec3(-0.285614, -0.10917, 0.869609), // right front
        glm::vec3(0.285614, -0.10917, 0.869609), // left front
        glm::vec3(-0.279064, -0.10917, -0.98846),   // right back
        glm::vec3(0.279064, -0.10917, -0.98846)   // left back
    };
<<<<<<< Updated upstream
=======

    ServerSpaceship() = default;
    ServerSpaceship(uint32_t spaceshipID) : id(spaceshipID){}

    void Update(float dt); //updates spaceship movement
    bool CheckCollision(); //validate collisions 
    void SyncWithClient(); //synchronize with the clients
>>>>>>> Stashed changes
};


//DONT NEED THIS MANAGER
// Spaceship manager singleton
class SpaceshipManager
{
public:
    static SpaceshipManager& Instance(); //Singleton Instance

    void AddSpaceship(uint32_t id, bool isLocalPlayer, bool isServer = false);
    void RemoveSpaceship(uint32_t id);
    void UpdateAll(float dt);
    void RenderAll();

    ClientSpaceship* GetSpaceship(uint32_t);
    std::unordered_map<uint32_t, ServerSpaceship>& GetSList();

private:
    std::unordered_map<uint32_t, ClientSpaceship> c_Spaceships;
    std::unordered_map<uint32_t, ServerSpaceship> s_spaceships;

    uint32_t localPlayerID = 0; //Stores ID of the local player
};

extern SpaceshipManager ssm_instance;

}