#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "ofxLaserDacEtherdream.h"
#include "ofxLaserGraphic.h"
#include "ofxGui.h"
#include "ofxSvg.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
    void keyReleased(int key);
	void keyPressed  (int key);
    void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
    
    void drawStarted();
    void drawStoped();
    void drawDragged(int x, int y);
    void pickerMoved(int x, int y);
    
    void saveSVG();
    void loadSVGs();
    
    void loadColorMasks();
	
	void drawLaser(int svgNum);
	
    ofParameter<int> currentSVG;

    ofParameter<int> currentLaserEffect;
	
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

	vector<tuple<ofPolyline,ofColor>> polyLines;
    
    vector<ofxLaser::Graphic> laserGraphics;
    vector<string> fileNames;

	float elapsedTime; 

    ofxPanel cgui;
    ofParameter<ofColor> color;
    
    ofImage colorMask;
    vector<string> colorMaskNames;
    ofParameter<int> currentColorMask;
    ofColor getMaskedColor(int x, int y);
};

