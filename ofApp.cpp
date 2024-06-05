#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //-------ADJUSTING CAM-------
    camDistance = 500;
    easyCam.setPosition(0, 500, camDistance);
    easyCam.setNearClip(1);
    easyCam.setFarClip(10000);
    //---------------------------
    //GUI FOR SLIDERS
    gui.setup();
    gui.add(followMode.setup("Following", false));
    gui.add(ManualchaseMode.setup("Manual Chase", false));
    gui.add(AutoChaseMode.setup("Automatic Chase", false));
    gui.add(flockAcceleration.setup("Thrust", 10.0, 10.0, 25.0));
    
    //--------------
    //----LOAD TEXTURES--------
    ofDisableArbTex();
    ofLoadImage(BlueTex,"flock/birdTex.jpeg");
    BlueTex.generateMipmap();
    ofDisableArbTex();
    ofLoadImage(CyanTex, "flock/cyanTex.jpeg");
    CyanTex.generateMipmap();
    ofDisableArbTex();
    ofLoadImage(CharcoalTex, "flock/charcoalTex.jpeg");
    CharcoalTex.generateMipmap();
    textures.push_back(BlueTex);
    textures.push_back(CyanTex);
    textures.push_back(CharcoalTex);
    
    ofLoadImage(roboTex,"robot/roboTex.jpeg");
    roboTex.generateMipmap();
    //-------------------------
    //-------PLANE POSITION---------
    plane.set(10000, 10000, 10, 10); // A very large plane
    plane.setPosition(0, 0, 0); // Centered in the world space
    //-------------------------
    //----building setup------
    building.setup();
    //-------------------------
    for (int i = 0; i < numBoids; i++) {
        birds.push_back(flocker(ofRandom(-4800, 4800), ofRandom(200, 4800), ofRandom(-4800, 4800), speed));
    }
    
    isSimulationPaused = false; //pause sim when true
    resetMessage = "Boid simulation reset!";
    showResetMessage = false;
    messageTimer = 0.0f;
    
    ofDisableDepthTest();
}

void ofApp::resetSimulation() {
    // Clear the current list of boids
    birds.clear();
    
    numBoids = 200;  // Reset to the default value provided during setup
    followMode=false;
    ManualchaseMode=false;
    AutoChaseMode=false;
    flockAcceleration = 10.0;
    speed = 20.0;
    // Re-populate the list with new boids
    for (int i = 0; i < numBoids; ++i) {
        birds.push_back(flocker(ofRandom(-4800, 4800), ofRandom(200, 4800), ofRandom(-4800, 4800), speed));
    }
    isSimulationPaused = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!isSimulationPaused){
        if (followMode and !ManualchaseMode){
            glm::vec3 leader = robot.position;
            for (auto& bird : birds) {
                glm::vec3 followForce = bird.seek(leader);
                bird.applyForce(followForce * 1.0);
            }
        }
        if (ManualchaseMode and !followMode){
            glm::vec3 predatorPosition = robot.position; // Get the current position of the predator
            float dispersionThreshold = 100.0; // Set a threshold distance for dispersion

            for (auto& bird : birds) {
                // Apply dispersion based on predator's proximity
                bird.dispersion(predatorPosition, dispersionThreshold);
            }
        }
        
        // AUTOMATIC CHASE
        if (AutoChaseMode){
            glm::vec3 flockCenter = calculateFlockCentroid();
            glm::vec3 predatorPosition = robot.position;
            float dispersionThreshold = 100.0;
              // Implement a method in roboBoid to seek towards this point
            robot.seek(flockCenter, keyMap, OF_KEY_UP, flockAcceleration);
            for (auto& bird : birds) {
                // Apply dispersion based on predator's proximity
                bird.dispersion(predatorPosition, dispersionThreshold);
            }
        }
        
        for (auto& bird : birds) {
            bird.integrate(birds,speed);
        }
        if (followMode or ManualchaseMode){
            robot.integrate(keyMap, OF_KEY_UP, flockAcceleration);
        }
    }
}

glm::vec3 ofApp::calculateFlockCentroid() {
    glm::vec3 centroid = glm::vec3(0, 0, 0);
    for (auto& bird : birds) {
        centroid += bird.position;
    }
    centroid /= birds.size();
    return centroid;
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(showResetMessage) {
        float timePassed = ofGetElapsedTimef() - messageTimer;
        if(timePassed < 5.0f) { // Show message for 5 seconds
            ofSetColor(ofColor::white);
            ofDrawBitmapString(resetMessage, ofGetWidth() / 2 - resetMessage.length() * 4, ofGetHeight() - 20); // Adjust as needed
        } else {
            showResetMessage = false; // Stop showing message after 5 seconds
        }
    }
    
    // pause sim message
    if(isSimulationPaused){
            std::string pauseMessage = "Simulation Paused";
            ofDrawBitmapStringHighlight(pauseMessage, (ofGetWidth() / 2) - 100, ofGetHeight() / 2); // Centered on screen
    }
    easyCam.begin();
    ofEnableDepthTest();
    //------DRAW PLANE------
    shader.begin(); // Begin shader, if it's intended for the entire scene
    ofPushMatrix();
    ofRotateXDeg(-90); // Rotate to make it horizontal
    plane.enableColors();
    plane.draw(); // Or plane.drawWireframe() for a wireframe plane
    plane.disableColors();
    ofPopMatrix();
    //----------------------
    
    //--------DRAW PRAY BIRDS------
    for (int i = 0; i < birds.size(); i++) {
        ofPushMatrix();
        shader.begin();
        textures[birds[i].textureIdx].bind();
        birds[i].draw(birdscale);
        textures[birds[i].textureIdx].unbind();
        shader.end();
        ofPopMatrix();
    }
    //------------------------
    //----DRAW PREDATOR BIRD-------
    ofPushMatrix();
    roboTex.bind(); // Bind bird texture
    robot.draw(3.0); // Draw bird
    roboTex.unbind(); // Unbind bird texture
    ofPopMatrix();
    //-----------------------------
    
    //------DRAW BUILDING HERE----
    building.draw();
    //----------------------------
//    ofDisableDepthTest();
    easyCam.end();
    ofDisableDepthTest();
    gui.setPosition(10, 10);
    gui.draw();

    // Draw messages on the top right
    std::string instructions = "1. Press 'r' to reset position.\n2. Slide or check toggles to change modes.\n3. Press 'p' to pause sim";
        int padding = 20; // Space from the right edge of the window
        int lineHeight = 15; // Approximate height of a line of text
        int xOffset = ofGetWidth() - (8 * instructions.length() / 3) - padding; // Approximation of text width and padding
        int yOffset = 20; // Space from the top of the window

        ofDrawBitmapStringHighlight(instructions, xOffset, yOffset, ofColor::black, ofColor::white);
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    keyMap[key] = true;
    if (key == 'r') {
        resetSimulation();
        showResetMessage = true;
        messageTimer = ofGetElapsedTimef();
    }
    if(key == 'p'){
        isSimulationPaused = !isSimulationPaused; // Toggle the pause state
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    keyMap[key] = false;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
