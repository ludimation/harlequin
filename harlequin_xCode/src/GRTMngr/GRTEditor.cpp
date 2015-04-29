//
//  GRTEditor.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "GRTEditor.h"

//--------------------------------------------------------------
void GRTEditor::setup(string guiSettingsPath_, string imgsDir_, string imageJntDataDir_, string imageTagDataDir_, string imageGRTDataDir_) {
    
    gui = new ofxUISuperCanvas("GRTEditor");
    
    //////////////
    // creator
    //////////////
    // compile a list of image data XMLs
    ofDirectory imgJntDataDir(imageJntDataDir_);
    vector<string> imgJntDataXMLPaths;
    imgJntDataXMLPaths = listFilesOfType(imgJntDataDir, "xml");
    // create a map of image paths to their respective imgDataObj
    map< string, imgData* > imgPathDataMap; // <imgPath, imgDataObject pointer>
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
    map< string, vector< string > > tagImgPathMap; // map< tagName, map<on/off, vector< imgPaths > > >
    string imgsDirPath = ofToDataPath(imgsDir_);
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
    
    // create an imgGRTData object
    //    - string baseName
    //    - string GRTfileSavePath
    //    - string XMLFileSavePath
    //    - map< string, bool > tagSettings
    //    - vector< string > imgPaths // index is GRT label number
    //    - vector< jointDataTypes > jntDataformat
    
    // gui for selecting tags for images that should be made into a single GRT file
    // gui for selecting data type for each joint (abs, rel, axis angle)
    // string to specify baseName for both GRT & XML files (could be auto-generated based on tags as well——ssave directory should be specified on an application level)
    // save imgGRTData XML file
    //    - GRTfileSavePath
    //    - imgPaths
    //    - tagSettings
    //    - jointDataFormat
    // build and save imgGRTData GRT file
    //    — unique entries for each training data point for each image, including a copy of the same user-specified joints in each entry
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
////            break;
////            
////        case '<':
////        case ',':
////        case '[':
////        case 'p': // previous
////            
////            GRTMngr->saveData();
////            
////            if (displayState == 'i') break; // do not train data during installation mode
////            
////            // display previous image in database
////            if (label > 0) label--;
////            //            img_name = imageNames[label];
////            
////            break;
////            
////        case '>':
////        case ']':
////        case 'n': // next
////            
////            GRTMngr->saveData();
////            
////            if (displayState == 'i') break; // do not train data during installation mode
////            
////            // display next image in database
////            if (label < images.size()-1) label++;
////            //            img_name = imageNames[label];
////            
////            break;
////            
////        case 'r': // random image
////            
////            GRTMngr->saveData();
////            
////            if (displayState == 'i') break; // do not train data during installation mode
////            
////            // display random image from database
////            label = bodyClass->getRandomExcluding(0, images.size() - 1, label);
////            //            img_name = imageNames[label];
////            
////            break;
////            
////        case 's': // NOTE: Moved save functionality here to minimize lagging during data building phase
////            if (displayState == 'i') break; // do not train data during installation mode
////            
////            GRTMngr->saveData();
////            GRTMngr->saveModel();
////            
////            break;
////            
////        case 'i': // interactive mode
////        case 't': // training
////        case 'd': // debug
////            
////            displayStateKeyed = true;
////            break;
////            
////        case '=': // increase drawFrameRate
////            // fall through (intentional)
////        case '+': // increase drawFrameRate
////            
////            if (drawFrameRate < 90) drawFrameRate = drawFrameRate + 5;
////            break;
////            
////        case '_': // decrease drawFrameRate
////            // fall through (intentional)
////        case '-': // decrease drawFrameRate
////            
////            if (drawFrameRate > 5) drawFrameRate = drawFrameRate - 5;
////            break;
////            
////        case 'm':
////            
////            drawMirrored = !drawMirrored;
////            openNIPlayer.setMirror(drawMirrored);
////            break;
////            
////        case 'x':
////            
////            kinected = kinectInterface.stopKinects();
////            break;
////            
////        case 'k':
////            kinectsInitialized = kinectInterface.setupKinects(drawMirrored);
////            // TODO: proper implementation of stopping kinects and starting kinects again. doesn't seem to work properly after kinects have been stopped.
////            kinected = kinectsInitialized;
////            break;
////            
////        case 'f':
////            ofToggleFullscreen();
////            windowResized();
////            break;
////            
////        default:
////            cout << "Unrecognized key press = " << key << endl;
////            // TODO: plan error sound when receiving unrecognized key presses.
////            break;
////    }
////    
////    if (displayStateKeyed && key != displayState)
////    {
////        switch(key){
////            case 'i': // interactive mode
////                
////                // TODO: clean out displayStateString code. nothing seems to reference it.
////                displayStateString = "interactive";
////                break;
////                
////            case 't': // training
////                
////                displayStateString = "training";
////                break;
////                
////            case 'd': // debug
////                
////                displayStateString = "debug";
////                break;
////        }
////        
////        setDisplayState(key);
////    }
}

//--------------------------------------------------------------
void GRTEditor::guiEvent(ofxUIEventArgs &e) {
    string nameStr = e.widget->getName();
    int kind = e.widget->getKind();
    
    /*  */ if(nameStr == "load model"){
        // do something
    } else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE) cout << "[verbose] imgLoader::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
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

void GRTEditor::mapAllImageData() {
    
//    // debug
//    cout << "imgEditor::mapAllImages() -- called" << endl;
//    
//    // clear the map it if has already been populated
//    if (imagePathMap.size()) imagePathMap.clear();
//    
//    // get recursive list of jpg files in images directory
//    string path = ofToDataPath(imagesDirectory);
//    string ext = "jpg";
//    ofDirectory dir(path);
//    //    ofDirUtils dirUtils;
//    //    vector<string> fileListing = dirUtils.listFilesOfType(dir, ext);
//    vector<string> fileListing = listFilesOfType(dir, ext);
//    
//    // pupulate map
//    if (fileListing.size()) {
//        for (int i = 0; i < fileListing.size(); ++i) {
//            string filePath = fileListing[i];
//            string fileBaseName = ofFile(filePath).getBaseName();
//            
//            addImgToPathMap(fileBaseName, filePath);
//            
//            // debug
//            // cout << "imgEditor::mapAllImages() -- fileBaseName = " << fileBaseName << endl;
//            // cout << "                          -- filePath = " << filePath << endl;
//            
//        }
//    }
//    
//    
//    // update editor variables based on map dimensions
//    if (imagePathMap.size()){
//        it = imagePathMap.begin();
//        currentImgIndex = 0;
//    } else {
//        it = imagePathMap.end();
//        currentImgIndex = 0;
//        currentImgIndexFloat = 0.0f;
//        currentImgBaseName = "";
//        cout << "imgEditor::mapAllImages() -- no images found in the 'data/images' directory" << endl;
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
