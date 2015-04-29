//
//  imgEditor.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "imgEditor.h"

//--------------------------------------------------------------
void imgEditor::setup(string guiSettingsPath_, string imagesDirectory_, string imageJointDataDirectory_) {
    
    initializing = true; // flag to check to make sure application is not running initialization operations twice
    currentImgBaseName = "";
    guiJntDataTglMtxSize = 16;
    jointsScale = -0.5f;
    trackedUserJointsSize = 10;

    guiSettingsPath = guiSettingsPath_;
    imagesDirectory = imagesDirectory_;
    imageJointDataDirectory = imageJointDataDirectory_;
    
    ///////////////////
    // 1) create a map of all unique image file names in the images directory
    //     - include several paths for images that have multiple resolutions
    ///////////////////
    mapAllImages();
    
    ///////////////////
    // 2) setup objects and interactive elements for training image metadata
    //     - load metadata for an image if it already exists
    ///////////////////
    // MSA joints for displaying joint position data before capturing it
    jointsCount = 15; // TODO: move these into imgData object to clean up this editor
    jointsAnchorInPercentages.set(0.5f, 0.5f);
    for(int jnt = 0; jnt < jointsCount; ++jnt) {
        MSAjoint *obj = new MSAjoint();
        obj->set(
                   830 + (jnt * (trackedUserJointsSize + 5))
                 , trackedUserJointsSize + 5
                 , trackedUserJointsSize
                 , trackedUserJointsSize
                 );

        obj->setColors(0x00FFFF, 0x0000FF, 0xFFFF00);
        joints.push_back(obj);
    }

    ///////////////////
    // 3) setup a gui & keybindings for displaying pathMap & selecting which image to load training
    ///////////////////
    setupGui();
    
    ///////////////////
    // 4) setup gui & functions that save image metadata
    //     - at comon directory level of all image paths (remove "_540" / "_1080" portion)
    //     - include multiple paths in XML file for selection later on
    ///////////////////
    
    initializing = false; // done initializing
}

//--------------------------------------------------------------
void imgEditor::exit() {
    
    // clean up objects
    delete gui; gui = NULL;
    delete imgDataObj; imgDataObj = NULL;
    
    // MSAjoints list
    for (int i = 0; i < joints.size(); ++i) {
        delete joints[i]; joints[i] = NULL;
    }
    vector<MSAjoint*>().swap(joints); // TODO: joints -- does swapping it for an empty vector call "delete" for each object pointed to?
}

//--------------------------------------------------------------
void imgEditor::enable() {
    enabled = true;
    if (gui) gui->enable();
    if (imgDataObj) imgDataObj->setMSAJntVisibility(true);
    setMSAJntVisibility(true);
}

//--------------------------------------------------------------
void imgEditor::disable() {
    enabled = false;
    if (gui) gui->disable();
    if (imgDataObj) imgDataObj->setMSAJntVisibility(false);
    setMSAJntVisibility(false);
}

//--------------------------------------------------------------
void imgEditor::setMSAJntVisibility(bool visible_) {
    setMSAJntVisibility(joints, visible_);
}
void imgEditor::setMSAJntVisibility(vector< MSAjoint* > joints_, bool visible_) {
    // iterate through joints and set visibility
    for (int jnt = 0; jnt < joints_.size(); ++jnt) {
        if (visible_) joints_[jnt]->enableAppEvents();
        else joints_[jnt]->disableAppEvents();
    }
}

