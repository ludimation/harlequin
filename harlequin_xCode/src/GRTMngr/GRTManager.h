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

#include "ofxUI.h"
//#include "ofDirUtils.h"

class GRTManager {

public:
    
    // core fuctions
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    // specialized functions
    void guiEvent(ofxUIEventArgs &e);
    // file handling functions
    bool loadData();
    bool saveData();
    bool loadModel();
    bool trainModel();
    bool saveModel();
    vector<string> listFilesOfType(ofDirectory dir, string ext, vector<string> fileList = vector<string>()); // TODO: move this to a "dir helper" or something"

    // properties
    ofxUISuperCanvas* gui;
    bool loadImagesNow;
    
    
private:
    // internal data

};


//#endif /* defined(__harlequin__GRTEditor__) */
