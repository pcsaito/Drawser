#include "ofApp.h"
#include "ofxPolylinesToSVG.h"
#include <time.h>

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetEscapeQuitsApp(false);

    loadSVGs();
    
	laserWidth = 800;
	laserHeight = 800;
	laser.setup(laserWidth, laserHeight);
    
    cursorX = laserWidth / 2;
    cursorY = laserHeight / 2;
    showCursor = true;
    cursorType = 0;
    colorPicking = false;
    
    laser.addProjector(dac);
    
    // replace this with the IP address of your etherdream (find it with the sitter diagnostic
    // tool at https://ether-dream.com/downloads.html ) 
    string dacIP = "192.168.0.2";

    dac.setup(dacIP);
	
    laser.initGui();
    currentSVG = 0;
    numLaserEffects = 4;
	 
    cgui.setup("color panel", "colors.xml", ofGetWidth()-240, 700 );
    cgui.add(color.set("color", ofColor(0, 255, 0), ofColor(0), ofColor(255)));
    cgui.getGroup("color").maximize();
    
    colorPicker.load("fullColorPicker.png");
}

//--------------------------------------------------------------
void ofApp::update(){
    
	float deltaTime = ofClamp(ofGetLastFrameTime(), 0, 0.2);
	elapsedTime+=deltaTime;
    
    // prepares laser manager to receive new points
    laser.update();
	
}


void ofApp::draw() {
    ofBackground(0);

    if (colorPicking) {
        ofSetColor(255,255,255,127);
        colorPicker.draw(10, 10);
    }

	ofNoFill();
	ofSetLineWidth(1);
	ofDrawRectangle(10,10,laserWidth, laserHeight);
	
	int ypos = laserHeight+20;
    ofDrawBitmapString("Current FPS : "+ofToString(laser.getProjectorFrameRate(0)), 400, ypos+=30);
    if (fileNames.size()>0) ofDrawBitmapString("Current SVG : "+ofToString(currentSVG) + " "+fileNames[currentSVG], 400, ypos+=30);
    ofDrawBitmapString("TAB to change view, F to toggle full screen", 400, ypos+=30);
	ofDrawBitmapString("Left and Right Arrows to change current effect", 400, ypos+=30);
    ofDrawBitmapString("Mouse to draw polylines, 'C' to clear", 400, ypos+=30);
    
    showLaserEffect(currentSVG);

    // sends points to the DAC
    laser.send();

    laser.drawUI();
    
    cgui.draw();
}


void ofApp :: showLaserEffect(int effectnum) {
    
    
	float left = laserWidth*0.1;
	float top = laserHeight*0.1;
	float right = laserWidth*0.9;
	float bottom = laserHeight*0.9;
	float width = laserWidth*0.8;
	float height = laserHeight*0.8;
	
//    switch (currentSVG) {
//        case 1: {
//
//            // LASER PARTICLES
//            int numParticles = 20;
//
//            for(int i = 0; i<numParticles; i++) {
//
//                float progress =(float)i/(float)(numParticles-1);
//
//                float xpos =left + (width*progress) ;
//
//                laser.drawDot(ofPoint(xpos, top+height*0.3), ofColor(255));
//                ofColor c;
//                c.setHsb(progress*255, 255, 255);
//                laser.drawDot(ofPoint(xpos, top+height*0.7), c);
//
//            }
//
//            break;
//
//        }
//        case 2: {
//
//            // LASER PARTICLES ANIMATING
//
//            float speed = 0.5;
//            for(int i = 0; i<15; i++) {
//
//                ofColor c;
//                c.setHsb(i*5.5,255,255);
//                ofPoint p;
//                float spread = ofMap(cos(elapsedTime*0.4),1,-1,0.01,0.1);
//                p.x = sin((elapsedTime-((float)i*2*spread)) *1.83f * speed) * 300;
//                p.y = sin((elapsedTime-((float)i*2*spread)) *2.71f *speed) * 300;
//                p.x+=laserWidth/2;
//                p.y+=laserHeight/2;
//
//                laser.drawDot(p, c);
//
//            }
//
//            break;
//
//        }
//
//        case 3: {
//
//            // LASER PARTICLES ANIMATING
//
//            float speed = 0.5;
//            for(int i = 0; i<3; i++) {
//
//                ofColor c;
//                c.setHsb(i*20,255,255);
//                ofPoint p;
//                float spread = ofMap(cos(elapsedTime*0.4),1,-1,0.01,0.1);
//                p.x = sin((elapsedTime-((float)i*2*spread)) *1.83f * speed) * 300;
//                p.y = sin((elapsedTime-((float)i*2*spread)) *2.71f *speed) * 300;
//                p.x+=laserWidth/2;
//                p.y+=laserHeight/2;
//
//                laser.drawDot(p, c);
//
//            }
//
//            break;
//
//        }
//    }

    // LASER SVGS
    ofPushMatrix();
    ofTranslate(400, 400);
    if(laserGraphics.size()>currentSVG) {
        laserGraphics[currentSVG].renderToLaser(laser, 1, OFXLASER_PROFILE_DEFAULT);
    }
    ofPopMatrix();
    
	// LASER POLYLINES
	for(size_t i = 0; i<polyLines.size(); i++) {
		laser.drawPoly(get<0>(polyLines[i]), get<1>(polyLines[i]));
	}
    
    //Cursor
    //laser.drawDot(ofPoint(cursorX, cursorY), ofColor(255, 0, 0));
    if (showCursor) {
        int size = 20;
        if (colorPicking) {
            laser.drawCircle(ofPoint(cursorX, cursorY), 20, color);
        } else {
            laser.drawDot(ofPoint(cursorX, cursorY), color);
        }
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
        case 'p':
            colorPicking = false;
            break;
    }

}

