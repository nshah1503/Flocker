//
//  predator.hpp
//  FollowingBehaviour
//
//  Created by Naisheel on 31/03/24.
//
#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

class roboBoid {
public:
    roboBoid();

    
    int currentModelIndex;
    float lastModelChange=0;
    float changeModelTime;
    
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 position;
    float maxSpeed;
    float maxForce;
    glm::quat currentOrientation;

    float size = 15.0;
    std::vector<std::shared_ptr<ofxAssimpModelLoader>> roboBoid_anim;

    void draw(float scale);
    void update();
    void setup(const std::string& modelBasePath, int numModels);
    void integrate(unordered_map<int,bool>&keyMap, int OF_KEY_UP, float thrust);
    
    glm::vec3 orientation;
    
    // AUTOMATIC CHASE
    void calculateSteeringForce(const glm::vec3& target, float maxSpeed);
    void autoChaseUpdate(const glm::vec3& target, float thrust);
    void seek(const glm::vec3& target, unordered_map<int,bool>&keyMap, const int OF_KEY_UP, float thrust);
    void applyForce(glm::vec3 force);
};
