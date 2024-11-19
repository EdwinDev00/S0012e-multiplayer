//#pragma once
//#include "render/model.h"
//
//#include "render/debugrender.h"
//#include "physics/physics.h"
//
//#include <iostream>
//
//namespace Render
//{
//    struct ParticleEmitter;
//}
//
//namespace Game
//{
//
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
//        Physics::RaycastPayload payload = Physics::Raycast(position, direction, glm::length(direction));
//        if (payload.hit)
//        {
//            hitColliderINDEX = payload.collider.index;
//            std::cout << hitColliderINDEX << "\n";
//            Debug::DrawDebugText("HIT LASER", payload.hitPoint, glm::vec4(1, 1, 1, 1));
//            hit = true;
//        }
//        return hit;
//    }
//
//    void Destroy()
//    {
//        //mark for delete
//        hit = true;
//    }; //Destroy the projectile (by time or collided)
//};
//
//struct SpaceShip
//{
//    SpaceShip();
//    
//    glm::vec3 position = glm::vec3(0);
//    glm::quat orientation = glm::identity<glm::quat>();
//    glm::vec3 camPos = glm::vec3(0, 1.0f, -2.0f);
//    glm::mat4 transform = glm::mat4(1);
//    glm::vec3 linearVelocity = glm::vec3(0);
//
//    //keep track of the projectiles fired by this unit
//    std::vector<Projectile> projectiles;
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
//    void OnFire();
//};
//
//}