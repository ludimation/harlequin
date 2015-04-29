//
//  GRTEditor.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "GRTEditor.h"

//--------------------------------------------------------------
void GRTEditor::setup(string guiSettingsPath_, string imgsDir_, string imgJntDataDir_, string imgTagDataDir_, string imgGRTDataDir_) {
    
    initializing = true;
    guiSettingsPath = guiSettingsPath_;
    imgsDirPath = imgsDir_;
    imgJntDataDirPath = imgJntDataDir_;
    
    //////////////
    // initialize vectors and maps with all necessary data
    //////////////
    mapAllGRTData(imgJntDataDir_, imgsDirPath);
    
    //////////////////
    // create an imgGRTData object to store, open, and save GRT data
    //////////////////
    imgGRTDataObj = new imgGRTData("test", imgGRTDataDir_);
    
    //////////////////
    // create gui for loading and modifying GRT data files
    //////////////////
    gui = new ofxUISuperCanvas("GRTEditor");
    gui -> setWidth(400);
    gui -> addSpacer();
    //
    // select GRT image set to modify from list of GRT XML files
    // text field with currently displayed XML baseName: includes a slider to navigate list quickly, as well as buttons & keybindings to navigate up and down one image at a time
    gui -> addTextArea("current image set baseName:", "TBU");
    //    - slider like image editor
    //    - update list button
    //    - string input to specify baseName for a new set of GRT & XML files
    //        - default could be auto-generated based on tags as well——ssave directory should be specified on an application level
    //        - create new GRT button
    //            - gets file name from name entry field
    //            - updates GRT XML file list
    // gui for selecting tags for images that should be made into a single GRT file
    //    - update tags list button
    //    - push new tags to object as they are selected
    //    - get tags from GRT XML file as well
    // gui for selecting data type for each joint (abs, rel, axis angle)
    // save imgGRTData XML file
    //    - GRTfileSavePath
    //    - imgPaths
    //    - tagSettings
    //    - jointDataFormat
    // build and save imgGRTData GRT file
    //    — unique entries for each training data point for each image, including a copy of the same user-specified joints in each entry
    //
    // save imgSet data button
    ofxUILabelButton *saveImgDataButton = gui -> addLabelButton("'d' save image set data", false);
    saveImgDataButton -> bindToKey('d');
    saveImgDataButton -> bindToKey('D');
    gui -> addSpacer();
    //
    // save GRTEditor gui settings button
    ofxUILabelButton *saveImgEditorSettingsButton = gui -> addLabelButton("'s' save GRTEditor settings", false);
    saveImgEditorSettingsButton -> bindToKey('s');
    saveImgEditorSettingsButton -> bindToKey('S');
    //
    // closing operations
    gui -> autoSizeToFitWidgets();
    ofAddListener(gui -> newGUIEvent, this, &GRTEditor::guiEvent);
    // load settings
    gui -> loadSettings(guiSettingsPath);
    
    initializing = false;
}

void GRTEditor::exit() {
    delete imgGRTDataObj; imgGRTDataObj = NULL;
}


