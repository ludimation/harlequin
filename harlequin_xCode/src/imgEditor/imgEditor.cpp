//
//  imgEditor.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "imgEditor.h"

//--------------------------------------------------------------
void imgEditor::setup(string guiSettingsPath_, string imagesDirectory_) {
    
    initializing = true; // flag to check to make sure application is not running initialization operations twice
    
    ///////////////////
    // 1) create a map of all unique image file names in the images directory
    //     - include several paths for images that have multiple resolutions
    ///////////////////
    imagesDirectory = imagesDirectory_;
    mapAllImages();

    ///////////////////
    // 2) setup a gui & keybindings for displaying pathMap & selecting which image to load training
    ///////////////////
    guiSettingsPath = guiSettingsPath_;
    setupGui();
    
    ///////////////////
    // 3) setup interactive elements for training image metadata
    //     - load metadata for an image if it already exists
    ///////////////////
    img = new ofImage();
    jointsScale = -0.5f;
    // MSA joints for editing joint position data
    jointSetsCount = 18; // 0 is input skeleton, 1 is trained skeleton, 2-17 are training data points skeleton
    jointsCount = 15;
    joints.resize(jointSetsCount);
    for(int set = 0; set <jointSetsCount; ++set) {
        for(int jnt = 0; jnt < jointsCount; ++jnt) {
            MSAjoint *obj = new MSAjoint();
            obj->set(830 + (jnt*15), 15 + (set*15), 10, 10);
            if (set == 0) {
                // set input-based joint color
                obj->setColors(0x00FFFF, 0x0000FF, 0xFFFF00);
            } else if (set == 1) {
                // set user-specified joint color
                obj->setColors(0xFFFF00, 0x00FF00, 0xFF00FF);
                obj->enableMouseEvents();
                obj->setDraggable(true);
            } else {
                // set traing data joint color
            }
            joints[set].push_back(obj);
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
void imgEditor::update(vector< vector<ofPoint> > trackedUserJoints) {
    
    // load a new image if currentImgIndexFloat has been changed
    if ((int)currentImgIndexFloat != currentImgIndex) {
        
        // clamp current ImgIndex to length of image map
        if ((int)currentImgIndexFloat > imagePathMap.size()) currentImgIndexFloat = imagePathMap.size();
        
        // store the new image index
        currentImgIndex = (int)currentImgIndexFloat;

        // increment the image path map iterator to the appropriate image
        reiterateIt();
        
        currentImgBaseName = "";
        imgMirrored = false;
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
    
    // update GUI to match number of users in trackedUserJoints
    if (trackedUsersCount != trackedUserJoints.size()) {
        trackedUsersCount = trackedUserJoints.size();
        // TODO: updated GUI to reflect number of possible tracked users
        
    }
    
    // update MSAjoints positions to match trackedUserJoints
    for(int jnt = 0; jnt < jointsCount; ++jnt) {
        for (int set = 0; set < jointSetsCount; ++set) {
            MSAjoint *obj = joints[set][jnt];
            if (set == 0) {
                if (trackedUserIndex < trackedUsersCount) {
                    ofPoint j = trackedUserJoints[trackedUserIndex][jnt];
                    obj->setPosition(
                                     ofGetWidth() / 2 + j.x * jointsScale
                                     , ofGetHeight() / 2 + j.y * jointsScale
                                     );
                } else {
                    obj->set(830 + (jnt*15), 15 + (set*15), 10, 10);
                }
            } else if (set == 1) {
                // set position based on user-specified skeleton
            } else {
                // set position based on trained data sets
            }
        }
    }
}

//--------------------------------------------------------------
void imgEditor::draw(bool drawMirrored) {
    if (currentImgBaseName != "") {
        
        // calculate imageRatio
        float imgRatioX;
        float imgRatioY;
        float imgRatio;
        if (img->width) {
            imgRatioX = float(ofGetWidth()) / float(img->width);
        } else {
            imgRatioX = 1.0f;
        }
        if (img->height) {
            imgRatioY = float(ofGetHeight()) / float(img->height);
        } else {
            imgRatioY = 1.0f;
        }
        if (imgRatioX < imgRatioY) {
            imgRatio = imgRatioX;
        } else {
            imgRatio = imgRatioY;
        }
        
        // mirror image if necessary // TODO: is there a way to do this with a transformation instead?
        if (imgMirrored != drawMirrored) {
            img->mirror(0, true);
            imgMirrored = drawMirrored;
        }
        img->setAnchorPercent(0.5f, 0.5f);
        
        ofPushStyle();
        img->draw(
                    ofGetWidth() / 2
                  , ofGetHeight() / 2
                  , img->width * imgRatio
                  , img->height * imgRatio
                  );
        
        ofPopStyle();
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
    string path = ofToDataPath(imagesDirectory);
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


//==============================================================
////////////////////////////////////////////////////////////////
// sets up gui & keybindings setup for displaying & selecting
// which image to load for training
////////////////////////////////////////////////////////////////

void imgEditor::setupGui() {

    gui = new ofxUISuperCanvas("imgEditor");
    gui -> setWidth(600); // NOTE: setting the text of text areas with wider strings will crash the build with "Thread 1: signal SIGABRT"
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
    // GUI for selecting user from tracked joints list
    gui -> addLabel("'<' '>' user being tracked");
    vector<string> trkdUserV;
    trkdUserV.push_back("0"); trkdUserV.push_back("1"); trkdUserV.push_back("2"); trkdUserV.push_back("3"); trkdUserV.push_back("4"); trkdUserV.push_back("5"); trkdUserV.push_back("6"); trkdUserV.push_back("7"); trkdUserV.push_back("<"); trkdUserV.push_back(">");
    ofxUIRadio* trkdUserRadio = gui -> addRadio("traked user", trkdUserV, OFX_UI_ORIENTATION_HORIZONTAL);
    trkdUserRadio -> getEmbeddedWidget(8) -> bindToKey(',');
    trkdUserRadio -> getEmbeddedWidget(8) -> bindToKey('<');
    trkdUserRadio -> getEmbeddedWidget(9) -> bindToKey('.');
    trkdUserRadio -> getEmbeddedWidget(9) -> bindToKey('>');
    trkdUserRadio -> activateToggle(ofToString(trackedUserIndex));
    // capture user data button
    ofxUILabelButton *captureUsrJntsButton = gui -> addLabelButton("'b' capture user joints", false);
    captureUsrJntsButton -> bindToKey('b');
    captureUsrJntsButton -> bindToKey('B');
    gui -> addSpacer();
    //
    // select user data radio (add & remove embedded toggles)
    gui -> addLabel("';' ''' captured data joints");
    ofxUIToggleMatrix *dataTglMtx = gui -> addToggleMatrix("trained data joints", 1, 16);
    dataTglMtx -> bindToKey(';');
    dataTglMtx -> bindToKey(':');
    dataTglMtx -> bindToKey('\'');
    dataTglMtx -> bindToKey('"');
    dataTglMtx -> bindToKey('A');
    dataTglMtx -> setAllowMultiple(true);
    vector<ofxUIToggle*> dataTglMtxTgls = dataTglMtx -> getToggles();
    for (int i = 0; i < dataTglMtxTgls.size(); ++i) {
        dataTglMtxTgls[i] -> setVisible(false);
    }
    // delete selected data points
    ofxUILabelButton *deleteSelJntSetsButton = gui -> addLabelButton("'r' remove selected data joints", false);
    deleteSelJntSetsButton -> bindToKey('r');
    deleteSelJntSetsButton -> bindToKey('R');
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
    ofAddListener(gui -> newGUIEvent, this, &imgEditor::guiEvent);
    // load settings
    gui -> loadSettings(guiSettingsPath);
    
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
        
        
    } else if (nameStr == "tracked user" ||nameStr=="0"||nameStr=="1"||nameStr=="2"||nameStr=="3"||nameStr=="4"||nameStr=="5"||nameStr=="6"||nameStr=="7"||nameStr=="<"||nameStr==">") {
        //
        ofxUIRadio* radio;
        if (kind == OFX_UI_WIDGET_RADIO) {
            radio = (ofxUIRadio*)e.widget;
        } else {
            radio = (ofxUIRadio*)e.widget->getParent();
        }
        //
        int i = radio -> getValue();
        if (i!= trackedUserIndex)
            trackedUserIndex =
              (i>=0 && i<=7) * i
            + (trackedUserIndex-1>=0 && i==8) * (trackedUserIndex-1)
            + (trackedUserIndex+1<=7 && i==9) * (trackedUserIndex+1)
            + (trackedUserIndex==7&&i==9) * trackedUserIndex
            ;
        //
        radio -> activateToggle(ofToString(trackedUserIndex));
        
        
    } else if (nameStr == "'s' save imgLoader settings") {
        if (!buttonPressed && !initializing && !ofGetMousePressed()) {
            // when button is released, not when dragging out, or simply when gui is created during setup();
            gui->saveSettings(guiSettingsPath);
        }
    }else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE) cout << "[verbose] imgLoader::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
}