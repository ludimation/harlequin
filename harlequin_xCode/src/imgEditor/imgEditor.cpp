//
//  imgEditor.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "imgEditor.h"

//--------------------------------------------------------------
void imgEditor::setup() {
    ///////////////////
    // 1) create a map of all unique image file names in the images directory
    //     - include several paths for images that have multiple resolutions
    ///////////////////
    imagePathMap = mapAllImages();

    ///////////////////
    // 2) gui & keypresses for displaying pathMap & selecting which image to load training
    //     - load metadata for an image if it already exists
    ///////////////////
    gui = new ofxUISuperCanvas("imgEditor");
    ofAddListener(gui -> newGUIEvent, this, &imgEditor::guiEvent);
    gui -> addSpacer();
    //
    // display imagePathMap
    //    - list all images with selectable unique image names
    //    - list selectable multiple paths for a selected image
    //    - update list button
    gui -> addSpacer();
    //
    // load / display image button
    gui -> addToggle("display image", false);
    gui -> addSpacer();
    //
    // save image data button
    gui -> addLabelButton("save image data", false);
    //
    // save image loader settings button // what would these be? selected image + directories?
    gui -> addLabelButton("save imgLoader settings", false);
    gui -> autoSizeToFitWidgets();
    //
    // load settings
    
    
    ///////////////////
    // 3) gui for training image metadata
    ///////////////////
    // MSA joints for editing joint position data
    jointSets_count = 4;
    joints_count = 15;
    joints.resize(jointSets_count);
    for(int s = 0; s <jointSets_count; ++s) {
        for(int j = 0; j < joints_count; ++j) {
            MSAjoint *obj = new MSAjoint();
            obj->set(300 + (j*15), 400, 10, 10);
            obj->enabled = true;
            obj->enableMouseEvents();
            joints[s].push_back(obj);
        }
    }
    
    ///////////////////
    // 4) gui to save image metadata
    //     - at comon directory level of all image paths (remove "_540" / "_1080" portion)
    //     - include multiple paths in XML file for selection later on
    ///////////////////
    
}

//--------------------------------------------------------------
void imgEditor::guiEvent(ofxUIEventArgs &e) {
    string nameStr = e.widget->getName();
    int kind = e.widget->getKind();
    
    /*  */ if(nameStr == ""){
        //
    } else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE) cout << "[verbose] imgLoader::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
}

//--------------------------------------------------------------
void imgEditor::update() {
    
}

//--------------------------------------------------------------
void imgEditor::draw() {
    
}

//--------------------------------------------------------------
map <string, vector<string> > imgEditor::mapAllImages() {
    ///////////////////
    // 1) create a map of all unique image file names in the images directory
    //     - include several paths for images that have multiple resolutions
    ///////////////////
    map <string, vector<string> > pathMap;
    
    maxFilesToLoad = pathMap.size();
    
    return pathMap;
}