//// text field with currently displayed image baseName: includes a slider to navigate list quickly, as well as buttons & keybindings to navigate up and down one image at a time
//gui -> addTextArea("current image baseName", "current image: " + it->first);
//gui -> addSlider("imagePathMap index", 1.0f, imagePathMap.size(), &currentImgIndexFloat);
//ofxUIButton *buttonPrevImg = gui -> addButton("'[' previous image", false);
//buttonPrevImg -> bindToKey('[');
//buttonPrevImg -> bindToKey('{');
//ofxUIButton *buttonNextImg = gui -> addButton("']' next image", false);
//buttonNextImg -> bindToKey(']');
//buttonNextImg -> bindToKey('}');
//gui -> addSpacer();
////
//// update list button
//ofxUILabelButton *buttonUpdateImgList = gui -> addLabelButton("'u' update image list", false);
//buttonUpdateImgList -> bindToKey('u');
//buttonUpdateImgList -> bindToKey('U');
//gui -> addSpacer();
////
//// tracked user radio
//gui -> addLabel("'<' '>' user being tracked");
//vector<string> trkdUserV;
//trkdUserV.push_back("0"); trkdUserV.push_back("1"); trkdUserV.push_back("2"); trkdUserV.push_back("3"); trkdUserV.push_back("4"); trkdUserV.push_back("5"); trkdUserV.push_back("6"); trkdUserV.push_back("7"); trkdUserV.push_back("<"); trkdUserV.push_back(">");
//ofxUIRadio *trkdUserRadio = gui -> addRadio("traked user", trkdUserV, OFX_UI_ORIENTATION_HORIZONTAL);
//trkdUserRadio -> getEmbeddedWidget(8) -> bindToKey(',');
//trkdUserRadio -> getEmbeddedWidget(8) -> bindToKey('<');
//trkdUserRadio -> getEmbeddedWidget(9) -> bindToKey('.');
//trkdUserRadio -> getEmbeddedWidget(9) -> bindToKey('>');
//trkdUserRadio -> activateToggle(ofToString(trackedUserIndex));
//// capture user data button
//ofxUILabelButton *captureUsrJntsButton = gui -> addLabelButton("'b' capture user joints", false);
//captureUsrJntsButton -> bindToKey('b');
//captureUsrJntsButton -> bindToKey('B');
//gui -> addSpacer();
////
//// select user data radio (add & remove embedded toggles)
//gui -> addLabel("';' ''' captured data joints");
//guiJntDataTglMtx = gui -> addToggleMatrix("trained data joints", 1, guiJntDataTglMtxSize);
//guiJntDataTglMtx -> bindToKey(';');
//guiJntDataTglMtx -> bindToKey(':');
//guiJntDataTglMtx -> bindToKey('\'');
//guiJntDataTglMtx -> bindToKey('"');
//guiJntDataTglMtx -> bindToKey('A');
//guiJntDataTglMtx -> setAllowMultiple(true);
//guiJntDataTglMtxTgls = guiJntDataTglMtx -> getToggles();
//for (int i = 0; i < guiJntDataTglMtxTgls.size(); ++i) {
//    guiJntDataTglMtxTgls[i] -> setVisible(false);
//}
//// transfer data to joints from selected data set (grabs first one fornow, would be nice to do an axis-angle average at some point)
//ofxUILabelButton *setJointsFromTrainingJointSetButton = gui -> addLabelButton("'g' set training joints from first selected data set", false);
//setJointsFromTrainingJointSetButton -> bindToKey('g');
//setJointsFromTrainingJointSetButton -> bindToKey('G');
//// delete selected data points
//ofxUILabelButton *deleteSelJntSetsButton = gui -> addLabelButton("'r' remove selected data joints", false);
//deleteSelJntSetsButton -> bindToKey('r');
//// delete data points that are un-selected
//ofxUILabelButton *deleteNonSelJntSetsButton = gui -> addLabelButton("'R' remove UN-selected data joints", false);
//deleteNonSelJntSetsButton -> bindToKey('R');
//gui -> addSpacer();
////
//// image scale update buttons
//ofxUIButton *buttonScaleDown = gui -> addButton("'-' scale down image", false);
//buttonScaleDown -> bindToKey('-');
//buttonScaleDown -> bindToKey('_');
//ofxUIButton *buttonScaleUp = gui -> addButton("'+' scale up image", false);
//buttonScaleUp -> bindToKey('=');
//buttonScaleUp -> bindToKey('+');
//gui -> addSpacer();


