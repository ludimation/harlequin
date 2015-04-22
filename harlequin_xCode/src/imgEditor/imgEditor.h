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
    void setup();
    void exit();
    void update();
    void draw();
    
    // image handling functions
    void                            guiEvent(ofxUIEventArgs &e);
    void                            mapAllImages();
    void                            scanDirectory(
                                                    ofDirectory                     dir
//                                                  , map <string, vector<string> >   &pathMap
                                                  , string                          ext
                                                  );
    void                            addImgToPathMap(string baseName, string path);
    
    // properties
    ofxUISuperCanvas*               gui; // gui = new ofxUISuperCanvas("harlequin");
    vector< vector <MSAjoint*> >    joints;
    int                             jointSets_count, joints_count;
    map <string, vector<string> >   imagePathMap; // <string imgFileName, vector<string> imgPaths>
    map <string, vector<string> >::iterator it; // iterator storage for paths to image that is currently being edited
    ofImage*                        img;
    int                             currentImgIndex;
    float                           currentImgIndexFloat;
    string                          currentImgBaseName;
    
private:
    
    // internal data
    
};

//#endif
