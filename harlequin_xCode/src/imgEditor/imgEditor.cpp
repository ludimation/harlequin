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
    if (imagePathMap.size()) {
        it = imagePathMap.begin();
        currentImgIndex = 1;
        currentImgIndexFloat = 1.0f;
    } else {
        it = imagePathMap.end();
        currentImgIndex = 0;
        currentImgIndexFloat = 0.0f;
    }
    

    ///////////////////
    // 2) gui & keypresses for displaying pathMap & selecting which image to load training
    //     - load metadata for an image if it already exists
    ///////////////////
    gui = new ofxUISuperCanvas("imgEditor");
    gui -> setWidth(600); // NOTE: setting the text of text areas with wider strings will crash the build with "Thread 1: signal SIGABRT"
    ofAddListener(gui -> newGUIEvent, this, &imgEditor::guiEvent);
    gui -> addSpacer();
    //
    // text field with currently displayed image baseName: slider to navigate list quickly, buttons & keybindings to navigate up and down one image
    gui -> addTextArea("current image baseName", "current image: " + it->first);
    gui -> addSlider("imagePathMap index", currentImgIndex, imagePathMap.size(), &currentImgIndexFloat);
    ofxUIButton *buttonPrevImg = gui -> addButton("- previous image", false);
    buttonPrevImg->bindToKey('[');
    buttonPrevImg->bindToKey('-');
    buttonPrevImg->bindToKey('_');
    ofxUIButton *buttonNextImg = gui -> addButton("+ next image", false);
    buttonNextImg->bindToKey(']');
    buttonNextImg->bindToKey('=');
    buttonNextImg->bindToKey('+');
    //
    // update list button
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
    string          nameStr = e.widget->getName();
    ofxUIWidgetType kind    = e.widget->getKind();
    ofxUIButton     *button;
    bool            buttonPressed;
    
    if (kind == OFX_UI_WIDGET_BUTTON) {
        button = (ofxUIButton*)e.widget;
        buttonPressed = button ->getValue();
    }
    
    /*  */ if(nameStr == "- previous image"){
        if (currentImgIndex > 0) {
            if (buttonPressed) currentImgIndexFloat -= 1.0f;
        }
    } else if(nameStr == "+ next image") {
        if (currentImgIndex < imagePathMap.size()) {
            if (buttonPressed) currentImgIndexFloat += 1.0f;
        }
    } else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE) cout << "[verbose] imgLoader::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
}

//--------------------------------------------------------------
void imgEditor::update() {
    
    // check to see if currentImgIndexFloat has been changed
    if ((int)currentImgIndexFloat != currentImgIndex) {
        
        // store the new image loaded
        currentImgIndex = (int)currentImgIndexFloat;

        // increment the image path map iterator to the appropriate image
        it = imagePathMap.begin();
        for(int i=1; i<currentImgIndex; ++i) {
            ++it;
        }
        
        // update the gui text field for current image being displayed to match the updated iterator
        ofxUIWidget* widget = gui -> getWidget("current image baseName");
        ofxUIWidgetType widgetType = widget->getKind();
        string newText = "current image: " + it->first;
        if (widgetType == OFX_UI_WIDGET_TEXTAREA) {
            ofxUITextArea * textArea = (ofxUITextArea *)widget;
            textArea -> setTextString(newText);
        } else {
            cout << "imgEditor::update() -- gui->getWidget(\"current image baseName\")->getKind() == " + ofToString(widgetType) << cout;
        }
    }
}

//--------------------------------------------------------------
void imgEditor::draw() {
    bool drawImage = false;
    
    if (img->loadImage(it->second[it->second.size()-1])) {
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
void imgEditor::scanDirectory(ofDirectory dir, string ext) {
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
void imgEditor::addImgToPathMap(string baseName, string imagePath) {
    if (imagePathMap.find(baseName) == imagePathMap.end()) {
        vector<string> pathVtmp;
        imagePathMap[baseName] = pathVtmp;
    }
    imagePathMap[baseName].push_back(imagePath);
}
