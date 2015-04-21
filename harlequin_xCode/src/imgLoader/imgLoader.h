//
//  imgLoader.h
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

//#ifndef __harlequin__imgLoader__
//#define __harlequin__imgLoader__

#pragma once

#include "ofxUI.h"

class imgLoader {
    
public:
    
    // functions
    void setup();
    void guiEvent(ofxUIEventArgs &e);
    void loadImages(bool load);
    
    
    // properties
    ofxUISuperCanvas*   gui;
    int                 maxFilesToLoad, nFilesToLoad;
    bool                loadImagesNow;
    vector< string >    directoriesAll;
    
private:
    
    // internal data
    
};


//#endif /* defined(__harlequin__imgLoader__) */
