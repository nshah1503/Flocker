//
//  pray.cpp
//  FollowingBehaviour
//
//  Created by Naisheel on 31/03/24.
//

#include "pray.h"

flocker::flocker(float x, float y, float z, float speed){
    position = glm::vec3(x, y, z);

//    velocity = glm::vec3(vx, vy, vz) * speed;
    velocity = glm::vec3(ofRandom(-2, 2), ofRandom(-2, 2), ofRandom(-2,2));
    acceleration = glm::vec3(0, 0, 0);
    maxSpeed = speed;
    maxForce = 0.03;
    setup("flock/birdObj_frame", 13);
    textureIdx = ofRandom(0,3);
}

void flocker::setup(const std::string& modelBasePath, int numModels){
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
            birdModels.push_back(std::move(model));
        }
    }
}

void flocker::checkBoundary(){
    // Define the boundaries of your simulation space.
    float minX = -4900, maxX = 4900;
    float minY = 100, maxY = 4900; // Adjust minY to ensure birds stay above the plane
    float minZ = -4900, maxZ = 4900;

    // Apply immediate correction if out of bounds, with a soft push back if near the boundary
    float boundaryBuffer = 200; // Buffer distance from boundary where soft correction starts
    float correctionStrength = 0.1; // Strength of correction force

    // X-axis boundaries
    if (position.x < minX) {
        position.x = minX;
        velocity.x *= -1;
    } else if (position.x > maxX) {
        position.x = maxX;
        velocity.x *= -1;
    } else if (position.x < minX + boundaryBuffer) {
        acceleration.x += correctionStrength;
    } else if (position.x > maxX - boundaryBuffer) {
        acceleration.x -= correctionStrength;
    }

    // Y-axis boundaries (prevent going below the plane and correct if too high)
    if (position.y < minY) {
        position.y = minY;
        velocity.y *= -1; // Damping the bounce back
    } else if (position.y > maxY) {
        position.y = maxY;
        velocity.y *= -1;
    } else if (position.y > maxY - boundaryBuffer) {
        acceleration.y -= correctionStrength;
    }

    // Z-axis boundaries
    if (position.z < minZ) {
        position.z = minZ;
        velocity.z *= -1;
    } else if (position.z > maxZ) {
        position.z = maxZ;
        velocity.z *= -1;
    } else if (position.z < minZ + boundaryBuffer) {
        acceleration.z += correctionStrength;
    } else if (position.z > maxZ - boundaryBuffer) {
        acceleration.z -= correctionStrength;
    }
}



void flocker::update(){
    float timeNow = ofGetElapsedTimef();
    if(timeNow - lastModelChange > changeModelTime) {
        currentModelIndex = (currentModelIndex + 1) % birdModels.size();
        lastModelChange = timeNow;
    }
}
void flocker::draw(float scale){
    if(currentModelIndex < birdModels.size()) {
        auto& model = birdModels[currentModelIndex];
        glm::vec3 desiredForward = glm::normalize(velocity);
        glm::quat targetOrientation = glm::quatLookAt(desiredForward, glm::vec3(0, 1, 0)); 

        float interpolationFactor = 0.02f; // Adjust this factor to control the interpolation speed
        glm::quat newOrientation = glm::slerp(currentOrientation, targetOrientation, interpolationFactor);

        currentOrientation = newOrientation; // Update the current orientation

        glm::vec3 scaleFactors(scale * size, scale * size, scale * size); // Uniform scaling
        model->setScale(scaleFactors.x, scaleFactors.y, scaleFactors.z);
        model->setPosition(position.x, position.y, position.z);

        // Convert quaternion to matrix and apply it to the model
        glm::mat4 rotationMatrix = glm::toMat4(newOrientation);
        glm::vec3 euler = glm::eulerAngles(currentOrientation);
        euler = glm::degrees(euler);

        model->setRotation(0, euler.x, 1, 0, 0);
        model->setRotation(1, euler.y, 0, 1, 0);
        model->setRotation(2, euler.z, 0, 0, 1);
        
        model->setRotation(3, 180, 1, 0, 0);

        model->drawFaces();
    } else {
        ofLogError() << "Current model index is out of range.";
    }
}

//-----APPLY FORCES AND INTEGRATE------
void flocker::integrate(vector<flocker>& birds, float speed) {
    environmentalForce = glm::vec3(0.01, 0.01, 0.01);
    environmentalForce2 = glm::vec3(-0.01, -0.01, -0.01);
    applyForce(environmentalForce);
    applyForce(environmentalForce2);
    
    separationForce = separation(birds, 20.0);
    cohesionForce = cohesion(birds, 1400.0);
    alignmentForce = alignment(birds, 1400.0);
    
    maxSpeed = speed;
    separationForce *=1.5;
    cohesionForce *= 1.5;
    alignmentForce *= 1.0;
    
//    glm::vec3 evasion = vision();
    glm::vec3 evasion = vision(obj);
    
    applyForce(evasion);
    applyForce(separationForce);
    applyForce(cohesionForce);
    applyForce(alignmentForce);
    update();
    checkBoundary();
    velocity += acceleration;
    velocity = glm::clamp(velocity, -maxSpeed, maxSpeed);
    //ROTATE
    glm::vec3 desiredForward = glm::normalize(velocity);
    glm::quat targetOrientation = glm::quatLookAt(desiredForward, glm::vec3(0, 1, 0));

    float interpolationFactor = 0.02f;
    glm::quat newOrientation = glm::slerp(currentOrientation, targetOrientation, interpolationFactor);

    currentOrientation = newOrientation; // Update the current orientation
    
    glm::mat4 rotationMatrix = glm::toMat4(newOrientation);
    glm::vec3 euler = glm::eulerAngles(currentOrientation);
    euler = glm::degrees(euler);

    birdModels[currentModelIndex]->setRotation(0, euler.x, 1, 0, 0);
    birdModels[currentModelIndex]->setRotation(1, euler.y, 0, 1, 0);
    birdModels[currentModelIndex]->setRotation(2, euler.z, 0, 0, 1);
    speed = glm::length(velocity);
    position += velocity;
    
    acceleration *= 0;
    
}

