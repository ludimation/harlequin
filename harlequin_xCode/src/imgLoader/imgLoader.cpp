//
//  imgLoader.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "imgLoader.h"


void imgLoader::setup() {
    
    // loader
    // compile a list of available GRT & XML models
    // gui for selecting which ones to load
    // interface for grabbing a reference to an image based on filename
    
    // image loader // items below should eventually be handled by image loader
    string trainedImagesDirectory = "images/hand_drawings/LT/";
    //    trainedImagesDirectory = "images/hand_drawings/V/";
    // TODO: make image directory selection dynamic (1. include text input in GUI, 2. separate out data loading into a function that can be called when target data set is changed)
    directoriesAll.push_back(trainedImagesDirectory + "_540"); // i = 0
    directoriesAll.push_back(trainedImagesDirectory + "_1080"); // i = 1
    //
    // declare local variables
    string          directoryPath;
    ofDirectory     dir;
    int             nFiles;
    //
    directoryPath = directoriesAll[0]; // loading _540 images
    nFiles = dir.listDir(directoryPath);
    maxFilesToLoad = dir.size();
    nFilesToLoad = 64; // for testing purposes only (quick load)
    
    
    /////////
    // gui //
    /////////
    gui = new ofxUISuperCanvas("imgLoader");
    ofAddListener(gui -> newGUIEvent, this, &imgLoader::guiEvent);
    gui -> addSpacer();
    //
    // Load files // move these to imgLoader
    gui -> addIntSlider("number of files to load", 0, maxFilesToLoad, &nFilesToLoad);
    gui -> addLabelToggle("load images", &loadImagesNow);
    gui -> addSpacer();
    //
    // Save Settings
    gui -> addLabelButton("save imgLoader settings", false);
    gui -> autoSizeToFitWidgets();
}

//--------------------------------------------------------------
void imgLoader::guiEvent(ofxUIEventArgs &e) {
    string nameStr = e.widget->getName();
    int kind = e.widget->getKind();
    
    /*  */ if(nameStr == "load images"){
        loadImages(loadImagesNow);
    } else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE) cout << "[verbose] imgLoader::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
}

//--------------------------------------------------------------
void imgLoader::loadImages(bool load) {
    
    if (load) {
        
//        // declare local variables
//        string          directoryPath;
//        string          filePath;
//        ofDirectory     dir;
//        int             nFilesInDir;
//        int             nFilesLoaded = images.size();
//        ofImage         imgTMP;
//        //
//        // initialize
//        directoryPath = directoriesAll[0];
//        nFilesInDir = dir.listDir(directoryPath);
//        maxFilesToLoad = dir.size();//TODO: update maximum for GUI slider named "number of files to load"
//        images.resize(nFilesToLoad);
//        imagesPTRs.resize(nFilesToLoad);
//        //        imageNames.resize(nFilesToLoad);
//        imgTMP.setCompression(OF_COMPRESS_ARB); // OF_COMPRESS_NONE || OF_COMPRESS_SRGB || OF_COMPRESS_ARB
//        //
//        // load files
//        if(nFilesInDir) {
//            for(int i = nFilesLoaded; i < maxFilesToLoad; ++i) {
//                if (nFilesLoaded >= nFilesToLoad) break;
//                
//                ofImage*        imgTMPptr;
//                imgTMPptr = new ofImage();
//                imgTMPptr->setCompression(OF_COMPRESS_ARB); // OF_COMPRESS_NONE || OF_COMPRESS_SRGB || OF_COMPRESS_ARB
//                
//                // add the image name to a list
//                filePath = dir.getPath(i);
//                if (imgTMP.loadImage(filePath))
//                {
//                    //                    imageNames[nFilesLoaded] = filePath;
//                    //                    if (imgInvertColors) {
//                    //                        invertImage(imgTMP); // TODO: invertImage(imgTMP) doesn't seem to be working
//                    //                    }
//                    images[nFilesLoaded] = imgTMP;
//                    //                    nFilesLoaded++;
//                    cout << "loaded image [" << ofToString(nFilesLoaded) << "/" << ofToString(nFilesToLoad) << "] : " << filePath << endl;
//                }
//                
//                if (imgTMPptr->loadImage(filePath))
//                {
//                    //                    imageNames[nFilesLoaded] = filePath;
//                    //                    if (imgInvertColors) {
//                    //                        invertImage(imgTMPptr); // TODO: invertImage(imgTMPptr) doesn't seem to be working
//                    //                    }
//                    //                    imgTMPptr->update();
//                    //                    imagesPTRs[nFilesLoaded] = imgTMPptr;
//                    //                    imagesPTRs[nFilesLoaded] = bikersPTR;
//                    //                    imagesMap[filePath] = bikersPTR;
//                    //                    if (imagesMap.find(filePath) != imagesMap.end()) imagesMap[filePath] = imgTMPptr;
//                    //                    if (imagesMap.find(filePath) != imagesMap.end()) imagesMap[filePath] = imgTMPptr;
//                    nFilesLoaded++;
//                    cout << "loaded image PTR [" << ofToString(nFilesLoaded) << "/" << ofToString(nFilesToLoad) << "] : " << filePath << endl;
//                }
//                
//            }
//            //
//            // imgColorsAreInverted = imgInvertColors;
//            
//        } else cout << "Could not find \"" << ofToString(directoryPath) << " directory\n" << endl;
//        //
//        // Select image to start with
//        label = 0;
//        if (imageNames.size()) img_name = imageNames[label];
//        // TODO: use "img_name" to stream images from HD (can set a global vairable called streamFromSSD to determine whether or not to stream images every frame or use our current pre-loading method
//        // TODO: clear out the imageNames array code after the data handling re-structuring takes place
//        
//        
//        GRTEdtr.loadData();
    }
    else
    {
//        // clear image arrays to unload all images
//        //        imageNames.clear();
//        images.clear();
//        imagesPTRs.clear();
//        imagesMap.clear();
//        // TODO: clean up prediction data & models as well
//        cout << "images unloaded -- images.size() = "<< images.size() << endl;
//        cout << "images PTR -- imagesPTRs.size() = " << imagesPTRs.size() << endl;
    }
}