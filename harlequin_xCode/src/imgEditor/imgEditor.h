//
//  imgEditor.h
//  harlequin
//
//  Created by david allen on 4/15/15.
//
//

#pragma once

//#ifndef harlequin_imgEditor_h
//#define harlequin_imgEditor_h

#include "ofxUI.h"
#include "imgData.h"
#include "MSAjoint.h"
//#include "ofDirUtils.h"

class imgEditor {
    
public:
    
    // core functions
    void setup(string guiSettingsPath_, string imagesDirectory_, string imageJointDataDirectory_);
    void exit();
    void enable();
    void disable();
    void setMSAJntVisibility(bool visible_);
    void setMSAJntVisibility(vector< MSAjoint* > joints_, bool visible_);
    void update(vector< vector<ofPoint> > trackedUserJoints_);
    void draw(bool drawMirrored_);
    
    // specialized functions
    void mapAllImages();
    vector<string> listFilesOfType(ofDirectory dir, string ext, vector<string> fileList = vector<string>()); // TODO: move this to a "dir helper" or something"
    void addImgToPathMap(string baseName, string path);
    void reiterateIt();
    void loadImgData();
    void setupGui();
    void guiEvent(ofxUIEventArgs &e);
    void upatedTrnDataVisibilty();
    
    ////////////////
    // properties //
    ////////////////
    bool                            initializing, enabled;
    ofxUISuperCanvas*               gui; // gui = new ofxUISuperCanvas("harlequin");
    string                          guiSettingsPath, imagesDirectory, imageJointDataDirectory;
    int                             guiJntDataTglMtxSize;
    ofxUIToggleMatrix               *guiJntDataTglMtx;
    vector<ofxUIToggle*>            guiJntDataTglMtxTgls;
    //
    // joint tracking
    int                             trackedUserIndex, trackedUsersCount, trackedUserJointsSize;
    vector<MSAjoint*>               joints;
    ofVec2f                         jointsAnchorInPercentages;
    float                           jointsScale;
    int                             jointSetsCount, jointsCount;
    //
    // images
    map <string, vector<string> >   imagePathMap; // <string imgFileName, vector<string> imgPaths>
    map <string, vector<string> >::iterator it; // iterator storage for paths to image that is currently being edited
    int                             currentImgIndex;
    float                           currentImgIndexFloat;
    string                          currentImgBaseName;
    imgData                         *imgDataObj;
    
private:
    
    // internal data
    
};

//#endif
