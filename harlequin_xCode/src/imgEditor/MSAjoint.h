/********  Test sample for ofxInteractiveObject									********/
/********  Make sure you open your console to see all the events being output	********/


#pragma once

#include "ofxMSAInteractiveObject.h"

#define		IDLE_COLOR		0xFFFFFF
#define		OVER_COLOR		0x00FF00
#define		DOWN_COLOR		0xFF0000


class MSAjoint : public ofxMSAInteractiveObject {
public:
    
    bool draggable, dragging;
    int pMouseX, pMouseY;
    int colorIdle, colorOver, colorDown;
    float z;
    
    MSAjoint() {
        setColors(IDLE_COLOR, OVER_COLOR, DOWN_COLOR);
        dragging = false;
        draggable = false;
        pMouseX = ofGetMouseX();
        pMouseY = ofGetMouseY();
        z = 0;
    }
    
    ~MSAjoint() {
        // do nothing for now
    }

    // core functions
    void setup() {
        printf("MSAjoint::setup() - hello!\n");
        enableMouseEvents();
        enableKeyEvents();
    }
    
	void exit() {
		printf("MSAjoint::exit() - goodbye!\n");
	}
	
	void update() {
		//		x = ofGetWidth()/2 + cos(ofGetElapsedTimef() * 0.2) * ofGetWidth()/4;
		//		y = ofGetHeight()/2 + sin(ofGetElapsedTimef() * 0.2) * ofGetHeight()/4;
	}
	
	void draw() {
		if(isMousePressed()) ofSetHexColor(colorDown);
		else if(isMouseOver()) ofSetHexColor(colorOver);
		else ofSetHexColor(colorIdle);
		
        ofRect(x, y, width, height);
    }
    
    void setColors(int colorIdle_, int colorOver_, int colorDown_) {
        colorIdle = colorIdle_;
        colorOver = colorOver_;
        colorDown = colorDown_;
    }
    
    void setPosition3D(float _x, float _y, float _z) {
        z = _z;
        this->setPosition(_x, _y);
    }
    
    void setDraggable(bool draggable_) {
        draggable = draggable_;
    }

	virtual void onRollOver(int x, int y) {
		printf("MSAjoint::onRollOver(x: %i, y: %i)\n", x, y);
	}
	
	virtual void onRollOut() {
		printf("MSAjoint::onRollOut()\n");
	}
	
	virtual void onMouseMove(int x, int y){
//		printf("MSAjoint::onMouseMove(x: %i, y: %i)\n", x, y);
	}
    
    void updatePosition(int x, int y) {
        // TODO: extend this class to include a Z coordinate >> to be set relative to MSAlimb
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
//        printf("MSAjoint::mouseDragged(x: %i, y: %i, button: %i)\n", x, y, button);
        if (dragging) updatePosition(x, y);
    }
    
	virtual void onDragOver(int x, int y, int button) {
//		printf("MSAjoint::onDragOver(x: %i, y: %i, button: %i)\n", x, y, button);
	}
    
	virtual void onDragOutside(int x, int y, int button) {
//		printf("MSAjoint::onDragOutside(x: %i, y: %i, button: %i)\n", x, y, button);
	}
	
	virtual void onPress(int x, int y, int button) {
		printf("MSAjoint::onPress(x: %i, y: %i, button: %i)\n", x, y, button);
        if (draggable) {
            dragging = true;
            pMouseX = x;
            pMouseY = y;
        }
	}
	
	virtual void onRelease(int x, int y, int button) {
		printf("MSAjoint::onRelease(x: %i, y: %i, button: %i)\n", x, y, button);
        if (dragging) dragging = false;
	}
	
	virtual void onReleaseOutside(int x, int y, int button) {
		printf("MSAjoint::onReleaseOutside(x: %i, y: %i, button: %i)\n", x, y, button);
        if (dragging) dragging = false;
	}
	
	virtual void keyPressed(int key) {
		printf("MSAjoint::keyPressed(key: %i)\n", key);
	}
	
	virtual void keyReleased(int key) {
		printf("MSAjoint::keyReleased(key: %i)\n", key);
	}
	
};
