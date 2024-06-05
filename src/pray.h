//
//  pray.hpp
//  FollowingBehaviour
//
//  Created by Naisheel on 31/03/24.
//

#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "building.h"

class flocker{
public:
    flocker(float x, float y, float z, float speed);

    //--GENERAL METHODS-----------
    void setup(const std::string& modelBasePath, int numModels);
    void update();
    void draw(float scale);
    //----------------------------
    
    //---LOAD MODELS and TEXTURES------
    std::vector<std::shared_ptr<ofxAssimpModelLoader>> birdModels;
    int textureIdx;
    //
    
    //------FOR UPDATE METHOD-----
    int currentModelIndex;
    float lastModelChange=0;
    float changeModelTime;
    //----------------------------
    
    //-----POSITION AND SPEED AND ROTATION------
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float maxSpeed;
    float maxForce;
    glm::quat currentOrientation;
    //----------------------------
    float size = 8.0;
    //----INTELLIGENCE FUNCTIONS and FORCES------
    glm::vec3 separation(const vector<flocker>& birds, float separationDistance);
    glm::vec3 alignment(const vector<flocker>& birds, float dist);
    glm::vec3 cohesion(const vector<flocker>& birds, float dist);
    glm::vec3 steerTowards(const glm::vec3& target);
    
    void integrate(vector<flocker>& birds, float speed);
    
    void applyForce(glm::vec3 force);
    
    glm::vec3 environmentalForce, environmentalForce2, separationForce, cohesionForce, alignmentForce;
    //--------------------------------
    
    //---CHECK BOUNDARY-----
    void checkBoundary();
    //----------------------
    
    //LEADER SEEK FUNCTION
    glm::vec3 seek(const glm::vec3& target);
    //CHASE FUNCTION
    void dispersion(const glm::vec3& predatorPosition, float threshold);
    
    // OBSTACLE AVOIDANCE
//    glm::vec3 vision();
    glm::vec3 vision(const building_class& building);
    building_class obj;
    glm::vec3 buildingPoint = glm::vec3(0, 1751, 0);
    float buildingLen = 3500;

};
