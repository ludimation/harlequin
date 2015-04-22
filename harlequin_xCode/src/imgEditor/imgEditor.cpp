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
    mapAllImages();
    currentImgIndex = 1;
    currentImgIndexFloat = 1.0f;

    ///////////////////
    // 2) gui & keypresses for displaying pathMap & selecting which image to load training
    //     - load metadata for an image if it already exists
    ///////////////////
    gui = new ofxUISuperCanvas("imgEditor");
    ofAddListener(gui -> newGUIEvent, this, &imgEditor::guiEvent);
    gui -> addSpacer();
    //
    // display imagePathMap
    //    - text field with currently displayed image baseName
    //        - (WISH: list all images with selectable unique image names)
    //            - vector<string> ddStrV; ddStrV.push_back("dirA"); ddStrV.push_back("dirB"); ddStrV.push_back("dirC");
    //              gui -> addDropDownList("directory list", ddStrV);
    //            - gui -> addRangeSlider(<#string _name#>, <#float _min#>, <#float _max#>, <#float *_valuelow#>, <#float *_valuehigh#>);
    //            - vector<string> slStrV; slStrV.push_back("imgA");  slStrV.push_back("imgB"); slStrV.push_back("imgC");
    //              gui -> addSortableList("images in directory", slStrV);
    //    - slider with index of currently displayed image(s)
    gui -> addSlider("imagePathMap index", 1, imagePathMap.size(), &currentImgIndexFloat);
    //    - previous image and next image buttons
    //    - sortable list with for multiple paths of the current image baseName
    vector<string> irpStrV; irpStrV.push_back("pathA");  irpStrV.push_back("pathB"); irpStrV.push_back("pathC");
    gui -> addSortableList("image resolution paths", irpStrV);
    //    - update list button
    gui -> addLabelButton("update image list", false);
    gui -> addSpacer();
    //
    // save image data button
    gui -> addLabelButton("save image data", false);
    //
    // save image loader settings button // what would these be? selected image + directories?
    gui -> addLabelButton("save imgLoader settings", false);
    gui -> autoSizeToFitWidgets();
    //
    // load settings?
    
    
    ///////////////////
    // 3) gui for training image metadata
    ///////////////////
    img = new ofImage();
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
void imgEditor::exit() {
    
    delete gui;
    delete img;
    joints.clear();
    vector<vector<MSAjoint*> >().swap(joints);
    
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
    if ((int)currentImgIndexFloat != currentImgIndex) {
        currentImgIndex = (int)currentImgIndexFloat;
        it = imagePathMap.begin() + currentImgIndex;
    }
}

//--------------------------------------------------------------
void imgEditor::draw() {
    if (img->loadImage(it->second[0])) {
        img->draw(100, 100);
    } else {
        cout << "imgEditor::draw() -- could not load image at it->second[0] = " << it->second[0] << endl;
    }
}

//--------------------------------------------------------------
void imgEditor::mapAllImages() {
    ///////////////////
    // 1) create a map of all unique image file names in the images directory
    //     - include several paths for images that have multiple resolutions
    ///////////////////
    
    string path = ofToDataPath("images");
    string ext = "jpg";
    ofDirectory dir(path);
    scanDirectory(dir, ext);
}

//--------------------------------------------------------------
void imgEditor::scanDirectory(ofDirectory dir, string ext) { // TODO: make this more generic so it returns an array of file paths with a given extension, or an array of directory paths
    int i, size;
    size = dir.listDir();
    dir.sort();
    
    for (int i = 0; i < size; i++) {
        ofFile  fileObj         = dir.getFile(i);
        bool    fileIsDir       = fileObj.isDirectory();
        string  fileBaseName    = fileObj.getBaseName();
        string  fileAbsPath     = fileObj.getAbsolutePath();
        string  filePath        = fileObj.path();
        string  fileExt         = fileObj.getExtension();
        if (fileIsDir) {
            ofDirectory newDir(fileAbsPath);
            scanDirectory(newDir, ext);
        }else if (fileExt == ext) {
            
            addImgToPathMap(fileBaseName, filePath);
            
            // debug
            cout << "imgEditor::scanDirectory() -- baseName = " << fileBaseName << endl;
            // cout << "imgEditor::scanDirectory() -- fileNameSTR = " << fileNameSTR << endl;
            // cout << "imgEditor::scanDirectory() -- path = " << path << endl;
        }
    }
}

//--------------------------------------------------------------
void imgEditor::addImgToPathMap(string baseName, string path) {
    if (imagePathMap.find(baseName) == imagePathMap.end()) {
        vector<string> pathVtmp;
        pathVtmp.push_back(path);
        imagePathMap[baseName] = pathVtmp;
    } else {
        imagePathMap[baseName].push_back(path);
    }
}
