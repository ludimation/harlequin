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
#include "imgData.h"
//#include "ofDirUtils.h"

class GRTEditor {

public:
    
    // core fuctions
    void setup(string guiSettingsPath_, string imgsDir_, string imageJointDataDir_, string imageTagDataDir_, string imageGRTDataDir_);
    void update();
    void draw();
    void keyPressed(int key);
    
    // specialized functions
    void guiEvent(ofxUIEventArgs &e);
    // file handling functions
    void mapAllImageData();
    vector<string> listFilesOfType(ofDirectory dir_ = ofDirectory(), string ext_ = "", vector<string> fileList_ = vector<string>()); // TODO: move this to a "dir helper" or something"
    bool loadData();
    bool saveData();
    bool loadModel();
    bool trainModel();
    bool saveModel();

    // properties
    ofxUISuperCanvas* gui;
    bool loadImagesNow;
    
    
private:
    // internal data

};


//#endif /* defined(__harlequin__GRTEditor__) */
