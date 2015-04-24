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
#include "MSAjoint.h"

class imgEditor {
    
public:
    
    // core functions
    void setup(string guiSettingsPath_, string imagesDirectory_);
    void exit();
    void update(vector< vector<ofPoint> > trackedUserJoints_);
    void draw(bool drawMirrored_);
    
    // specialized functions
    void setupGui();
    void guiEvent(ofxUIEventArgs &e);
    void mapAllImages();
    void scanDirectory(ofDirectory dir, string ext);// TODO: make "imgEditor::scanDirectory()" more generic and move it to its own class so it returns an array of file paths with a given extension, or an array of directory paths for any object that needs it
    void addImgToPathMap(string baseName, string path);
    void reiterateIt();
    
    ////////////////
    // properties //
    ////////////////
    bool                            initializing;
    ofxUISuperCanvas*               gui; // gui = new ofxUISuperCanvas("harlequin");
    string                          guiSettingsPath, imagesDirectory;
    //
    // joint tracking
    int                             trackedUserIndex, trackedUsersCount;
    vector< vector <MSAjoint*> >    joints;
    float                           jointsScale;
    int                             jointSetsCount, jointsCount;
    //
    // images
    map <string, vector<string> >   imagePathMap; // <string imgFileName, vector<string> imgPaths>
    map <string, vector<string> >::iterator it; // iterator storage for paths to image that is currently being edited
    int                             currentImgIndex;
    float                           currentImgIndexFloat;
    string                          currentImgBaseName;
    ofImage*                        img;
    float                           imgScale;
    bool                            imgMirrored;
    
private:
    
    // internal data
    
};

//#endif