//--------------------------------------------------------------
void imgEditor::update(vector< vector<ofPoint> > trackedUserJoints_) {
    if (!enabled) return;
    
    // load a new image if currentImgIndexFloat has been changed
    if ((int)currentImgIndexFloat != currentImgIndex) {
        
        
        // clamp current ImgIndex to length of image map
        if ((int)currentImgIndexFloat > imagePathMap.size()) currentImgIndexFloat = imagePathMap.size();
        
        // store the new image index
        currentImgIndex = (int)currentImgIndexFloat;

        // increment the image path map iterator to the appropriate image
        reiterateIt();
        loadImgData();
        
        currentImgBaseName = it->first;
        
        // update the gui text field for current image being displayed to match the updated iterator
        ofxUIWidget *widget = gui -> getWidget("current image baseName");
        ofxUIWidgetType widgetType = widget->getKind();
        string newText = "current image: " + it->first;
        if (widgetType == OFX_UI_WIDGET_TEXTAREA) {
            ofxUITextArea   *textArea = (ofxUITextArea *)widget;
            textArea -> setTextString(newText);
        } else {
            cout << "imgEditor::update() -- gui->getWidget(\"current image baseName\")->getKind() == " + ofToString(widgetType) << cout;
        }
        
    }
    
    // update GUI to match number of users in trackedUserJoints
    if (trackedUsersCount != trackedUserJoints_.size()) {
        trackedUsersCount = trackedUserJoints_.size();
        // TODO: updated GUI to reflect number of possible tracked users
        
    }
    
    // update MSAjoints positions to match trackedUserJoints
    // create a vector of an MSAjoint object per joint
    for(int jnt = 0; jnt < jointsCount; ++jnt) {
        MSAjoint *obj = joints[jnt];
        if (trackedUserIndex < trackedUsersCount) {
            ofPoint j = trackedUserJoints_[trackedUserIndex][jnt];
            obj->setPosition3D(
                                 ofGetWidth()  * jointsAnchorInPercentages.x    + j.x * jointsScale
                               , ofGetHeight() * jointsAnchorInPercentages.y    + j.y * jointsScale
                               ,                                                + j.z * jointsScale
                             );
        } else {
            obj->set(830 + (jnt*15), 15, 10, 10);
        }
    }
}

