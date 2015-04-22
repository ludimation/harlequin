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
    
    initializing = true; // flag to check to make sure application is not running initialization operations twice
    
    ///////////////////
    // 1) create a map of all unique image file names in the images directory
    //     - include several paths for images that have multiple resolutions
    ///////////////////
    mapAllImages();

    ///////////////////
    // 2) setup a gui & keybindings for displaying pathMap & selecting which image to load training
    ///////////////////
    guiSettingsPath = ofToDataPath("settings/imgEditorGuiSettings.xml");
    gui = new ofxUISuperCanvas("imgEditor");
    gui -> setWidth(600); // NOTE: setting the text of text areas with wider strings will crash the build with "Thread 1: signal SIGABRT"
    ofAddListener(gui -> newGUIEvent, this, &imgEditor::guiEvent);
    gui -> addSpacer();
    //
    // text field with currently displayed image baseName: includes a slider to navigate list quickly, as well as buttons & keybindings to navigate up and down one image at a time
    gui -> addTextArea("current image baseName", "current image: " + it->first);
    gui -> addSlider("imagePathMap index", 1.0f, imagePathMap.size(), &currentImgIndexFloat);
    ofxUIButton *buttonPrevImg = gui -> addButton("'-' previous image", false);
    buttonPrevImg -> bindToKey('[');
    buttonPrevImg -> bindToKey('{');
    buttonPrevImg -> bindToKey('-');
    buttonPrevImg -> bindToKey('_');
    ofxUIButton *buttonNextImg = gui -> addButton("'+' next image", false);
    buttonNextImg -> bindToKey(']');
    buttonNextImg -> bindToKey('}');
    buttonNextImg -> bindToKey('=');
    buttonNextImg -> bindToKey('+');
    gui -> addSpacer();
    //
    // update list button
    ofxUILabelButton *buttonUpdateImgList = gui -> addLabelButton("'u' update image list", false);
    buttonUpdateImgList -> bindToKey('u');
    buttonUpdateImgList -> bindToKey('U');
    gui -> addSpacer();
    //
    // save image data button
    ofxUILabelButton *buttonSaveData = gui -> addLabelButton("'d' save image data", false);
    buttonSaveData -> bindToKey('d');
    buttonSaveData -> bindToKey('D');
    gui -> addSpacer();
    //
    // save image loader settings button // what would these be? selected image + directories?
    ofxUILabelButton *buttonSaveSettings = gui -> addLabelButton("'s' save imgLoader settings", false);
    buttonSaveSettings -> bindToKey('s');
    buttonSaveSettings -> bindToKey('S');
    //
    // closing operations
    gui -> autoSizeToFitWidgets();
    // load settings
    gui -> loadSettings(guiSettingsPath);
    
    ///////////////////
    // 3) setup interactive elements for training image metadata
    //     - load metadata for an image if it already exists
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
    // 4) setup gui & functions that save image metadata
    //     - at comon directory level of all image paths (remove "_540" / "_1080" portion)
    //     - include multiple paths in XML file for selection later on
    ///////////////////
 
    
    initializing = false; // done initializing
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
    string              nameStr = e.widget->getName();
    ofxUIWidgetType     kind    = e.widget->getKind();
    ofxUIButton         *button;
    ofxUILabelButton    *labelbutton;
    bool                buttonPressed = false;
    
    if (kind == OFX_UI_WIDGET_BUTTON) {
        button = (ofxUIButton*)e.widget;
        buttonPressed = button -> getValue();
    } else if (kind == OFX_UI_WIDGET_LABELBUTTON) {
        labelbutton = (ofxUILabelButton*)e.widget;
        buttonPressed = labelbutton -> getValue();
    }
    
    /*  */ if(nameStr == "'-' previous image"){
        if (currentImgIndex > 1) {
            if (buttonPressed) currentImgIndexFloat -= 1.0f;
        }
    } else if(nameStr == "'+' next image") {
        if (currentImgIndex < imagePathMap.size()) {
            if (buttonPressed) currentImgIndexFloat += 1.0f;
        }
    } else if(nameStr == "'u' update image list") {
        if (!buttonPressed && !initializing && !ofGetMousePressed()) {
            // when button is released, not when dragging out, or simply when gui is created during setup();
            mapAllImages();
        }
    } else if (nameStr == "'s' save imgLoader settings") {
        if (!buttonPressed && !initializing && !ofGetMousePressed()) {
            // when button is released, not when dragging out, or simply when gui is created during setup();
            gui->saveSettings(guiSettingsPath);
        }
    }else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE) cout << "[verbose] imgLoader::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
}

//--------------------------------------------------------------
void imgEditor::update() {
    
    // check to see if currentImgIndexFloat has been changed
    if ((int)currentImgIndexFloat != currentImgIndex) {
        
        // clamp current ImgIndex to length of image map
        if ((int)currentImgIndexFloat > imagePathMap.size()) currentImgIndexFloat = imagePathMap.size();
        
        // store the new image index
        currentImgIndex = (int)currentImgIndexFloat;

        // increment the image path map iterator to the appropriate image
        reiterateIt();
        
        currentImgBaseName = "";
        bool imgLoaded = img->loadImage(it->second[it->second.size()-1]);
        if (imgLoaded) {
            currentImgBaseName = it->first;
        } else {
            cout << "imgEditor::update() -- could not load image at it->second[it->second.size()-1] = "
            << it->second[it->second.size()-1] << endl;
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
    if (currentImgBaseName != "") {
        img->draw(100, 100);
    }
}

//==============================================================
////////////////////////////////////////////////////////////////
// create a map of all unique image file names in the
// images directory, including several paths for images that have
// multiple resolutions
////////////////////////////////////////////////////////////////

void imgEditor::mapAllImages() {
    
    // debug
    cout << "imgEditor::mapAllImages() -- called" << endl;
    
    // clear the map it if has already been populated
    if (imagePathMap.size()) imagePathMap.clear();
    
    // pupulate map
    string path = ofToDataPath("images");
    string ext = "jpg";
    ofDirectory dir(path);
    scanDirectory(dir, ext);
    
    // update editor variables based on map dimensions
    if (imagePathMap.size()){
        it = imagePathMap.begin();
        currentImgIndex = 0;
    } else {
        it = imagePathMap.end();
        currentImgIndex = 0;
        currentImgIndexFloat = 0.0f;
        currentImgBaseName = "";
        cout << "imgEditor::mapAllImages() -- no images found in the 'data/images' directory" << endl;
    }
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
            //            cout << "imgEditor::scanDirectory() -- fileBaseName = " << fileBaseName << endl;
            //            cout << "-- fileAbsPath = " << fileAbsPath << endl;
            //            cout << "-- filePath = " << filePath << endl;
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

//--------------------------------------------------------------
void imgEditor::reiterateIt() {
    it = imagePathMap.begin();
    for(int i=1; i<currentImgIndex; ++i) {
        ++it;
    }
}
