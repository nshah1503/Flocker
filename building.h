//
//  building.hpp
//  FollowingBehaviour
//
//  Created by Naisheel on 01/04/24.
//
#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

class building_class{
public:
    //-----OBSTACLE COURSE----
    ofBoxPrimitive tower;
    
    ofTexture buildingTex;
    ofMaterial buildingMaterial;
    glm::vec3 startingPoint = glm::vec3(0, 1751, 0);
    //------------------------
    
    void setup();
    void draw();
    
    
    
    float width;
    float height;
    float depth;

    // Bounding box (in world coordinates)
    glm::vec3 minBounds;
    glm::vec3 maxBounds;
    void calculateBoundingBox();
    
};