//--------------------------------------------------------------
void GRTEditor::guiEvent(ofxUIEventArgs &e) {
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
        //        if (e.widget -> getParent() -> getName() == "trained data joints") upatedTrnDataVisibilty();
    }
    // register button release only when mouse buttons are not pressed and application is not initializing
    //    if (!buttonPressed && !initializing && !ofGetMousePressed()) buttonReleased = true;
    
    
    
    /*  */ if(nameStr == "'[' previous image set"){
        //        if (currentImgIndex > 1) {
        //            if (buttonPressed) currentImgIndexFloat -= 1.0f;
        //        }
    } else if(nameStr == "']' next image set") {
        //        if (currentImgIndex < imagePathMap.size()) {
        //            if (buttonPressed) currentImgIndexFloat += 1.0f;
        //        }
    } else if(nameStr == "'u' update image set list") {
        //        if (buttonReleased) {
        //            mapAllImages();
        //        }
        
        //
        //    } else if (nameStr == "tracked user" ||nameStr=="0"||nameStr=="1"||nameStr=="2"||nameStr=="3"||nameStr=="4"||nameStr=="5"||nameStr=="6"||nameStr=="7"||nameStr=="<"||nameStr==">") {
        //        //
        //        if (kind == OFX_UI_WIDGET_RADIO) {
        //            radio = (ofxUIRadio*)e.widget;
        //        } else {
        //            radio = (ofxUIRadio*)e.widget->getParent();
        //        }
        //        //
        //        int i = radio -> getValue();
        //        if (i!= trackedUserIndex)
        //            trackedUserIndex =
        //            (i>=0 && i<=7) * i
        //            + (trackedUserIndex-1>=0 && i==8) * (trackedUserIndex-1)
        //            + (trackedUserIndex+1<=7 && i==9) * (trackedUserIndex+1)
        //            + (trackedUserIndex==7&&i==9) * trackedUserIndex
        //            ;
        //        //
        //        radio -> activateToggle(ofToString(trackedUserIndex));
        //    } else if (nameStr == "'b' capture user joints") {
        //        if (buttonReleased) {
        //            int trnDataSize = imgDataObj -> getTrnDataSize();
        //            if (trnDataSize < guiJntDataTglMtxSize) {
        //                // store training joint positions imgDataObj
        //                imgDataObj -> pushTrnData(joints);
        //                // update "';' ''' captured data joints" gui to reflect an added data point
        //                guiJntDataTglMtxTgls[imgDataObj -> getTrnDataSize()-1] -> toggleVisible();
        //                guiJntDataTglMtx -> setAllToggles(false);
        //                guiJntDataTglMtxTgls[imgDataObj -> getTrnDataSize()-1] -> toggleValue();
        //            } else {
        //                cout << "imgEditor:: guiEvent(ofxUIEventArgs &e) -- already trained maximum training data sets = " << ofToString(guiJntDataTglMtxTgls.size()) << endl;
        //                cout << " -- delete some training data sets for this image in order to create new ones." << endl ;
        //            }
        //            upatedTrnDataVisibilty();
        //        }
        //    } else if (nameStr == "'g' set training joints from first selected data set") {
        //        if (buttonReleased) {
        //            // cycle through toggles from start and transfer data from first selected toggle set to imgData joints
        //            int tgl;
        //            for (tgl = 0; tgl < imgDataObj -> getTrnDataSize(); ++tgl) {
        //                if (guiJntDataTglMtxTgls[tgl] -> getValue()) {
        //                    imgDataObj -> setJointsFromTrainingJointSet(tgl);
        //                    break;
        //                }
        //            }
        //            // reset all toggles
        //            guiJntDataTglMtx -> setAllToggles(false);
        //            guiJntDataTglMtxTgls[tgl] -> toggleValue();
        //            upatedTrnDataVisibilty();
        //        }
        //    } else if (nameStr == "'r' remove selected data joints") {
        //        if (buttonReleased) {
        //            // cycle through toggles from end to start and erase data for selected sets
        //            for (int tgl = imgDataObj -> getTrnDataSize()-1; tgl >= 0; --tgl) {
        //                if (guiJntDataTglMtxTgls[tgl] -> getValue()) {
        //                    imgDataObj -> eraseTrnData(tgl);
        //                    guiJntDataTglMtxTgls[imgDataObj -> getTrnDataSize()] -> toggleVisible();
        //                }
        //            }
        //            // reset all toggles
        //            guiJntDataTglMtx -> setAllToggles(false);
        //            upatedTrnDataVisibilty();
        //        }
        //    } else if (nameStr == "'R' remove UN-selected data joints") {
        //        if (buttonReleased) {
        //            // cycle through toggles from end to start and erase data for unselected sets
        //            for (int tgl = imgDataObj -> getTrnDataSize()-1; tgl >= 0; --tgl) {
        //                if (!guiJntDataTglMtxTgls[tgl] -> getValue()) {
        //                    imgDataObj -> eraseTrnData(tgl);
        //                    guiJntDataTglMtxTgls[imgDataObj -> getTrnDataSize()] -> toggleVisible();
        //                }
        //            }
        //            // reset all toggles
        //            guiJntDataTglMtx -> setAllToggles(false);
        //            // re-select all visible toggles
        //            for (int tgl = 0; tgl< imgDataObj -> getTrnDataSize(); ++tgl) {
        //                guiJntDataTglMtxTgls[tgl] -> toggleValue();
        //            }
        //            upatedTrnDataVisibilty();
        //        }
        //
        //
        //    } else if (nameStr == "'-' scale down image" || nameStr == "'+' scale up image") {
        //        if (buttonReleased) {
        //            int sign = -1;
        //            float scaleAmt = 0.05f; // five percent
        //            if (nameStr == "'+' scale up image") sign *= sign;
        //            // scale more when shift key is pressed
        //            bool rightShft = ofGetKeyPressed(OF_KEY_RIGHT_SHIFT);
        //            bool leftShft = ofGetKeyPressed(OF_KEY_LEFT_SHIFT);
        //            bool shft = ofGetKeyPressed(OF_KEY_SHIFT);
        //            bool keysPressed = ofGetKeyPressed();
        //            cout << "imgEditor::guiEvent()  -- rightShftPressed = " << rightShft << endl;
        //            cout << "                       -- leftShftPressed = " << leftShft << endl;
        //            cout << "                       -- shftPressed = " << shft << endl;
        //            cout << "                       -- keyPressed = " << keysPressed << endl;
        //            if (rightShft || leftShft) { // TODO: guiEvent -- scale image -- debug SHIFT keypress checks which don't seem to be working.
        //                scaleAmt *= 10.0f;
        //            }
        //            imgDataObj->scaleImages(1+ sign * scaleAmt);
        //        }
        //
        
    } else if (nameStr == "'d' save image set data") {
        if (buttonReleased) {
            imgGRTDataObj->save();
        }
        
        
    } else if (nameStr == "'s' save GRTEditor settings") {
        if (buttonReleased) {
            gui->saveSettings(guiSettingsPath);
            cout << "GRTEditor::guiEvent() -- guiSettingsPath = " << guiSettingsPath << endl;
        }
    }else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE)
            cout << "[verbose] imgLoader::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
}



