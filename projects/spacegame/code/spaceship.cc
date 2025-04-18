#include "config.h"
#include "spaceship.h"
#include "input/inputserver.h"
#include "render/cameramanager.h"
<<<<<<< Updated upstream
#include "physics/physics.h"
#include "render/debugrender.h"
#include "render/particlesystem.h"

=======
#include "render/particlesystem.h"

#include "network/network.h"
#include <chrono>
#include <gtx/string_cast.hpp>

//#include "physics/physics.h"
////#include "render/debugrender.h"


>>>>>>> Stashed changes
using namespace Input;
using namespace glm;
using namespace Render;

namespace Game
{
<<<<<<< Updated upstream
SpaceShip::SpaceShip()
{
    uint32_t numParticles = 2048;
    this->particleEmitterLeft = new ParticleEmitter(numParticles);
    this->particleEmitterLeft->data = {
        .origin = glm::vec4(this->position + (vec3(this->transform[2]) * emitterOffset),1),
        .dir = glm::vec4(glm::vec3(-this->transform[2]), 0),
        .startColor = glm::vec4(0.38f, 0.76f, 0.95f, 1.0f) * 2.0f,
        .endColor = glm::vec4(0,0,0,1.0f),
        .numParticles = numParticles,
        .theta = glm::radians(0.0f),
        .startSpeed = 1.2f,
        .endSpeed = 0.0f,
        .startScale = 0.025f,
        .endScale = 0.0f,
        .decayTime = 2.58f,
        .randomTimeOffsetDist = 2.58f,
        .looping = 1,
        .emitterType = 1,
        .discRadius = 0.020f
    };
    this->particleEmitterRight = new ParticleEmitter(numParticles);
    this->particleEmitterRight->data = this->particleEmitterLeft->data;
=======
    //Globally instance spaceship manager (only need to import spaceship.h for usage)
    SpaceshipManager ssm_instance = SpaceshipManager::Instance();

    //TESTING FOR GAME MANAGER TO STORE FOR THE SERVER
   // std::vector<Projectile> projInWorld;

//SpaceShip::SpaceShip()
//{
//    static uint32_t count = 0;
//    id = count;
//    count++;
//
//    // Load the spaceship model and create a collider
//    model = LoadModel("assets/space/spaceship.glb");
//    colliderMesh = Physics::LoadColliderMesh("assets/space/spaceship_physics.glb");
//    colliderID = Physics::CreateCollider(colliderMesh, transform);
//
//    // Initialize particle emitters for the spaceship thrusters
//    uint32_t numParticles = 2048;
//    this->particleEmitterLeft = new ParticleEmitter(numParticles);
//    this->particleEmitterRight = new ParticleEmitter(numParticles);
//    this->particleEmitterLeft->data = {
//        .origin = glm::vec4(this->position + (vec3(this->transform[2]) * emitterOffset),1),
//        .dir = glm::vec4(glm::vec3(-this->transform[2]), 0),
//        .startColor = glm::vec4(0.38f, 0.76f, 0.95f, 1.0f) * 2.0f,
//        .endColor = glm::vec4(0,0,0,1.0f),
//        .numParticles = numParticles,
//        .theta = glm::radians(0.0f),
//        .startSpeed = 1.2f,
//        .endSpeed = 0.0f,
//        .startScale = 0.025f,
//        .endScale = 0.0f,
//        .decayTime = 2.58f,
//        .randomTimeOffsetDist = 2.58f,
//        .looping = 1,
//        .emitterType = 1,
//        .discRadius = 0.020f
//    };
//    this->particleEmitterRight->data = this->particleEmitterLeft->data;
//
//    ParticleSystem::Instance()->AddEmitter(this->particleEmitterLeft);
//    ParticleSystem::Instance()->AddEmitter(this->particleEmitterRight);
//}
//
//void
//SpaceShip::Update(float dt)
//{
//    Mouse* mouse = Input::GetDefaultMouse();
//    Keyboard* kbd = Input::GetDefaultKeyboard();
//
//    Camera* cam = CameraManager::GetCamera(CAMERA_MAIN);
//
//    if (kbd->held[Key::W])
//    {
//        if (kbd->held[Key::Shift])
//            this->currentSpeed = mix(this->currentSpeed, this->boostSpeed, std::min(1.0f, dt * 30.0f));
//        else
//            this->currentSpeed = mix(this->currentSpeed, this->normalSpeed, std::min(1.0f, dt * 90.0f));
//    }
//    else
//    {
//        this->currentSpeed = 0;
//    }
//
//  //  if (kbd->pressed[Key::Space]) OnFire();
//
//    vec3 desiredVelocity = vec3(0, 0, this->currentSpeed);
//    desiredVelocity = this->transform * vec4(desiredVelocity, 0.0f);
//
//    this->linearVelocity = mix(this->linearVelocity, desiredVelocity, dt * accelerationFactor);
//
//    float rotX = kbd->held[Key::Left] ? 1.0f : kbd->held[Key::Right] ? -1.0f : 0.0f;
//    float rotY = kbd->held[Key::Up] ? -1.0f : kbd->held[Key::Down] ? 1.0f : 0.0f;
//    float rotZ = kbd->held[Key::A] ? -1.0f : kbd->held[Key::D] ? 1.0f : 0.0f;
//
//    this->position += this->linearVelocity * dt * 10.0f;
//
//    const float rotationSpeed = 1.8f * dt;
//    rotXSmooth = mix(rotXSmooth, rotX * rotationSpeed, dt * cameraSmoothFactor);
//    rotYSmooth = mix(rotYSmooth, rotY * rotationSpeed, dt * cameraSmoothFactor);
//    rotZSmooth = mix(rotZSmooth, rotZ * rotationSpeed, dt * cameraSmoothFactor);
//    quat localOrientation = quat(vec3(-rotYSmooth, rotXSmooth, rotZSmooth));
//    this->orientation = this->orientation * localOrientation;
//    this->rotationZ -= rotXSmooth;
//    this->rotationZ = clamp(this->rotationZ, -45.0f, 45.0f);
//    mat4 T = translate(this->position) * (mat4)this->orientation;
//    this->transform = T * (mat4)quat(vec3(0, 0, rotationZ));
//    this->rotationZ = mix(this->rotationZ, 0.0f, dt * cameraSmoothFactor);
//
//    // update camera view transform
//    vec3 desiredCamPos = this->position + vec3(this->transform * vec4(0, camOffsetY, -4.0f, 0));
//    this->camPos = mix(this->camPos, desiredCamPos, dt * cameraSmoothFactor);
//    cam->view = lookAt(this->camPos, this->camPos + vec3(this->transform[2]), vec3(this->transform[1]));
//
//    const float thrusterPosOffset = 0.365f;
//    this->particleEmitterLeft->data.origin = glm::vec4(vec3(this->position + (vec3(this->transform[0]) * -thrusterPosOffset)) + (vec3(this->transform[2]) * emitterOffset), 1);
//    this->particleEmitterLeft->data.dir = glm::vec4(glm::vec3(-this->transform[2]), 0);
//    this->particleEmitterRight->data.origin = glm::vec4(vec3(this->position + (vec3(this->transform[0]) * thrusterPosOffset)) + (vec3(this->transform[2]) * emitterOffset), 1);
//    this->particleEmitterRight->data.dir = glm::vec4(glm::vec3(-this->transform[2]), 0);
//    
//    float t = (currentSpeed / this->normalSpeed);
//    this->particleEmitterLeft->data.startSpeed = 1.2 + (3.0f * t);
//    this->particleEmitterLeft->data.endSpeed = 0.0f  + (3.0f * t);
//    this->particleEmitterRight->data.startSpeed = 1.2 + (3.0f * t);
//    this->particleEmitterRight->data.endSpeed = 0.0f + (3.0f * t);
//}
//
//bool
//SpaceShip::CheckCollisions()
//{
//    //IMPROVEMENT COULD USE A BETTER METHOD (currently uses physis.glb for detecting)
//    glm::mat4 rotation = (glm::mat4)orientation;
//    bool hit = false;
//    for (int i = 0; i < 8; i++)
//    {
//        glm::vec3 pos = position;
//        glm::vec3 dir = rotation * glm::vec4(glm::normalize(colliderEndPoints[i]), 0.0f);
//        float len = glm::length(colliderEndPoints[i]);
//        Physics::RaycastPayload payload = Physics::Raycast(position, dir, len);
//
//        // debug draw collision rays
//        Debug::DrawLine(pos, pos + dir * len, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);
//
//        if (payload.hit)
//        {
//            this->position = glm::vec3(0, 0, 40);
//            hit = true;
//            Debug::DrawDebugText("HIT", payload.hitPoint, glm::vec4(1, 1, 1, 1));
//        }
//
//        //TODO is dying by laser
//        //if (projInWorld.size() > 0)
//        //{
//        //    for(auto it = projInWorld.begin(); it != projInWorld.end();) //currently this will never get hit by a projectile
//        //    {
//        //        if (it->hit && it->hitColliderINDEX == this->colliderID.index) //server side to check if the projectile hit and erase message
//        //        {
//        //            std::cout << "you got hit " << '\n';
//        //            it = projInWorld.erase(it);
//        //        }
//        //        else it++;
//        //    }
//        //}
//    }
//    return hit;
//}

//void SpaceShip::OnFire()
//{
//    glm::vec3 projectileDirection = glm::normalize(glm::vec4(glm::vec3(transform[2]),0)); //forward direction of the spaceship
//    glm::vec3 spawnLocation = position + projectileDirection * 2.0f; // constant value = offset from the space ship
//    projInWorld.emplace_back(glm::translate(glm::mat4(1.0f), spawnLocation) * glm::mat4_cast(this->orientation));
//
//    // initalize by transform 4x4
//   // projectiles.emplace_back(glm::translate(glm::mat4(1.0f), spawnLocation) * glm::mat4_cast(this->orientation));
//    // initalize by pos , direction , orientation (quat)
//    //projectiles.emplace_back(spawnLocation, projectileDirection,this->orientation);
//}

>>>>>>> Stashed changes

//Client spaceship 
 void ClientSpaceship::InitSpaceship(){
    //Load spaceship model
     model = LoadModel("assets/space/spaceship.glb");

     //Initalize particle emitters for thrusters
     uint32_t numParticles = 2048;
     particleEmitterLeft = new Render::ParticleEmitter(numParticles);
     particleEmitterRight = new Render::ParticleEmitter(numParticles);

     //Configure particle emitters for thrusters
     this->particleEmitterLeft->data = {
          .origin = glm::vec4(this->position + (vec3(this->transform[2]) * emitterOffset),1),
          .dir = glm::vec4(glm::vec3(-this->transform[2]), 0),
          .startColor = glm::vec4(0.38f, 0.76f, 0.95f, 1.0f) * 2.0f,
          .endColor = glm::vec4(0,0,0,1.0f),
          .numParticles = numParticles,
          .theta = glm::radians(0.0f),
          .startSpeed = 1.2f,
          .endSpeed = 0.0f,
          .startScale = 0.025f,
          .endScale = 0.0f,
          .decayTime = 2.58f,
          .randomTimeOffsetDist = 2.58f,
          .looping = 1,
          .emitterType = 1,
          .discRadius = 0.020f
     };

<<<<<<< Updated upstream
    if (kbd->held[Key::W])
    {
        if (kbd->held[Key::Shift])
            this->currentSpeed = mix(this->currentSpeed, this->boostSpeed, std::min(1.0f, dt * 30.0f));
        else
            this->currentSpeed = mix(this->currentSpeed, this->normalSpeed, std::min(1.0f, dt * 90.0f));
    }
    else
    {
        this->currentSpeed = 0;
    }
    vec3 desiredVelocity = vec3(0, 0, this->currentSpeed);
    desiredVelocity = this->transform * vec4(desiredVelocity, 0.0f);
=======
     this->particleEmitterRight->data = this->particleEmitterLeft->data;

     ParticleSystem::Instance()->AddEmitter(this->particleEmitterLeft);
     ParticleSystem::Instance()->AddEmitter(this->particleEmitterRight);
 }

 ClientSpaceship::ClientSpaceship(uint32_t uniqueID)
 {
     id = uniqueID;
    // InitSpaceship();
 }
>>>>>>> Stashed changes

 void ClientSpaceship::ProcessInput()
 {
     Keyboard* kbd = Input::GetDefaultKeyboard();

     //Reset movement inputs
     this->inputState.ResetInputHistory();

     ////Movement input listener
     //if(kbd->held[Key::W])
     //{
     //    inputState.moveForward = true;
     //    if (kbd->held[Key::Shift]) {
     //        inputState.boost = true;
     //    }
     //}

     ////Rotation input
     //inputState.rotX = kbd->held[Key::Left] ? 1.0f : kbd->held[Key::Right] ? -1.0f : 0.0f;
     //inputState.rotY = kbd->held[Key::Up] ? -1.0f : kbd->held[Key::Down] ? 1.0f : 0.0f;
     //inputState.rotZ = kbd->held[Key::A] ? -1.0f : kbd->held[Key::D] ? 1.0f : 0.0f;

     ////Fire input
     //if (kbd->held[Key::Space])
     //    inputState.fire = true;

     //TESTING WITH THE BITMASK FOR SENDING THE CLIENT INPUT TO SERVER
     //LocalState
     inputState.moveForward = kbd->held[Key::W];
     inputState.boost = kbd->held[Key::Shift];
     inputState.rotX = kbd->held[Key::Left] ? 1.0f : kbd->held[Key::Right] ? -1.0f : 0.0f;
     inputState.rotY = kbd->held[Key::Up] ? -1.0f : kbd->held[Key::Down] ? 1.0f : 0.0f;
     inputState.rotZ = kbd->held[Key::A] ? -1.0f : kbd->held[Key::D] ? 1.0f : 0.0f;
     inputState.fire = kbd->held[Key::Space];

<<<<<<< Updated upstream
bool
SpaceShip::CheckCollisions()
{
    glm::mat4 rotation = (glm::mat4)orientation;
    bool hit = false;
    for (int i = 0; i < 8; i++)
    {
        glm::vec3 pos = position;
        glm::vec3 dir = rotation * glm::vec4(glm::normalize(colliderEndPoints[i]), 0.0f);
        float len = glm::length(colliderEndPoints[i]);
        Physics::RaycastPayload payload = Physics::Raycast(position, dir, len);

        // debug draw collision rays
        // Debug::DrawLine(pos, pos + dir * len, 1.0f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1), Debug::RenderMode::AlwaysOnTop);

        if (payload.hit)
        {
            Debug::DrawDebugText("HIT", payload.hitPoint, glm::vec4(1, 1, 1, 1));
            hit = true;
        }
    }
    return hit;
}
=======
     //Input bitmap
     unsigned short bitmap = 0;
     bitmap |= (kbd->held[Key::W] ? 1 << 0 : 0);
     bitmap |= (kbd->held[Key::A] ? 1 << 1 : 0);
     bitmap |= (kbd->held[Key::D] ? 1 << 2 : 0);
     bitmap |= (kbd->held[Key::Up] ? 1 << 3 : 0);
     bitmap |= (kbd->held[Key::Down] ? 1 << 4 : 0);
     bitmap |= (kbd->held[Key::Left] ? 1 << 5 : 0);
     bitmap |= (kbd->held[Key::Right] ? 1 << 6 : 0);
     bitmap |= (kbd->held[Key::Space] ? 1 << 7 : 0);
     bitmap |= (kbd->held[Key::Shift] ? 1 << 8 : 0);

    //Timestamp (UNIX epoc in ms)
     auto now = std::chrono::system_clock::now();
     auto duration = now.time_since_epoch();
     uint64_t epoch_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

     inputState.timeSet = epoch_ms;
     inputState.bitmap = bitmap;

     ////Build the packet and send
     //auto fbb = packet::InputC2S(epoch_ms, bitmap);
     //net_instance.SendToServer(fbb);


 }

 void ClientSpaceship::UpdateCamera(float dt)
 {
     cam = CameraManager::GetCamera(CAMERA_MAIN);
     const vec3 desiredCamPos = this->position + vec3(this->transform * vec4(0, camOffsetY, -4.0f, 0));
     this->camPos = this->camPos = mix(this->camPos, desiredCamPos, dt * cameraSmoothFactor);
     cam->view = 
         lookAt(this->camPos,
             this->camPos + vec3(this->transform[2]),
             vec3(this->transform[1]));
 }

 void ClientSpaceship::UpdateLocally(float dt)
 {
     // Apply input-based acceleration
     if(inputState.moveForward){
         float targetSpeed = inputState.boost ? boostSpeed : normalSpeed;
         currentSpeed = glm::mix(normalSpeed, targetSpeed, dt * accelerationFactor);
     }
     else {
         currentSpeed = glm::mix(currentSpeed, 0.0f, dt * accelerationFactor);
     }

     //Compute the new velocity based on direction
     glm::vec3 desiredVelocity = glm::vec3(0.0f, 0.0f, this->currentSpeed);
     desiredVelocity = orientation * desiredVelocity;

     //Apply velocity smoothing
     linearVelocity = glm::mix(linearVelocity, desiredVelocity, dt * accelerationFactor);

     //Apply position update
     position += linearVelocity * dt;

     //Handle rotation from input
     float rotationSpeed = 1.8f * dt;
     rotXSmooth = glm::mix(rotXSmooth, inputState.rotX * rotationSpeed, dt * cameraSmoothFactor);
     rotYSmooth = glm::mix(rotYSmooth, inputState.rotY * rotationSpeed, dt * cameraSmoothFactor);
     rotZSmooth = glm::mix(rotZSmooth, inputState.rotZ * rotationSpeed, dt * cameraSmoothFactor);

     glm::quat localRotation = glm::quat(glm::vec3(-rotYSmooth, rotXSmooth, rotZSmooth));
     orientation = normalize(orientation * localRotation);

     //Limiting rotation
     rotationZ -= rotXSmooth;
     rotationZ = glm::clamp(rotationZ, -45.0f, 45.0f);

     //Compute final transform matrix
     transform = glm::translate(position) * glm::mat4_cast(orientation) * glm::scale(glm::vec3(1.0f));
     //    * glm::rotate(glm::mat4(1), glm::radians(rotationZ), glm::vec3(0, 0, 1));

     const float thrusterPosOffset = 0.365f;
     this->particleEmitterLeft->data.origin = glm::vec4(vec3(this->position + (vec3(this->transform[0]) * -thrusterPosOffset)) + (vec3(this->transform[2]) * emitterOffset), 1);
     this->particleEmitterLeft->data.dir = glm::vec4(glm::vec3(-this->transform[2]), 0);
     this->particleEmitterRight->data.origin = glm::vec4(vec3(this->position + (vec3(this->transform[0]) * thrusterPosOffset)) + (vec3(this->transform[2]) * emitterOffset), 1);
     this->particleEmitterRight->data.dir = glm::vec4(glm::vec3(-this->transform[2]), 0);

     float t = (currentSpeed / this->normalSpeed);
     this->particleEmitterLeft->data.startSpeed = 1.2 + (3.0f * t);
     this->particleEmitterLeft->data.endSpeed = 0.0f + (3.0f * t);
     this->particleEmitterRight->data.startSpeed = 1.2 + (3.0f * t);
     this->particleEmitterRight->data.endSpeed = 0.0f + (3.0f * t);

 }

 void ClientSpaceship::CorrectFromServer(glm::vec3 newPos, glm::quat newOrient, glm::vec3 newVel, uint64_t timestamp)
 {
     const float correctionFactor = 0.2f; //We can adjust the smoothness
     
     auto now = std::chrono::system_clock::now();
     auto duration = now.time_since_epoch();
     uint64_t nowMillis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

     float latencySec = (nowMillis - timestamp) / 1000.0f;


     // Orientation Correction
     const float angleThreshold = glm::radians(1.0f); // or use glm::degree with degress
     glm::quat q1 = glm::normalize(this->orientation);
     glm::quat q2 = glm::normalize(newOrient);
     //Compute the angular difference
     float dotProduct = glm::dot(q1, q2);
     float angleBetween = 2.0f * glm::acos(glm::clamp(dotProduct, -1.0f, 1.0f)); // in radians

     if (angleBetween > angleThreshold) //Check if the angle difference is greater than our threshold to change
         this->orientation = glm::slerp(orientation, newOrient, correctionFactor);

     // Velocity correction with directional allignment
     //glm::vec3 forward = orientation * glm::vec3(0, 0, currentSpeed);
     //float serverVelMag = glm::length(newVel);
     //if(serverVelMag > 0.01f)
     //{
     //    glm::vec3 serverDir = glm::normalize(newVel);
     //    float dotDir = glm::dot(serverDir, normalize(forward));

     //    if(dotDir > 0.9)
     //    {
     //        // Server velocity matches ship direction closely -> trust it
     //    }
     //    else
     //    {
     //        //Mismatch -> reallign velocity to forward vector with same speed
     //        glm::vec3 realligned = forward * serverVelMag;
     //        this->linearVelocity = glm::mix(this->linearVelocity, realligned, correctionFactor);
     //    }
     //}
     this->linearVelocity = glm::mix(this->linearVelocity, newVel, correctionFactor);

     glm::vec3 predictedServerPos = newPos + linearVelocity * latencySec;

     // Position Correction
     if (glm::distance(position, newPos) > 0.05f)
         this->position = glm::mix(position, predictedServerPos, correctionFactor);

     //Recalculate transform matrix / Final transform update
     this->transform = glm::translate(this->position) * glm::mat4_cast(this->orientation) * glm::scale(glm::vec3(1.0f));
        // * glm::rotate(glm::mat4(1), glm::radians(rotationZ), glm::vec3(0, 0, 1));
 }

 void ClientSpaceship::Render()
 {
     if (!model) return; //Ensure model is valid before rendering

     //Draw the spaceship model
     RenderDevice::Draw(model, transform);

     //Update particle effect for thrusters
     particleEmitterLeft->data.origin = glm::vec4(position + glm::vec3(transform[0]) * -0.365f + glm::vec3(transform[2]) * emitterOffset, 1);
     particleEmitterRight->data.origin = glm::vec4(position + glm::vec3(transform[0]) * 0.365f + glm::vec3(transform[2]) * emitterOffset, 1);
 }


 //Server spaceship
 void ServerSpaceship::Update(float dt)
 {
     inputCooldown += dt;
     const float inputTimeout = 0.2f; // 200ms without input = stop

     if(inputCooldown > inputTimeout)
     {
         lastInputBitmap = 0; //no input held anymore
         lastInputTimeStamp = 0;
     }

     //Movement input
     bool forward = lastInputBitmap & (1 << 0);
     bool boost = lastInputBitmap & (1 << 8);

     // Rotation input
     float rotX = (lastInputBitmap & (1 << 5)) ? -1.0f : (lastInputBitmap & (1 << 6)) ? 1.0f : 0.0f;
     float rotY = (lastInputBitmap & (1 << 4)) ? -1.0f : (lastInputBitmap & (1 << 3)) ? 1.0f : 0.0f;
     float rotZ = (lastInputBitmap & (1 << 1)) ? -1.0f : (lastInputBitmap & (1 << 2)) ? 1.0f : 0.0f;

     if (forward)
         currentSpeed = boost ? boostSpeed : normalSpeed;
     else
         currentSpeed = 0;

     //Apply acceleration to velocity
     glm::vec3 desiredVelocity = glm::vec3(0, 0, currentSpeed);
     desiredVelocity = orientation * desiredVelocity; // Apply orientation to movement
     linearVelocity = glm::mix(linearVelocity, desiredVelocity, dt * accelerationFactor);

     //Update position
     position += linearVelocity * dt;

     //update rotation quat
     float rotationSpeed = 1.8f * dt;
     const float smoothFactor = 10;
     rotXSmooth = glm::mix(rotXSmooth, rotX * rotationSpeed, dt * smoothFactor);
     rotYSmooth = glm::mix(rotYSmooth, rotY * rotationSpeed, dt * smoothFactor);
     rotZSmooth = glm::mix(rotZSmooth, rotZ * rotationSpeed, dt * smoothFactor);

     glm::quat localRotation = glm::quat(glm::vec3(-rotYSmooth, rotXSmooth, rotZSmooth));
     orientation = normalize(orientation * localRotation);

     //Limiting rotation
     //rotationZ -= rotXSmooth;
     //rotationZ = glm::clamp(rotationZ, -45.0f, 45.0f);

     //Update transformation matrix
     transform = glm::translate(position) * glm::mat4_cast(orientation) * glm::scale(glm::vec3(1.0f));
        // * glm::rotate(glm::mat4(1), glm::radians(rotationZ), glm::vec3(0, 0, 1));
 }

 bool ServerSpaceship::CheckCollision()
 {
     glm::mat3 rotation = glm::mat3(transform);  //use transforms orientation for rotation
     glm::vec3 position = glm::vec3(transform[3]); //current position data inside the transform
     bool hit = false;
     for(int i = 0; i < 8; i++)
     {
         glm::vec3 direction = rotation * vec4(normalize(colliderEndPoints[i]), 0.0f);
         const float len = glm::length(colliderEndPoints[i]);
         const Physics::RaycastPayload payload = Physics::Raycast(position, direction, len);

         if(payload.hit)
         {
             Debug::DrawDebugText("HIT", payload.hitPoint, vec4(1, 1, 1, 1));
             hit = true;
         }
     }
     return hit;
 }

 void ServerSpaceship::SyncWithClient()
 {
     //sends server updated packet back to client
     // SETUP THE PACKET BEFORE SENDING IT TO CLIENT
     //ENET PACKET FOR EVERYTHING (PLAYERID, POSITION, ORIENTATION, VELOCITY, etc..)
     //SEND THE PACKET USING FLATBUFFER TO THE CLIENTS (CONNECTED CLIENTS NEEDS TO GET THE UPDATE)

 }


 //Spaceship manager
 SpaceshipManager& SpaceshipManager::Instance()
 {
     static SpaceshipManager instance;
     return instance;
 }

 void SpaceshipManager::AddSpaceship(uint32_t id, bool isLocalPlayer, bool isServer)
 {
     if(!isServer)
     {
         if(c_Spaceships.find(id) == c_Spaceships.end())
         {
             //Construct the object inside the container avoiding copying or moving
             c_Spaceships.emplace(id, id); // SAME LOGIC spaceships.emplace(std::make_pair(id, ClientSpaceship(id)));
             if (isLocalPlayer) localPlayerID = id;
         }
     }
     else
     {
         s_spaceships.emplace(id, id);
     }
 }

 void SpaceshipManager::RemoveSpaceship(uint32_t id)
 {
     c_Spaceships.erase(id);
 }

 void SpaceshipManager::UpdateAll(float dt)
 {
     for(auto& [id,spaceship] : c_Spaceships)
     {
         if(id == localPlayerID)
         {
             spaceship.ProcessInput(); //Handle input for local player
             spaceship.UpdateLocally(dt); //Predict movement
             spaceship.UpdateCamera(dt); // only update the local player's camera
         }
     }
 }

 void SpaceshipManager::RenderAll()
 {
     //set the local player's camera before rendering
     if(auto* localShip = GetSpaceship(localPlayerID))
     {
         localShip->cam = CameraManager::GetCamera(CAMERA_MAIN);
         localShip->cam->view =
             lookAt(localShip->camPos,
                 localShip->camPos + vec3(localShip->transform[2]),
                 vec3(localShip->transform[1]));
     }

     //Render all spaceships
     for (auto& [id, spaceship] : c_Spaceships)
         spaceship.Render();
 }


 std::unordered_map<uint32_t, ServerSpaceship>& SpaceshipManager::GetSList()
 {
     // TODO: insert return statement here
     return s_spaceships;
 }

 ClientSpaceship* SpaceshipManager::GetSpaceship(uint32_t id)
 {
     auto it = c_Spaceships.find(id);
     return (it != c_Spaceships.end()) ? &it->second : nullptr;
 }


>>>>>>> Stashed changes
}