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
#include "imgGRTData.h"
//#include "ofDirUtils.h"

class GRTEditor {

public:
    
    // core fuctions
    void setup(string guiSettingsPath_, string imgsDir_, string imgJntDataDir_, string imgTagDataDir_, string imgGRTDataDir_);
    void exit();
    void enable();
    void disable();
    void update();
    void draw();
    void keyPressed(int key);
    
    // specialized functions
    void guiEvent(ofxUIEventArgs &e);
    // file handling functions
    void mapAllGRTData(string imageJntDataDir_, string imgsDir_);
    vector<string> listFilesOfType(ofDirectory dir_ = ofDirectory(), string ext_ = "", vector<string> fileList_ = vector<string>()); // TODO: move this to a "dir helper" or something"
    bool loadData();
    bool saveData();
    bool loadModel();
    bool trainModel();
    bool saveModel();

    // properties
    bool                            initializing;
    string                          guiSettingsPath, imgsDirPath, imgJntDataDirPath, imgTagDataDirPath, imgGRTDataDirPath;
    map< string, imgData* >         imgPathDataMap; // <imgPath, imgDataObject pointer>
    int                             currentGRTObjIndx;
    map< string, vector< string > > tagImgPathMap; // map< tagName, map<on/off, vector< imgPaths > > >
    imgGRTData                      *imgGRTDataObj;
    ofxUISuperCanvas                *gui;
    bool                            loadImagesNow;
    
private:
    // internal data

};


//#endif /* defined(__harlequin__GRTEditor__) */
