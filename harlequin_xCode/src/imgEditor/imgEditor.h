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
    void update();
    void draw();
    
    // image handling functions
    void                            guiEvent(ofxUIEventArgs &e);
    map <string, vector<string> >   mapAllImages();
    void                            scanDirectory(
                                                    ofDirectory                     dir
                                                  , map <string, vector<string> >   &pathMap
                                                  , string                          &ext
                                                  );
    
    // properties
    ofxUISuperCanvas*               gui; // gui = new ofxUISuperCanvas("harlequin");
    vector< vector <MSAjoint*> >    joints;
    int                             jointSets_count, joints_count;
    map <string, vector<string> >   imagePathMap; // <string imgFileName, vector<string> imgPaths>
    bool                            loadImagesNow;
    int                             nFilesToLoad, maxFilesToLoad;
    
private:
    
    // internal data
    
};

//#endif