void ofApp::keyPressed(int key){
    
    switch (key) {
        case 'z':
            if (!drawingShape) {
                if(!polyLines.empty()) polyLines.pop_back();
            }
            break;

        case 'c':
            polyLines.clear();
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'p':
            if (drawingShape) drawStoped();
            colorPicking = true;
            break;
            
        case 's':
            saveSVG();
            break;
            
        case ' ':
            laser.getProjector().armed.set(!laser.getProjector().armed.get());
            break;

        
        case OF_KEY_TAB:
            laser.nextProjector();
            break;

        case OF_KEY_LEFT:
            currentSVG--;
            if(currentSVG<0) currentSVG = laserGraphics.size()-1;
            break;

        case OF_KEY_RIGHT:
            currentSVG++;
            if(currentSVG>=laserGraphics.size()) currentSVG = 0;
            break;
            
        case OF_KEY_ESC:
            break;

    }
	
}

void ofApp::mouseMoved(int x, int y){
    cursorX = x;
    cursorY = y;
    
    if (colorPicking ) {
        color = colorPicker.getColor(cursorX, cursorY);
    }
}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (!colorPicking && !drawingShape && button == 0) drawStarted();
    drawDragged(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (button == 2) {
        if (drawingShape) drawStoped();
        colorPicking = true;
        return;
    }
   
    colorPicking = false;
    if (!drawingShape) drawStarted();
}

void ofApp::mouseReleased(int x, int y, int button) {
    if (button == 2) {
        colorPicking = false;
        return;
    }
   
    drawStoped();
}

void ofApp::drawStarted(){
    polyLines.push_back(make_tuple(ofPolyline(),color));
    drawingShape = true;
}

void ofApp::drawStoped(){
    if(drawingShape) {
        if (!polyLines.empty()) {
            ofPolyline &poly = get<0>(polyLines.back());
            //poly = poly.getSmoothed(2);
        }
        drawingShape = false;
    }
}

void ofApp::drawDragged(int x, int y){
    cursorX = x;
    cursorY = y;
    
    if (colorPicking) {
        color.set("color", colorPicker.getColor(cursorX, cursorY));
          
    } else if (drawingShape) {
        if (!polyLines.empty()) {
            ofPolyline &poly = get<0>(polyLines.back());
            //if(poly.getofVec3f end =poly.getVertices().back();
            //if(ofDist(x, y, end.x, end.y) > 5) {
            //poly.simplify();
            poly.addVertex(x, y);
            //}
        }
    }
}

void ofApp::saveSVG(){
    ofxPolylinesToSVG svg = ofxPolylinesToSVG();
    
    for(size_t i = 0; i<polyLines.size(); i++) {
        ofPolyline &poly = get<0>(polyLines[i]);
        ofColor color = get<1>(polyLines[i]);

        svg.addPolyline(poly, color);
    }
    
    time_t _tm =time(NULL );
    struct tm * curtime = localtime ( &_tm );
    string path = string("svgs/") + asctime(curtime) + string(".svg");
    svg.saveToFile(path);
    
    loadSVGs();
}

void ofApp::loadSVGs(){
    // get the filenames of all the svgs in the data/svgs folder
    string path = "svgs/";
    ofDirectory dir(path);
    dir.allowExt("svg");
    dir.listDir();
    dir.sort();

    // and load them all
    const vector<ofFile>& files = dir.getFiles();
    //laserGraphics.resize(files.size());
    //fileNames.resize(files.size());
    
    fileNames.clear();
    fileNames.push_back("  ");
    laserGraphics.clear();
    laserGraphics.push_back(ofxLaser::Graphic());

    for(int i = 0; i<files.size();i++) {
        const ofFile & file = files.at(i);
        ofxSVG svg;
        svg.load(file.getAbsolutePath());
        laserGraphics.push_back(ofxLaser::Graphic());
        laserGraphics.back().addSvg(svg);
        laserGraphics.back().connectLineSegments();
//        laserGraphics.back().autoCentre();
        
        ofLog(OF_LOG_NOTICE,file.getAbsolutePath());
        fileNames.push_back(file.getFileName());
    }
}



//--------------------------------------------------------------
void ofApp::exit(){
    laser.saveSettings();
    dac.close();
}
