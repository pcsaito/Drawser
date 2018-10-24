#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetEscapeQuitsApp(false);
    
	laserWidth = 800;
	laserHeight = 800;
	laser.setup(laserWidth, laserHeight);
    
    cursorX = laserWidth / 2;
    cursorY = laserHeight / 2;
    showCursor = false;
    cursorType = 0;
    colorPicking = false;
    
    laser.addProjector(dac);
    
    // replace this with the IP address of your etherdream (find it with the sitter diagnostic
    // tool at https://ether-dream.com/downloads.html ) 
    string dacIP = "192.168.1.100";

    dac.setup(dacIP);
	
    laser.initGui();
    currentLaserEffect = 0;
    numLaserEffects = 9;
	 
    cgui.setup("color panel", "colors.xml", ofGetWidth()-240, 700 );
    cgui.add(color.set("color", ofColor(0, 255, 0), ofColor(0), ofColor(255)));
    
    ofxTablet::start();
    ofAddListener(ofxTablet::tabletEvent, this, &ofApp::tabletMoved);
    
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
//    colorPicker.draw(0, 0);

	ofNoFill();
	ofSetLineWidth(1);
	ofDrawRectangle(0,0,laserWidth, laserHeight);
	
	int ypos = laserHeight+20;
    ofDrawBitmapString("Current FPS : "+ofToString(laser.getProjectorFrameRate(0)), 400, ypos+=30);
    ofDrawBitmapString("Current Effect : "+ofToString(currentLaserEffect), 400, ypos+=30);
    ofDrawBitmapString("TAB to change view, F to toggle full screen", 400, ypos+=30);
	ofDrawBitmapString("Left and Right Arrows to change current effect", 400, ypos+=30);
    ofDrawBitmapString("Mouse to draw polylines, 'C' to clear", 400, ypos+=30);
    
    showLaserEffect(currentLaserEffect);

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
	
	switch (currentLaserEffect) {
			
		case 1: {

			// LASER LINES
			int numlines = 10;
			
			for(int i = 0; i<numlines; i++) {
				
				float progress =(float)i/(float)(numlines-1);

				float xpos =left + (width*progress);
									
				laser.drawLine(ofPoint(xpos, top+height*0.1), ofPoint(xpos, top+height*0.4), ofColor(255));
              
				ofColor c;
				c.setHsb(progress*255, 255, 255);
                
				laser.drawLine(ofPoint(xpos, top+height*0.6), ofPoint(xpos, top+height*0.9), c);
		
			}

      break;

		}
		
			
		case 2: {
			
			// LASER LINES ANIMATING
			int numlines = 10;
			
			for(int i = 0; i<numlines; i++) {
				
				float progress =(float)i/(float)(numlines-1);
				
				float xpos =left + (width*progress) + (sin(elapsedTime*4+i*0.5)*width*0.05);
				
				laser.drawLine(ofPoint(xpos, top+height*0.1), ofPoint(xpos, top+height*0.4), ofColor(255));
				ofColor c;
				c.setHsb(progress*255, 255, 255);
				laser.drawLine(ofPoint(xpos, top+height*0.6), ofPoint(xpos, top+height*0.9), c);
				
			}
			
			break;
			
		}
		
			
		case 3: {
			
			// LASER CIRCLES
			int numCircles = 6;
			
			for(int i = 0; i<numCircles; i++) {
				
				float progress =(float)i/(float)(numCircles-1);
				
				float xpos =left + (width*progress);
				
				laser.drawCircle(ofPoint(xpos, top+height*0.3),30, ofColor(255));
				ofColor c;
				c.setHsb(progress*255, 255, 255);
				
				laser.drawCircle(ofPoint(xpos, top+height*0.7), 30, c);
				
			}
			
			break;
			
		}
			
		case 4: {
			
			// LASER CIRCLES ANIMATING
			int numCircles = 6;
			
			for(int i = 0; i<numCircles; i++) {
				
				float progress =(float)i/(float)(numCircles-1);
				
				float xpos =left + (width*progress) + (sin(elapsedTime*4+i*0.5)*width*0.05);
				
				laser.drawCircle(ofPoint(xpos, top+height*0.3), 30, ofColor::white);
				ofColor c;
				c.setHsb(progress*255, 255, 255);
				
				laser.drawCircle(ofPoint(xpos, top+height*0.7), 30, c);
				
			}
			
			break;
			
		}
			
		case 5: {
			
			// LASER PARTICLES
			int numParticles = 20;
			
			for(int i = 0; i<numParticles; i++) {
				
				float progress =(float)i/(float)(numParticles-1);
				
				float xpos =left + (width*progress) ;
				
				laser.drawDot(ofPoint(xpos, top+height*0.3), ofColor(255));
				ofColor c;
				c.setHsb(progress*255, 255, 255);
				laser.drawDot(ofPoint(xpos, top+height*0.7), c);
				
			}
			
			break;
			
		}
		case 6: {
			
			// LASER PARTICLES ANIMATING
			
			float speed = 0.5;
			for(int i = 0; i<15; i++) {
		
				ofColor c;
				c.setHsb(i*5.5,255,255);
				ofPoint p;
				float spread = ofMap(cos(elapsedTime*0.4),1,-1,0.01,0.1);
				p.x = sin((elapsedTime-((float)i*2*spread)) *1.83f * speed) * 300;
				p.y = sin((elapsedTime-((float)i*2*spread)) *2.71f *speed) * 300;
				p.x+=laserWidth/2;
				p.y+=laserHeight/2;
                
				laser.drawDot(p, c);
				
			}
			
			break;
			
		}
            
        case 7: {
            
            // LASER PARTICLES ANIMATING
            
            float speed = 0.5;
            for(int i = 0; i<3; i++) {
                
                ofColor c;
                c.setHsb(i*20,255,255);
                ofPoint p;
                float spread = ofMap(cos(elapsedTime*0.4),1,-1,0.01,0.1);
                p.x = sin((elapsedTime-((float)i*2*spread)) *1.83f * speed) * 300;
                p.y = sin((elapsedTime-((float)i*2*spread)) *2.71f *speed) * 300;
                p.x+=laserWidth/2;
                p.y+=laserHeight/2;
                
                laser.drawDot(p, c);
                
            }
            
            break;
            
        }
	}

	// LASER POLYLINES
	for(size_t i = 0; i<polyLines.size(); i++) {
		laser.drawPoly(get<0>(polyLines[i]), get<1>(polyLines[i]));
	}
    
    //Cursor
    //laser.drawDot(ofPoint(cursorX, cursorY), ofColor(255, 0, 0));
    if (showCursor) {
        int size = 20;
        if (cursorType == PTypeEraser || colorPicking) {
            laser.drawCircle(ofPoint(cursorX, cursorY), 20, color);
        } else if (cursorType == PTypePen) {
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
            if(polyLines.size()>0) polyLines.pop_back();
            break;

        case 'c':
            polyLines.clear();
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'p':
            colorPicking = true;
            break;
            
        case ' ':
            laser.getProjector().armed.set(!laser.getProjector().armed.get());
            break;

        
        case OF_KEY_TAB:
            laser.nextProjector();
            break;

        case OF_KEY_LEFT:
            currentLaserEffect--;
            if(currentLaserEffect<0) currentLaserEffect = numLaserEffects-1;
            break;

        case OF_KEY_RIGHT:
            currentLaserEffect++;
            if(currentLaserEffect>=numLaserEffects) currentLaserEffect = 0;
            break;
            
        case OF_KEY_ESC:
            break;

    }
	
}