//--------------------------------------------------------------
void imgEditor::draw(bool drawMirrored_) {
    if (imgDataObj && enabled) imgDataObj -> draw(drawMirrored_);
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
    
    // get recursive list of jpg files in images directory
    string path = ofToDataPath(imagesDirectory);
    string ext = "jpg";
    ofDirectory dir(path);
//    ofDirUtils dirUtils;
//    vector<string> fileListing = dirUtils.listFilesOfType(dir, ext);
    vector<string> fileListing = listFilesOfType(dir, ext);
    
    // pupulate map
    if (fileListing.size()) {
        for (int i = 0; i < fileListing.size(); ++i) {
            string filePath = fileListing[i];
            string fileBaseName = ofFile(filePath).getBaseName();

            addImgToPathMap(fileBaseName, filePath);
            
            // debug
            // cout << "imgEditor::mapAllImages() -- fileBaseName = " << fileBaseName << endl;
            // cout << "                          -- filePath = " << filePath << endl;

        }
    }

    
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
vector<string> imgEditor::listFilesOfType(ofDirectory dir_, string ext_, vector<string> fileList_) {
    int i, size;
    size = dir_.listDir();
    dir_.sort();
    vector<string> fileList;

    for (int i = 0; i < size; i++) {
        ofFile  fileObj         = dir_.getFile(i);
        bool    fileIsDir       = fileObj.isDirectory();
        string  filePath        = fileObj.path();
        string  fileExt         = fileObj.getExtension();
        if (fileIsDir) {
            ofDirectory newDir(filePath);
            fileList = listFilesOfType(newDir, ext_);
            for (int j = 0; j < fileList.size(); ++j) {
                fileList_.push_back(fileList[j]);
            }
        }else if (fileExt == ext_) {
            fileList_.push_back(filePath);
            // debug
            // cout << "imgEditor::listFilesOfType() -- filePath = " << filePath << endl;
        }
    }
    
    return fileList_;
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

//--------------------------------------------------------------
void imgEditor::loadImgData() {
    delete imgDataObj; imgDataObj = NULL;
    imgDataObj = new imgData();
    imgDataObj -> open(it, imageJointDataDirectory, jointsScale);
    
    if(gui) {
        // reset imgDataObj dependent gui elements
        guiJntDataTglMtx ->setAllToggles(false);
        for (int tgl = 0; tgl < guiJntDataTglMtxSize; ++tgl){
            if (tgl < imgDataObj->getTrnDataSize()) {
                guiJntDataTglMtxTgls[tgl]->setVisible(true);
            } else {
                guiJntDataTglMtxTgls[tgl]->setVisible(false);
            }
        }
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
    ofxUIButton *buttonPrevImg = gui -> addButton("'[' previous image", false);
    buttonPrevImg -> bindToKey('[');
    buttonPrevImg -> bindToKey('{');
    ofxUIButton *buttonNextImg = gui -> addButton("']' next image", false);
    buttonNextImg -> bindToKey(']');
    buttonNextImg -> bindToKey('}');
    gui -> addSpacer();
    //
    // update list button
    ofxUILabelButton *buttonUpdateImgList = gui -> addLabelButton("'u' update image list", false);
    buttonUpdateImgList -> bindToKey('u');
    buttonUpdateImgList -> bindToKey('U');
    gui -> addSpacer();
    //
    // tracked user radio
    gui -> addLabel("'<' '>' user being tracked");
    vector<string> trkdUserV;
    trkdUserV.push_back("0"); trkdUserV.push_back("1"); trkdUserV.push_back("2"); trkdUserV.push_back("3"); trkdUserV.push_back("4"); trkdUserV.push_back("5"); trkdUserV.push_back("6"); trkdUserV.push_back("7"); trkdUserV.push_back("<"); trkdUserV.push_back(">");
    ofxUIRadio *trkdUserRadio = gui -> addRadio("traked user", trkdUserV, OFX_UI_ORIENTATION_HORIZONTAL);
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
    guiJntDataTglMtx = gui -> addToggleMatrix("trained data joints", 1, guiJntDataTglMtxSize);
    guiJntDataTglMtx -> bindToKey(';');
    guiJntDataTglMtx -> bindToKey(':');
    guiJntDataTglMtx -> bindToKey('\'');
    guiJntDataTglMtx -> bindToKey('"');
    guiJntDataTglMtx -> bindToKey('A');
    guiJntDataTglMtx -> setAllowMultiple(true);
    guiJntDataTglMtxTgls = guiJntDataTglMtx -> getToggles();
    for (int i = 0; i < guiJntDataTglMtxTgls.size(); ++i) {
        guiJntDataTglMtxTgls[i] -> setVisible(false);
    }
    // transfer data to joints from selected data set (grabs first one fornow, would be nice to do an axis-angle average at some point)
    ofxUILabelButton *setJointsFromTrainingJointSetButton = gui -> addLabelButton("'g' set training joints from first selected data set", false);
    setJointsFromTrainingJointSetButton -> bindToKey('g');
    setJointsFromTrainingJointSetButton -> bindToKey('G');
    // delete selected data points
    ofxUILabelButton *deleteSelJntSetsButton = gui -> addLabelButton("'r' remove selected data joints", false);
    deleteSelJntSetsButton -> bindToKey('r');
    // delete data points that are un-selected
    ofxUILabelButton *deleteNonSelJntSetsButton = gui -> addLabelButton("'R' remove UN-selected data joints", false);
    deleteNonSelJntSetsButton -> bindToKey('R');
    gui -> addSpacer();
    //
    // image scale update buttons
    ofxUIButton *buttonScaleDown = gui -> addButton("'-' scale down image", false);
    buttonScaleDown -> bindToKey('-');
    buttonScaleDown -> bindToKey('_');
    ofxUIButton *buttonScaleUp = gui -> addButton("'+' scale up image", false);
    buttonScaleUp -> bindToKey('=');
    buttonScaleUp -> bindToKey('+');
    gui -> addSpacer();
    //
    // save image data button
    ofxUILabelButton *saveImgDataButton = gui -> addLabelButton("'d' save image data", false);
    saveImgDataButton -> bindToKey('d');
    saveImgDataButton -> bindToKey('D');
    gui -> addSpacer();
    //
    // save image loader settings button // what would these be? selected image + directories?
    ofxUILabelButton *saveImgEditorSettingsButton = gui -> addLabelButton("'s' save imgEditor settings", false);
    saveImgEditorSettingsButton -> bindToKey('s');
    saveImgEditorSettingsButton -> bindToKey('S');
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
    ofxUIRadio          *radio;
    ofxUIToggleMatrix   *toggleMatrix;
    bool                buttonPressed = false;
    bool                buttonReleased = false;
    
    /*  */ if (kind == OFX_UI_WIDGET_BUTTON) {
        button = (ofxUIButton*)e.widget;
        buttonPressed = button -> getValue();
    } else if (kind == OFX_UI_WIDGET_LABELBUTTON) {
        labelbutton = (ofxUILabelButton*)e.widget;
        buttonPressed = labelbutton -> getValue();
    } else if (kind == OFX_UI_WIDGET_TOGGLEMATRIX) {
        toggleMatrix = (ofxUIToggleMatrix*)e.widget;
    } else if (kind == OFX_UI_WIDGET_TOGGLE) {
        if (e.widget -> getParent() -> getName() == "trained data joints") upatedTrnDataVisibilty();
    }
    // register button release only when mouse buttons are not pressed and application is not initializing
    if (!buttonPressed && !initializing && !ofGetMousePressed()) buttonReleased = true;
    
    
    
    /*  */ if(nameStr == "'[' previous image"){
        if (currentImgIndex > 1) {
            if (buttonPressed) currentImgIndexFloat -= 1.0f;
        }
    } else if(nameStr == "']' next image") {
        if (currentImgIndex < imagePathMap.size()) {
            if (buttonPressed) currentImgIndexFloat += 1.0f;
        }
    } else if(nameStr == "'u' update image list") {
        if (buttonReleased) {
            mapAllImages();
        }
        
        
    } else if (nameStr == "tracked user" ||nameStr=="0"||nameStr=="1"||nameStr=="2"||nameStr=="3"||nameStr=="4"||nameStr=="5"||nameStr=="6"||nameStr=="7"||nameStr=="<"||nameStr==">") {
        //
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
    } else if (nameStr == "'b' capture user joints") {
        if (buttonReleased) {
            int trnDataSize = imgDataObj -> getTrnDataSize();
            if (trnDataSize < guiJntDataTglMtxSize) {
                // store training joint positions imgDataObj
                imgDataObj -> pushTrnData(joints);
                // update "';' ''' captured data joints" gui to reflect an added data point
                guiJntDataTglMtxTgls[imgDataObj -> getTrnDataSize()-1] -> toggleVisible();
                guiJntDataTglMtx -> setAllToggles(false);
                guiJntDataTglMtxTgls[imgDataObj -> getTrnDataSize()-1] -> toggleValue();
            } else {
                cout << "imgEditor:: guiEvent(ofxUIEventArgs &e) -- already trained maximum training data sets = " << ofToString(guiJntDataTglMtxTgls.size()) << endl;
                cout << " -- delete some training data sets for this image in order to create new ones." << endl ;
            }
            upatedTrnDataVisibilty();
        }
    } else if (nameStr == "'g' set training joints from first selected data set") {
        if (buttonReleased) {
            // cycle through toggles from start and transfer data from first selected toggle set to imgData joints
            int tgl;
            for (tgl = 0; tgl < imgDataObj -> getTrnDataSize(); ++tgl) {
                if (guiJntDataTglMtxTgls[tgl] -> getValue()) {
                    imgDataObj -> setJointsFromTrainingJointSet(tgl);
                    break;
                }
            }
            // reset all toggles
            guiJntDataTglMtx -> setAllToggles(false);
            guiJntDataTglMtxTgls[tgl] -> toggleValue();
            upatedTrnDataVisibilty();
        }
    } else if (nameStr == "'r' remove selected data joints") {
        if (buttonReleased) {
            // cycle through toggles from end to start and erase data for selected sets
            for (int tgl = imgDataObj -> getTrnDataSize()-1; tgl >= 0; --tgl) {
                if (guiJntDataTglMtxTgls[tgl] -> getValue()) {
                    imgDataObj -> eraseTrnData(tgl);
                    guiJntDataTglMtxTgls[imgDataObj -> getTrnDataSize()] -> toggleVisible();
                }
            }
            // reset all toggles
            guiJntDataTglMtx -> setAllToggles(false);
            upatedTrnDataVisibilty();
        }
    } else if (nameStr == "'R' remove UN-selected data joints") {
        if (buttonReleased) {
            // cycle through toggles from end to start and erase data for unselected sets
            for (int tgl = imgDataObj -> getTrnDataSize()-1; tgl >= 0; --tgl) {
                if (!guiJntDataTglMtxTgls[tgl] -> getValue()) {
                    imgDataObj -> eraseTrnData(tgl);
                    guiJntDataTglMtxTgls[imgDataObj -> getTrnDataSize()] -> toggleVisible();
                }
            }
            // reset all toggles
            guiJntDataTglMtx -> setAllToggles(false);
            // re-select all visible toggles
            for (int tgl = 0; tgl< imgDataObj -> getTrnDataSize(); ++tgl) {
                guiJntDataTglMtxTgls[tgl] -> toggleValue();
            }
            upatedTrnDataVisibilty();
        }
        
        
    } else if (nameStr == "'-' scale down image" || nameStr == "'+' scale up image") {
        if (buttonReleased) {
            int sign = -1;
            float scaleAmt = 0.05f; // five percent
            if (nameStr == "'+' scale up image") sign *= sign;
            // scale more when shift key is pressed
            bool rightShft = ofGetKeyPressed(OF_KEY_RIGHT_SHIFT);
            bool leftShft = ofGetKeyPressed(OF_KEY_LEFT_SHIFT);
            bool shft = ofGetKeyPressed(OF_KEY_SHIFT);
            bool keysPressed = ofGetKeyPressed();
            cout << "imgEditor::guiEvent()  -- rightShftPressed = " << rightShft << endl;
            cout << "                       -- leftShftPressed = " << leftShft << endl;
            cout << "                       -- shftPressed = " << shft << endl;
            cout << "                       -- keyPressed = " << keysPressed << endl;
            if (rightShft || leftShft) { // TODO: guiEvent -- scale image -- debug SHIFT keypress checks which don't seem to be working.
                scaleAmt *= 10.0f;
            }
            imgDataObj->scaleImages(1+ sign * scaleAmt);
        }
    
        
    } else if (nameStr == "'d' save image data") {
        if (buttonReleased) {
            imgDataObj->save();
        }
    
        
    } else if (nameStr == "'s' save imgEditor settings") {
        if (buttonReleased) {
            gui->saveSettings(guiSettingsPath);
        }
    }else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE)
            cout << "[verbose] imgLoader::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
}

//--------------------------------------------------------------
void imgEditor::upatedTrnDataVisibilty() {
    if(imgDataObj) {
        // cycle through toggles and show / hide training data based on values
        for (int tgl = 0; tgl < imgDataObj->getTrnDataSize(); ++tgl) {
            if (guiJntDataTglMtxTgls[tgl] -> getValue()) {
                imgDataObj -> setTrnDataVisibilty(tgl, true);
            } else {
                imgDataObj -> setTrnDataVisibilty(tgl, false);
            }
        }
    }
}