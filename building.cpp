//
//  building.cpp
//  FollowingBehaviour
//
//  Created by Naisheel on 01/04/24.
//

#include "building.h"

void building_class::setup(){
    ofLoadImage(buildingTex, "building/buildingTex.jpeg");
    buildingTex.generateMipmap();
    
    tower.set(1000, 3500, 1000);
    tower.setPosition(startingPoint.x, startingPoint.y, startingPoint.z);
    
    buildingMaterial.setDiffuseColor(ofColor(255, 255, 255)); // Set the color of the material
    buildingMaterial.setSpecularColor(ofColor(0, 0, 0)); // No specular reflection
    buildingMaterial.setShininess(0.1);
}
void building_class::draw(){
    buildingMaterial.begin();
    buildingTex.bind();
    tower.draw();
    buildingTex.unbind();
    buildingMaterial.end();
}

void building_class::calculateBoundingBox() {
    minBounds = startingPoint - glm::vec3(width / 2, 0, depth / 2);
    maxBounds = startingPoint + glm::vec3(width / 2, height, depth / 2);
}