//--------------------------------------------------------------
void GRTEditor::update() {
    
}

//--------------------------------------------------------------
void GRTEditor::draw() {
    
}

//--------------------------------------------------------------
void GRTEditor::keyPressed(int key){
    
    ////    // process key pressed
    ////    switch (key) {
    ////
    ////        case 'h':
    ////            gui->toggleVisible();
    ////            break;
    ////
    ////        case 'b': // NOTE: updated to 'b' for BUILD DATA
    ////            if (displayState == 'i') break; // do not train data during installation mode
    ////
    ////            // Store data to associate with currently displayed image
    ////            //  - joint positions (0–15) — 0 = center, 1–15 = joints
    ////
    ////            if ((displayState == 'd' || displayState == 't') && trackedUserJointsPosABS.size()){
    ////                trainingDataJointsPosABS.addSample(label, trackedUserJointsPosABSDouble[0]);
    ////                trainingDataJointsPosRel.addSample(label, trackedUserJointsPosRelDouble[0]);
    ////                trainingDataJointsRotAxisA.addSample(label, trackedUserJointsRotAxisADouble[0]);
    ////            } else {
    ////                // TODO: display some kind of error message that says data can only be saved in training mode?
    ////            }
    ////
    ////
    ////        case 's': // NOTE: Moved save functionality here to minimize lagging during data building phase
    ////            if (displayState == 'i') break; // do not train data during installation mode
    ////
    ////            GRTMngr->saveData();
    ////            GRTMngr->saveModel();
    ////
    ////            break;
    ////
    
}

