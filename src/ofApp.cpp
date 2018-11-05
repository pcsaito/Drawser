#include "ofApp.h"
#include "ofxPolylinesToSVG.h"
#include <time.h>

#include <ctime>


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
    
    currentLaserEffect = -1;
	 
    cgui.setup("color panel", "colors.xml", ofGetWidth()-240, 700 );
    cgui.add(color.set("color", ofColor(0, 255, 0), ofColor(0), ofColor(255)));
    cgui.getGroup("color").maximize();
    
    colorPicker.load("fullColorPicker.png");
    
    currentColorMask = 0;
    colorMask.load("mandala.png");
    colorMask.resize(800,800);
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
    if (fileNames.size()>0) ofDrawBitmapString("Current SVG: "+ofToString(currentSVG) + " "+fileNames[currentSVG], 400, ypos+=30);
    if (colorMaskNames.size()>0) ofDrawBitmapString("Current Color Mask: "+colorMaskNames[currentColorMask], 400, ypos+=30);
	ofDrawBitmapString("Left and Right Arrows to change saved frames", 400, ypos+=30);
    ofDrawBitmapString("Mouse to draw polylines, 'Z' to undo and 'C' to clear", 400, ypos+=30);
    ofDrawBitmapString("'P' or right click to pick color, 'S' to save the frame", 400, ypos+=30);

    drawLaser(currentSVG);

    // sends points to the DAC
    laser.send();

    laser.drawUI();
    
    cgui.draw();
}


