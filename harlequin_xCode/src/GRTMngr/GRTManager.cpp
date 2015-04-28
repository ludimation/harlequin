//
//  GRTEditor.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "GRTManager.h"

//--------------------------------------------------------------
void GRTManager::setup() {
    
    gui = new ofxUISuperCanvas("GRTManager");
    
    // creator
    // compile a list of directories that have harlequin image data in them
    // create gui for selecting which ones should be made into a single GRT file
    // speficify filename and path for GRT & XML files
    
}

//--------------------------------------------------------------
void GRTManager::update() {
    
}

//--------------------------------------------------------------
void GRTManager::draw() {
    
}

////--------------------------------------------------------------
//void GRTManager::keyPressed(int key){
//    
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
//}

//--------------------------------------------------------------
void GRTManager::guiEvent(ofxUIEventArgs &e) {
    string nameStr = e.widget->getName();
    int kind = e.widget->getKind();
    
    /*  */ if(nameStr == "load model"){
        // do something
    } else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE) cout << "[verbose] imgLoader::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
}


//--------------------------------------------------------------
bool GRTManager::loadData() {
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
bool GRTManager::saveData(){
    //    trainingDataJointsPosABS.saveDatasetToFile(ofToDataPath(trainingDataJointsPosABSfileName));
    //    trainingDataJointsPosRel.saveDatasetToFile(ofToDataPath(trainingDataJointsPosRelfileName));
    //    trainingDataJointsRotAxisA.saveDatasetToFile(ofToDataPath(trainingDataJointsRotAxisAfileName));
    
    cout << "GRTEditor::saveData() -- saved training data" << endl;
    return true;
}


//--------------------------------------------------------------
bool GRTManager::loadModel() {
    
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
bool GRTManager::trainModel() {
    
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
bool GRTManager::saveModel() {
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

//--------------------------------------------------------------
vector<string> listFilesOfType(ofDirectory dir_, string ext_ = "", vector<string> fileList_ = vector<string>()) {
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
            cout << "dirUtils » listFilesOfType() -- filePath = " << filePath << endl;
        }
    }
    
    return fileList_;
}
