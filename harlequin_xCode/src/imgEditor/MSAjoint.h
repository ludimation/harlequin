/********  Test sample for ofxInteractiveObject									********/
/********  Make sure you open your console to see all the events being output	********/


#pragma once

#include "ofxMSAInteractiveObject.h"

#define		IDLE_COLOR		0xFFFFFF
#define		OVER_COLOR		0x00FF00
#define		DOWN_COLOR		0xFF0000


class MSAjoint : public ofxMSAInteractiveObject {
public:
    
    bool dragging;
    int pMouseX, pMouseY;
    
	void setup() {
		printf("MyTestObject::setup() - hello!\n");
		enableMouseEvents();
		enableKeyEvents();
	}
	
	
	void exit() {
		printf("MyTestObject::exit() - goodbye!\n");
	}
	
	
	void update() {
		//		x = ofGetWidth()/2 + cos(ofGetElapsedTimef() * 0.2) * ofGetWidth()/4;
		//		y = ofGetHeight()/2 + sin(ofGetElapsedTimef() * 0.2) * ofGetHeight()/4;
	}
	
	
	void draw() {
		if(isMousePressed()) ofSetHexColor(DOWN_COLOR);
		else if(isMouseOver()) ofSetHexColor(OVER_COLOR);
		else ofSetHexColor(IDLE_COLOR);
		
        ofRect(x, y, width, height);
    }
    
	virtual void onRollOver(int x, int y) {
		printf("MyTestObject::onRollOver(x: %i, y: %i)\n", x, y);
	}
	
	virtual void onRollOut() {
		printf("MyTestObject::onRollOut()\n");
	}
	
	virtual void onMouseMove(int x, int y){
		printf("MyTestObject::onMouseMove(x: %i, y: %i)\n", x, y);
	}
    
    void updatePosition(int x, int y) {
        this->x += x - pMouseX;
        this->y += y - pMouseY;
        
        // TODO: make sure objects cannnot get dragged off the edge of the screen
        //        this->x -= (float)((int)this->x % ofGetWidth());
        //        this->y -= (float)((int)this->y % ofGetHeight());
        //        this->x = fmod(this->x, ofGetWidth());
        //        this->y = fmod(this->y, ofGetHeight());
        //
        //        if this-> >= ofGetWidth();
        
        pMouseX = x;
        pMouseY = y;
    }
    
    void mouseDragged(int x, int y, int button) {
        printf("MyTestObject::mouseDragged(x: %i, y: %i, button: %i)\n", x, y, button);
        if (dragging) updatePosition(x, y);
    }
    
	virtual void onDragOver(int x, int y, int button) {
		printf("MyTestObject::onDragOver(x: %i, y: %i, button: %i)\n", x, y, button);
	}
    
	virtual void onDragOutside(int x, int y, int button) {
		printf("MyTestObject::onDragOutside(x: %i, y: %i, button: %i)\n", x, y, button);
	}
	
	virtual void onPress(int x, int y, int button) {
		printf("MyTestObject::onPress(x: %i, y: %i, button: %i)\n", x, y, button);
        dragging = true;
        pMouseX = x;
        pMouseY = y;
	}
	
	virtual void onRelease(int x, int y, int button) {
		printf("MyTestObject::onRelease(x: %i, y: %i, button: %i)\n", x, y, button);
        if (dragging) dragging = false;
	}
	
	virtual void onReleaseOutside(int x, int y, int button) {
		printf("MyTestObject::onReleaseOutside(x: %i, y: %i, button: %i)\n", x, y, button);
        if (dragging) dragging = false;
	}
	
	virtual void keyPressed(int key) {
		printf("MyTestObject::keyPressed(key: %i)\n", key);
	}
	
	virtual void keyReleased(int key) {
		printf("MyTestObject::keyReleased(key: %i)\n", key);
	}
	
};
