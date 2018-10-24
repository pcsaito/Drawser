#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "ofxLaserDacEtherdream.h"
#include "ofxGui.h"
#include "ofxTablet.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
    void keyReleased(int key);
	void keyPressed  (int key);
    void tabletMoved(TabletData &data);
    void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
    
    void drawStarted();
    void drawStoped();
    void drawDragged(int x, int y);

	
	void showLaserEffect(int effectnum);
	
	ofParameter<int> currentLaserEffect;
	int numLaserEffects; 
	
	ofxLaser::Manager laser;
    ofxLaser::DacEtherdream dac;
	bool drawingShape = false;
	int laserWidth;
	int laserHeight;

    int cursorX;
    int cursorY;
    bool showCursor;
    int cursorType;
    
    ofImage colorPicker;
    bool colorPicking;

	std::vector<tuple<ofPolyline,ofColor>> polyLines;
		
	float elapsedTime; 

    ofxPanel cgui;
    ofParameter<ofColor> color;
};