void flocker::applyForce(glm::vec3 force) {
    acceleration += force;
}
//-----INTELLIGENCE FUNCTION DEFINITIONS------
glm::vec3 flocker::separation(const vector<flocker>& birds, float separationDistance) {
    glm::vec3 steer(0, 0, 0);
    int count = 0;

    for (const auto& other : birds) {
        float d = glm::distance(position, other.position);

        if ((d > 0) && (d < separationDistance)) {
            glm::vec3 diff = position - other.position;
            diff = glm::normalize(diff);
            diff /= d;
            steer += diff;
            count++;
        }
    }

    if (count > 0) {
        steer /= (float)count;
    }

    if (glm::length(steer) > 0) {
        steer = glm::normalize(steer);
        steer *= maxSpeed;
        steer -= velocity;
        steer = glm::min(steer, glm::vec3(maxForce, maxForce, maxForce)); // Limit to maximum steering force
    }

    return steer;
}

glm::vec3 flocker::cohesion(const vector<flocker>& birds, float neighborDist) {
    glm::vec3 sum(0, 0, 0); // Sum of all positions
    int count = 0;
    for (const auto& other : birds) {
        float d = glm::distance(position, other.position);
        if ((d > 0) && (d < neighborDist)) {
            sum += other.position; // Add position
            count++;
        }
    }
    if (count > 0) {
        sum /= count; // Average position
        return steerTowards(sum); // Steer towards the average position
    }
    return glm::vec3(0, 0, 0); // No steering if no neighbors close
}
glm::vec3 flocker::steerTowards(const glm::vec3& target) {
    glm::vec3 desired = target - position; // A vector pointing from the position to the target
    desired = glm::normalize(desired); // Normalize to get direction
    desired *= maxSpeed; // Scale by maxSpeed

    glm::vec3 steer = desired-velocity;
    return glm::length(steer) > maxForce ? glm::normalize(steer) * maxForce : steer; // Limit the steering force to maxForce
}

glm::vec3 flocker::alignment(const vector<flocker>& birds, float neighborDist) {
    glm::vec3 avgVelocity(0, 0, 0); // Average velocity
    int count = 0;
    for (const auto& other : birds) {
        float d = glm::distance(position, other.position);
        if ((d > 0) && (d < neighborDist)) {
            avgVelocity += other.velocity; // Sum up all velocities
            count++;
        }
    }
    if (count > 0) {
        avgVelocity /= (float)count; // Calculate the average velocity
        avgVelocity = glm::normalize(avgVelocity); // Normalize to get the direction
        avgVelocity *= maxSpeed; // Scale to maxSpeed
        // Steering = Desired - Velocity
        glm::vec3 steer = avgVelocity - velocity;
        return glm::length(steer) > maxForce ? glm::normalize(steer) * maxForce : steer; // Limit the steering force to maxForce
    }
    return glm::vec3(0, 0, 0); // No steering if no neighbors close
}

//SEEK THE LEADER
glm::vec3 flocker::seek(const glm::vec3& target) {
    glm::vec3 desired = target - position; // Vector from the flocker to the target
    desired = glm::normalize(desired);
    desired *= maxSpeed;

    // Steering = Desired minus Velocity
    glm::vec3 steer = desired - velocity;
    steer = glm::clamp(steer, -maxForce, maxForce); // Limit the steering force to maxForce
    return steer;
}
//DISPERSE
void flocker::dispersion(const glm::vec3& predatorPosition, float threshold) {
    glm::vec3 awayFromPredator = position - predatorPosition;
    float distance = glm::length(awayFromPredator);

    if (distance < threshold) {
        // Normalize the direction and scale by maxSpeed to get maximum dispersion effect
        glm::vec3 force = glm::normalize(awayFromPredator) * maxSpeed;
        applyForce(force);
    }
}

//OBSTACLE AVOIDANCE
glm::vec3 flocker::vision(const building_class& building) {
    glm::vec3 avoidanceVec(0, 0, 0);
    float safetyMargin = 500.0f; // distance within which birds start avoiding the building
    
    glm::vec3 futurePosition = position + velocity * 1; // Predict future position
    // Check if future position is inside the building's bounding box
    if (futurePosition.x > building.minBounds.x - safetyMargin &&
        futurePosition.x < building.maxBounds.x + safetyMargin &&
        futurePosition.y > building.minBounds.y - safetyMargin &&
        futurePosition.y < building.maxBounds.y + safetyMargin &&
        futurePosition.z > building.minBounds.z - safetyMargin &&
        futurePosition.z < building.maxBounds.z + safetyMargin) {

        // Compute an avoidance force, could be towards any direction that's away from the building
        glm::vec3 toCenter = building.startingPoint - position;
        glm::vec3 awayFromBuilding = glm::normalize(position - building.startingPoint);
        avoidanceVec = awayFromBuilding * maxSpeed * 2.0f; // Boost the avoidance force
    }
    return avoidanceVec;
}
