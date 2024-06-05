#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "predator.h"
#include "pray.h"
#include "building.h"
#include "ofLight.h"


class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
    
    //general
    ofEasyCam easyCam;
    ofxPanel gui;
    ofLight light;
    ofShader shader;
    float camDistance;
    
    //-----PLANE AND BOUNDARY STUFF----
    ofPlanePrimitive plane;
    /// method to add boid in any random point within the plane.
    //---------------------------------
    
    //pray stuff
    std::vector<flocker> birds;
    int numBoids = 200;
    float birdscale = 1.0, speed = 20.0;
    //------TEXTURE-----
    ofTexture BlueTex;
    ofTexture CyanTex;
    ofTexture CharcoalTex;
    vector<ofTexture> textures;
    ofTexture roboTex;
    //-------------------
    
    //predator stuff
    roboBoid robot;
    //sliders and modes
    ofxFloatSlider flockAcceleration;
    ofxToggle followMode;
    ofxToggle ManualchaseMode;
    ofxToggle AutoChaseMode;
    std::unordered_map<int, bool> keyMap;
    //CHASE MECHANISM----
    glm::vec3 calculateFlockCentroid();
    //---------
    building_class building;
    
    // utility and QOL features
    void resetSimulation();
    bool isSimulationPaused;
    string resetMessage;
    bool showResetMessage;
    float messageTimer;
};