void ofApp::drawLaser(int svgNum) {
    // LASER SVGS
    if(laserGraphics.size()>currentSVG) {
        vector<ofColor> colors = laserGraphics[currentSVG].colours;
        vector<ofPolyline> polys = laserGraphics[currentSVG].polylines;
        
        for(size_t i = 0; i<polys.size(); i++) {
            ofPolyline poly  = polys[i];
            
            if (currentColorMask == 0) {
                ofPushMatrix();
                ofTranslate(400, 400);
                
                laser.drawPoly(poly,colors[i]);
                ofPopMatrix();

            } else {
                colors.clear();
                vector<ofDefaultVertexType> points = poly.getVertices();
                
                for(size_t j = 0; j<points.size(); j++) {
                    ofDefaultVertexType point = points[j];
                    ofColor pointcolor = getMaskedColor(point.x, point.y);
                    colors.push_back(pointcolor);
                }
                
                laser.drawPoly(poly, colors);
            }
        }
    }
    
	// LASER POLYLINES
	for(size_t i = 0; i<polyLines.size(); i++) {
        if (currentColorMask == 0) {
            laser.drawPoly(get<0>(polyLines[i]), get<1>(polyLines[i]));
        } else {
            vector<ofColor> colors;
            vector<ofDefaultVertexType> points = get<0>(polyLines[i]).getVertices();
            
            for(size_t j = 0; j<points.size(); j++) {
                ofDefaultVertexType point = points[j];
                ofColor pointcolor = getMaskedColor(point.x, point.y);
                colors.push_back(pointcolor);
            }
            
            laser.drawPoly(get<0>(polyLines[i]), colors);

        }
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
    
    
    
    float left = laserWidth*0.1;
    float top = laserHeight*0.1;
    float right = laserWidth*0.9;
    float bottom = laserHeight*0.9;
    float width = laserWidth*0.8;
    float height = laserHeight*0.8;

    switch (currentLaserEffect) {
        case -1: { break; }
        case 1: {
            ofPath ellipse = ofPath();
            ellipse.setFilled(false);
            ellipse.setStrokeWidth(3);
            ellipse.setCircleResolution(100);
            
            float localTime = elapsedTime;
            float N = 12;
            for (int i = 0; i < N; i++) {
                ellipse.clear();
                float w = (75+(i*40))*cos((localTime+((float)cursorY/360.0*i)));
                float h = (75+(i*40))*cos(((float)cursorX/360.0)*i);
                ellipse.moveTo(0+(w/2),0);
                ellipse.ellipse(0,0,w,h);
                //        ellipse.rotateDeg(360.0*sin(elapsedTime), ofPoint(0,0,1));
                //        ellipse.draw(0,0);
                
                ofColor frameColor = ofColor::fromHsb(230.0/N*i, 255, 255);
                
                ellipse.translate(ofPoint(400,400,0));
                vector<ofPolyline> polys = ellipse.getOutline();
                for(size_t i = 0; i<polys.size(); i++) {
                    if (currentColorMask == 0) {
                        laser.drawPoly(polys[i], frameColor);
                    } else {
                        vector<ofColor> colors;
                        vector<ofDefaultVertexType> points = polys[i].getVertices();
                        
                        for(size_t j = 0; j<points.size(); j++) {
                            ofDefaultVertexType point = points[j];
                            ofColor pointcolor = getMaskedColor(point.x, point.y);
                            colors.push_back(pointcolor);
                        }
                        
                        laser.drawPoly(polys[i], colors);
                    }
                }
            }
            break;
        }
        case 2: {
            // LASER PARTICLES ANIMATING
            
            float speed = 0.2;
            for(int i = 0; i<10; i++) {
                
                ofPoint p;
                float spread = ofMap(cos(elapsedTime*0.4),1,-1,0.01,0.1);
                p.x = sin((elapsedTime-((float)i*spread)) *1.83f * speed) * 300;
                p.y = sin((elapsedTime-((float)i*spread)) *2.71f *speed) * 300;
                p.x+=laserWidth/2;
                p.y+=laserHeight/2;
                
                ofColor c;
                if (currentColorMask == 0) {
                    c.setHsb(i*10,255,255);
                } else {
                    c = getMaskedColor(p.x, p.y);
                }
                
                laser.drawDot(p, c);
                
            }
            
            break;
        }
            
        case 3: {
            
            // LASER LINES ANIMATING
            int numlines = cursorY / 50;
            
            ofPath line = ofPath();
            line.setFilled(false);
            line.setStrokeWidth(3);
            line.setCurveResolution(100);
            
            float localTime = elapsedTime;
            int delta = width/numlines;

            for(int i = 0; i<numlines; i++) {
                line.clear();

                float progress =(float)i/(float)(numlines);
                
                float xpos = elapsedTime*cursorX + i*delta;
                while (xpos > laserWidth+delta) xpos = xpos - delta*numlines;
                xpos = xpos - 1.1*delta;
                
                line.moveTo(xpos,top);
                line.curveTo(xpos,top);
                line.curveTo(xpos,top);

                line.curveTo(xpos,top+height);
                line.curveTo(xpos,top+height);
                
                ofColor frameColor = ofColor::fromHsb(progress*255, 255, 255);
                
                vector<ofPolyline> polys = line.getOutline();
                for(size_t i = 0; i<polys.size(); i++) {
                    if (currentColorMask == 0) {
                        laser.drawPoly(polys[i], frameColor);
                    } else {
                        vector<ofColor> colors;
                        vector<ofDefaultVertexType> points = polys[i].getVertices();
                        
                        for(size_t j = 0; j<points.size(); j++) {
                            ofDefaultVertexType point = points[j];
                            
                            ofColor pointcolor = getMaskedColor(point.x, point.y);
                            colors.push_back(pointcolor);
                        }
                        
                        laser.drawPoly(polys[i], colors);
                    }
                }
            }
            
            break;
            
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
            
        case '-':
            loadColorMasks();
            currentColorMask--;
            if(currentColorMask<0) currentColorMask = colorMaskNames.size()-1;
            colorMask.load(colorMaskNames[currentColorMask]);
            colorMask.resize(laserWidth,laserHeight);

            break;
            
        case '=':
            loadColorMasks();
            currentColorMask++;
            if(currentColorMask>=colorMaskNames.size()) currentColorMask = 0;
            colorMask.load(colorMaskNames[currentColorMask]);
            colorMask.resize(laserWidth,laserHeight);

            break;
            
        case '`':
            currentLaserEffect = -1;
            break;
            
        case '1':
            currentLaserEffect = 1;
            break;
            
        case '2':
            currentLaserEffect = 2;
            break;
            
        case '3':
            currentLaserEffect = 3;
            break;
            
        case '4':
            currentLaserEffect = 4;
            break;
            
        case '5':
            currentLaserEffect = 5;
            break;
            
        case '6':
            currentLaserEffect = 6;
            break;
            
        case '7':
            currentLaserEffect = 7;
            break;
            
        case '8':
            currentLaserEffect = 8;
            break;
            
        case '9':
            currentLaserEffect = 9;
            break;
            
        case '0':
            currentLaserEffect = 0;
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
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,sizeof(buffer),"%d%m%Y-%H%M%S",timeinfo);
    string path = string("svgs/") + string(buffer) + string(".svg");
    ofLog(OF_LOG_NOTICE , "Save path: " + path);
    
    ofxPolylinesToSVG svg = ofxPolylinesToSVG();
    
    for(size_t i = 0; i<polyLines.size(); i++) {
        ofPolyline &poly = get<0>(polyLines[i]);
        ofColor color = get<1>(polyLines[i]);

        svg.addPolyline(poly, color);
    }
    
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

void ofApp::loadColorMasks(){
    // get the filenames of all the svgs in the data/svgs folder
    string path = "colorMasks/";
    ofDirectory dir(path);
    dir.allowExt("png");
    dir.listDir();
    dir.sort();
    
    // and load them all
    const vector<ofFile>& files = dir.getFiles();
    //laserGraphics.resize(files.size());
    //fileNames.resize(files.size());
    
    colorMaskNames.clear();
    colorMaskNames.push_back(" ");
    
    for(int i = 0; i<files.size();i++) {
        const ofFile & file = files.at(i);
        ofLog(OF_LOG_NOTICE,file.path());
        colorMaskNames.push_back(file.path());
    }
}

ofColor ofApp::getMaskedColor(int x, int y) {
    return colorMask.getColor(ofClamp(x, 0, laserWidth-1), ofClamp(y, 0, laserHeight-1));
}


//--------------------------------------------------------------
void ofApp::exit(){
    laser.saveSettings();
    dac.close();
}
