//
//  GRTEditor.h
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

//#ifndef __harlequin__GRTEditor__
//#define __harlequin__GRTEditor__
#pragma once

#include "ofApp.h"
#include "ofxUI.h"

class GRTManager {

public:
    
    // core fuctions
    void setup();
    void update();
    void draw();
    
    // gui
    void guiEvent(ofxUIEventArgs &e);
    // properties
    ofxUISuperCanvas* gui;
    bool loadImagesNow;
    
    // file handling functions
    bool loadData();
    bool saveData();
    bool loadModel();
    bool trainModel();
    bool saveModel();
    
private:
    // internal data

};


//#endif /* defined(__harlequin__GRTEditor__) */