//--------------------------------------------------------------
bool GRTEditor::loadData() {
    //    /////////////////////////////////////////
    //    // load training data and setup models //
    //    /////////////////////////////////////////
    //    trainingDataJointsPosABSfileName    = trainedImagesDirectory + "JointsPosABSdata.txt"; // TODO: make these relative to selected directories
    //    trainingModelJointsPosABSfileName   = trainedImagesDirectory + "JointsPosABSmodel.txt";
    //    trainingDataJointsPosRelfileName    = trainedImagesDirectory + "JointsPosReldata.txt";
    //    trainingModelJointsPosRelfileName   = trainedImagesDirectory + "JointsPosRelmodel.txt";
    //    trainingDataJointsRotAxisAfileName  = trainedImagesDirectory + "JointsRotAxisAdata.txt";
    //    trainingModelJointsRotAxisAfileName = trainedImagesDirectory + "JointsRotAxisAmodel.txt";
    //    //
    //    trainingDataJointsPosABS.setDatasetName("harlequinPosABS");
    //    trainingDataJointsPosABS.setNumDimensions(45);
    //    trainingDataJointsPosRel.setDatasetName("harlequinPosRel");
    //    trainingDataJointsPosRel.setNumDimensions(45);
    //    trainingDataJointsRotAxisA.setDatasetName("harlequinRotAxisA");
    //    trainingDataJointsRotAxisA.setNumDimensions(45);
    //    //
    //    //
    //    trainingDataJointsPosABS.loadDatasetFromFile(ofToDataPath(trainingDataJointsPosABSfileName));
    //    trainingDataJointsPosRel.loadDatasetFromFile(ofToDataPath(trainingDataJointsPosRelfileName));
    //    trainingDataJointsRotAxisA.loadDatasetFromFile(ofToDataPath(trainingDataJointsRotAxisAfileName));
    //
    //    trainModelsNow = true;
    //
    //    // TODO: implement the suggested pipeline setup
    //    GRT::SVM trainingModelJointsPosABS(GRT::SVM::LINEAR_KERNEL);
    //    GRT::SVM trainingModelJointsPosRel(GRT::SVM::LINEAR_KERNEL);
    //    GRT::SVM trainingModelJointsRotAxisA(GRT::SVM::LINEAR_KERNEL);
    
    cout << "GRTEditor::loadData() -- loaded training data" << endl;
    return true;
}

//--------------------------------------------------------------
bool GRTEditor::saveData(){
    //    trainingDataJointsPosABS.saveDatasetToFile(ofToDataPath(trainingDataJointsPosABSfileName));
    //    trainingDataJointsPosRel.saveDatasetToFile(ofToDataPath(trainingDataJointsPosRelfileName));
    //    trainingDataJointsRotAxisA.saveDatasetToFile(ofToDataPath(trainingDataJointsRotAxisAfileName));
    
    cout << "GRTEditor::saveData() -- saved training data" << endl;
    return true;
}


//--------------------------------------------------------------
bool GRTEditor::loadModel() {
    
    //    // TODO: loading models only works when called outside startup()——could be fixed by implementing the suggested pipeline setup?
    //    trainingModelJointsPosABS.loadModelFromFile(ofToDataPath(trainingModelJointsPosABSfileName));
    //    trainingModelJointsPosRel.loadModelFromFile(ofToDataPath(trainingModelJointsPosRelfileName));
    //    trainingModelJointsRotAxisA.loadModelFromFile(ofToDataPath(trainingModelJointsRotAxisAfileName));
    //
    //    trainModelsNow = false;
    
    cout << "GRTEditor::loadModel() -- loaded training model" << endl;
    return true;
}

//--------------------------------------------------------------
bool GRTEditor::trainModel() {
    
    // TODO: training model data only works when called outside startup()——could this also be fixed by implementing the suggested pipeline setup?
    //    trainingModelJointsPosABS.train(trainingDataJointsPosABS);
    //    trainingModelJointsPosRel.train(trainingDataJointsPosRel);
    //    trainingModelJointsRotAxisA.train(trainingDataJointsRotAxisA);
    //
    //    trainModelsNow = false;
    
    cout << "GRTEditor::trainModel() -- trained model" << endl;
    return true;
}

//--------------------------------------------------------------
bool GRTEditor::saveModel() {
    //    trainingModelJointsPosABS.train(trainingDataJointsPosABS);
    //    trainingModelJointsPosABS.saveModelToFile(ofToDataPath(trainingModelJointsPosABSfileName));
    //
    //    trainingModelJointsPosRel.train(trainingDataJointsPosRel);
    //    trainingModelJointsPosRel.saveModelToFile(ofToDataPath(trainingModelJointsPosRelfileName));
    //
    //    trainingModelJointsRotAxisA.train(trainingDataJointsRotAxisA);
    //    trainingModelJointsRotAxisA.saveModelToFile(ofToDataPath(trainingModelJointsRotAxisAfileName));
    cout << "GRTEditor::saveModel() -- saved training model" << endl;
    return true;
}

