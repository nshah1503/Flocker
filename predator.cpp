//
//  predator.cpp
//  FollowingBehaviour
//
//  Created by Naisheel on 31/03/24.
//

#include "predator.h"

roboBoid::roboBoid(){
    // Your constructor's implementation
    position = glm::vec3(0, 200, 0);
    setup("robot/RobotObj_frame", 16);
    orientation = glm::vec3(0, 0, 0);
    maxSpeed = 3.0;
    maxForce = 0.5;
    changeModelTime=0.04;
    
    velocity = glm::vec3(0,0,0);
    acceleration = glm::vec3(0,0,0);
}
void roboBoid::setup(const std::string& modelBasePath, int numModels) {
    // Set up your texture, models, etc.
    // Similar to what you have in ofApp::setup()
    lastModelChange = ofGetElapsedTimef();
    changeModelTime = 1.0f / 24.0f; // Animation at 24 FPS
    
    //for irregular flaps
    int beginningFlap = ofRandom(1, numModels);
    for (int modelNum=beginningFlap; modelNum<=numModels; ++modelNum){
        auto model = make_unique<ofxAssimpModelLoader>();
        std::string modelPath = modelBasePath + ofToString(modelNum) + ".obj"; // Ensuring two digits for the model number
        modelPath = ofToDataPath(modelPath, true);
        if (!model->loadModel(modelPath)) {
            ofLogError() << "Can't load model " << modelPath;
            ofExit();
        } else {
            model->setRotation(0, 180, 1, 0, 0);
            model->setScaleNormalization(false);
            roboBoid_anim.push_back(std::move(model));
        }
    }
}
void roboBoid::update() {
    // Update your animation frame
    float timeNow = ofGetElapsedTimef();
    if(timeNow - lastModelChange > changeModelTime) {
        currentModelIndex = (currentModelIndex + 1) % roboBoid_anim.size();
        lastModelChange = timeNow;
    }
}

void roboBoid::integrate(unordered_map<int,bool>&keyMap, int OF_KEY_UP, float thrust) {
    update();
    float rotationAmount = 0.2*thrust; // Degrees to rotate per key press
    float moveAmount = 4.0; // Units to move per key press
    float forwardAmount = thrust; // TEMPORARY VALUE
    
    glm::vec3 forwardVec = glm::vec3(sin(glm::radians(orientation.y)), 0, -cos(glm::radians(orientation.y)));
    if(keyMap['w'] || keyMap['W']){
        position.y += moveAmount*thrust;
    }
    if(keyMap['s'] || keyMap['S']){
        position.y -= moveAmount*thrust;
    }
    if(keyMap['a'] || keyMap['A']){
        orientation.y -= rotationAmount;
    }
    if(keyMap['d'] || keyMap['D']){
        orientation.y += rotationAmount;
    }
    if(keyMap[OF_KEY_UP]) {
        position += forwardVec * forwardAmount;
    }
    if(position.y < 15) {
        position.y = 15; // Adjust to the plane's y position to make it sit on the ground
    }
}

void roboBoid::draw(float scale) {
    if(currentModelIndex < roboBoid_anim.size()) {
        auto& model = roboBoid_anim[currentModelIndex];
        
        glm::vec3 scaleFactors(scale * size, scale * size, scale * size); // Uniform scaling
        model->setScale(scaleFactors.x, scaleFactors.y, scaleFactors.z);
        model->setPosition(position.x, position.y, position.z);
        model->setRotation(0, orientation.y, 0, 1, 0);
        model->setRotation(1, 180, 1, 0, 0);
        model->drawFaces();
    } else {
        ofLogError() << "Current model index is out of range.";
    }
}



// FOR AUTOMATIC CHASE
void roboBoid::calculateSteeringForce(const glm::vec3& target, float maxSpeed) {
    glm::vec3 desired = target - position; // Direction to target
    desired = glm::normalize(desired) * maxSpeed; // Scale to maximum speed
    glm::vec3 steer = desired - velocity; // Steering = Desired minus Velocity
    steer = glm::clamp(steer, -maxForce, maxForce); // Limit steering by max force
    acceleration += steer; // Apply steering to acceleration
}
void roboBoid::autoChaseUpdate(const glm::vec3& target, float thrust) {
    calculateSteeringForce(target, thrust); // Calculate and apply steering force towards target
    
    velocity += acceleration; // Update velocity with acceleration
    position += velocity; // Update position with velocity
    acceleration *= 0; // Optionally reset acceleration after each update
}
void roboBoid::applyForce(glm::vec3 force) {
    acceleration += force;
}
void roboBoid::seek(const glm::vec3& target, unordered_map<int,bool>&keyMap, const int OF_KEY_UP, float thrust) {
    // Simple steering towards a target
    glm::vec3 desired = target - position;
    desired = glm::normalize(desired) * thrust;
    autoChaseUpdate(target, thrust);
    applyForce(desired - velocity); 
    
    integrate(keyMap, OF_KEY_UP, thrust);
}