void ofApp::tabletMoved(TabletData &data) {
    cursorX = data.abs_screen[0] * 1920;
    cursorY = (1.0 - data.abs_screen[1]) * 1200;
    showCursor = data.in_proximity;
    cursorType = data.pointerType;
    
    printf("pressure: %f \n", data.pressure);
    
    float threshold = 0.0;
    if (data.pressure > threshold && !drawingShape) {
        drawStarted();
    } else if (data.pressure <= threshold && drawingShape) {
        drawStoped();
    }

    if (drawingShape) {
        drawDragged(cursorX, cursorY);
    }
    
    if (data.pointerType == PTypeEraser || colorPicking == true) {
        color = colorPicker.getColor(cursorX, cursorY);
    }
}

void ofApp::mouseMoved(int x, int y){
//    cursorX = x;
//    cursorY = y;
}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
//    drawDragged(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
//    drawStarted();
}

void ofApp::mouseReleased(int x, int y, int button) {
//    drawStoped();
}

void ofApp::drawStarted(){
    polyLines.push_back(make_tuple(ofPolyline(),color));
    drawingShape = true;
}

void ofApp::drawStoped(){
    if(drawingShape) {
        ofPolyline &poly = get<0>(polyLines.back());
        poly = poly.getSmoothed(2);
        drawingShape = false;
    }
}

void ofApp::drawDragged(int x, int y){
    cursorX = x;
    cursorY = y;
    
    if(!drawingShape) return;
    
    ofPolyline &poly = get<0>(polyLines.back());
    //if(poly.getofVec3f end =poly.getVertices().back();
    //if(ofDist(x, y, end.x, end.y) > 5) {
    //poly.simplify();
    poly.addVertex(x, y);
    //}
}



//--------------------------------------------------------------
void ofApp::exit(){
    laser.saveSettings();
    dac.close();
}
