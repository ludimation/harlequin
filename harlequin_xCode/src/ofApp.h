//
//  ofApp.h
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxOsc.h"
#include "kinectIO.h"
#include "ofxOpenNI.h"
#include "imgEditor.h"
#include "body.h"
#include "GRT.h"
#include "GRTEditor.h"

// OSC
#define HOST "localhost"
#define PORT 9000

class ofApp : public ofBaseApp {

public:
    
    //////////////////////////////
    // openFrameworks functions //
    //////////////////////////////
    void setup();
    void exit();
	void update();
	void draw();
    // input
	void keyPressed(int key);
	void keyReleased(int key);
	void windowResized(int w, int h);
	void windowResized();
    
    
    //////////////////
    // harlequin UI //
    //////////////////
    void setupGuis();
    void guiEvent(ofxUIEventArgs &e);
    void setDisplayState(char newState);
    // properties
    ofxUISuperCanvas        *gui; // GUI
    ofxUISuperCanvas        *guiColor;
    ofxOscSender            sender; // OSC
    ofTrueTypeFont          verdana; // for degug messages
    
    
    ////////////
    // kinect //
    ////////////
    // properties
    kinectIO                *kinectInterface;
    bool                    kinectsInitialized;
    
    
    ////////////////////////////////////
    // image / training data handling //
    ////////////////////////////////////
    // properties
    imgEditor               *imgEdtr;
    GRTEditor               *GRTEdtr;
    string                  trainedImagesDirectory;
    GRT::SVM                trainingModelJointsPosABS;
    GRT::ClassificationData trainingDataJointsPosABS;
    GRT::SVM                trainingModelJointsPosRel;
    GRT::ClassificationData trainingDataJointsPosRel;
    GRT::SVM                trainingModelJointsRotAxisA;
    GRT::ClassificationData trainingDataJointsRotAxisA;
    GRT::UINT               label; // does this need to be global?
    GRT::UINT               lbl; // does this need to be global?
    bool                    trainModelsNow;
    string                  trainingModelJointsPosABSfileName;
    string                  trainingDataJointsPosABSfileName;
    string                  trainingModelJointsPosRelfileName;
    string                  trainingDataJointsPosRelfileName;
    string                  trainingModelJointsRotAxisAfileName;
    string                  trainingDataJointsRotAxisAfileName;
    string                  testFileName;
    string                  testFileModelName;

    /////////////////////////////
    // image loading / display // eventually move these to imgLoader and body
    /////////////////////////////
    void                    loadImages(bool load);
    void                    invertImage(ofImage* imgPTRlocal);
    void                    invertImage(ofImage &imgREF);
    void                    invertImage(ofPixels &imgPX, ofTexture &imgTEX);
    // properties
    body                    *bodyClass;
    vector < string >       directoriesAll;
    vector < string >       directoriesToLoad;
    bool                    loadImagesNow;
    int                     maxFilesToLoad;
    int                     nFilesToLoad;
    string                  img_name;
    vector< string >        imageNames;
    vector <ofImage>        images; //TODO: delete ofImage version of images (replace with imagesPTRs).
    vector <ofImage*>       imagesPTRs;
    map<string, ofImage*>   imagesMap;
    float                   imageScale;



private:
    
    //////////////////
    // harlequin UI //
    //////////////////
    int                         drawFrameRate;
    int                         drawNextFrameMilliseconds;
    bool                        drawMirrored;
    char                        displayState;
    string                      displayStateString;
    bool                        kinected;
    // debug drawing flags
    bool                        drawDepth;
    bool                        drawDepthBehind;
    int                         depthColorMode;
    bool                        drawSkeletons;
    bool                        drawJoints2MSG;
    bool                        drawMSG;
    // OSC control
    bool                        sendOSC;
    bool                        setupOSC;
    string                      myHost;
    string                      myPort;
    // colors // could these be defined per-image?
    float                       bgRed, bgGreen, bgBlue;
    int                         imgBlendMode;
    bool                        imgInvertColors;
    bool                        imgColorsAreInverted;
    float                       imgRed, imgGreen, imgBlue, imgAlpha;
    int                         depthBlendMode;
    float                       depthRed, depthGreen, depthBlue, depthAlpha;
    int                         skelBlendMode;
    float                       skelRed, skelGreen, skelBlue, skelAlpha;
    
};

#endif