void GRTEditor::mapAllGRTData(string imageJntDataDir_, string imgsDir_) {
    
    // debug
    cout << "GRTEditor::mapAllGRTData() -- called" << endl;
    
    // TODO: clear properties if they were already initialized
    // TODO: initialize a list of GRT XML files to scrub through
    
    // compile a list of image data XMLs
    ofDirectory imgJntDataDir(imageJntDataDir_);
    vector<string> imgJntDataXMLPaths;
    imgJntDataXMLPaths = listFilesOfType(imgJntDataDir, "xml");
    // create a map of image paths to their respective imgDataObj
    for(int pathIndx = 0; pathIndx < imgJntDataXMLPaths.size(); ++pathIndx) {
        // create imgdData object
        imgData* imgDataObj = new imgData();
        // load its img XML data
        imgDataObj->open(imgJntDataXMLPaths[pathIndx]);
        // geat a list of its image paths
        vector< string > imgPaths = imgDataObj -> getImgPaths();
        // map each image path to its corresponding imgData object
        for(int i = 0; i < imgPaths.size(); ++i) {
            imgPathDataMap[imgPaths[i]] = imgDataObj;
        }
    }
    
    // split all image path strings and compile tags maps
    imgsDirPath = ofToDataPath(imgsDir_);
    for(map< string, imgData* >::iterator it = imgPathDataMap.begin(); it!= imgPathDataMap.end(); ++it) {
        // truncate path name
        string imgPath = ofToDataPath(it->first);
        ofFile imgFile(imgPath);
        string imgParentPath = ofToDataPath(imgFile.getEnclosingDirectory());
        cout << "GRTEditor::setup() -- imgParentPath = " << imgParentPath << endl;
        cout << "                   -- imgsDirPath = " << imgsDirPath << endl;
        ofStringReplace(imgParentPath, imgsDirPath, "");
        cout << "                   -- truncated imgPath = " << imgParentPath << endl;
        // split trunkated directory into tags
        vector< string > imgPathSplit = ofSplitString(imgParentPath, "/");
        if (imgPathSplit.size() == 1) { // try backslash delimiter
            imgPathSplit = ofSplitString(imgPathSplit[0], "\\");
            cout << "GRTEditor::setup() -- forward slash '/' didn't work as split delimiter. Trying backslash '\\'" << endl;
        }
        // add each enclosing directory to the tagImgPathMap
        for(int i = 0; i < imgPathSplit.size(); ++i) {
            string pathTag = imgPathSplit[i];
            cout << "GRTEditor::setup() -- pathTag = " << pathTag << endl;
            if (pathTag != "") tagImgPathMap[pathTag].push_back(imgPath);
        }
        // split file's baseName
        string imgBaseName = imgFile.getBaseName();
        cout << "                   -- imgBaseName = " << imgBaseName << endl;
        vector< string > imgBaseNameSplit = ofSplitString(imgBaseName, "_");
        // add a subselection of fileName tags to tags map
        for(int i = 0; i < imgBaseNameSplit.size(); ++i) {
            string imgNameTag = imgBaseNameSplit[i];
            cout << "GRTEditor::setup() -- imgNameTag = " << imgNameTag << endl;
            int imgNmTgInt = ofToInt(imgNameTag);
            // cout << "                   -- imgNmTgInt = " << imgNmTgInt << endl;
            if ((imgNameTag != "" && imgNameTag.length() != 4 && imgNameTag.length() != 8) || !imgNmTgInt) {
                tagImgPathMap[imgPath].push_back(imgNameTag);
            } else {
                // do nothing - ignore tages with dates and sequence numbers (i.e.: "00000000" through "99999999" and "0000" through "9999")
                cout << "                   -- ignoring this tag because it is a date or series number" << endl;
            }
        }
        
    }
    
    //    // update editor variables based on new map dimensions
    //    if (imagePathMap.size()){
    //        it = imagePathMap.begin();
    //        currentImgIndex = 0;
    //    } else {
    //        it = imagePathMap.end();
    //        currentImgIndex = 0;
    //        currentImgIndexFloat = 0.0f;
    //        currentImgBaseName = "";
    //        cout << "GRTEditor::mapAllImages() -- no images found in the 'data/images' directory" << endl;
    //    }
}

//--------------------------------------------------------------
vector<string> GRTEditor::listFilesOfType(ofDirectory dir_, string ext_, vector<string> fileList_) {
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
        }else if (fileExt == ext_ || ext_ == "") {
            fileList_.push_back(filePath);
            // debug
            // cout << "GRTEditor::listFilesOfType() -- filePath = " << filePath << endl;
        }
    }
    
    return fileList_;
}
