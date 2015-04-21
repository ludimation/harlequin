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
    
    ///////////////////////////////////
    // core openFrameworks functions //
    ///////////////////////////////////
	void setup();
	void update();
	void draw();
    void exit();
    // input
	void keyPressed(int key);
	void keyReleased(int key);
	void windowResized(int w, int h);
	void windowResized();
    
    
    //////////////////
    // harlequin UI //
    //////////////////
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
    // 
    // properties
    kinectIO                kinectInterface;
    bool                    kinectsInitialized;
    ofxOpenNI               openNIPlayer;
    ofxOpenNIUser           nTrackedUser;
    int                     nTrackedUsers;
    ofFile                  file;
    
    
    ////////////////////////////////////
    // image / training data handling //
    ////////////////////////////////////
    //
    // properties
    body                    bodyClass;
    GRTEditor               GRTEdtr;
    string                  trainedImagesDirectory;
    float                   imageScale;
    GRT::SVM                    trainingModelJointsPosABS;
    GRT::ClassificationData     trainingDataJointsPosABS;
    GRT::SVM                    trainingModelJointsPosRel;
    GRT::ClassificationData     trainingDataJointsPosRel;
    GRT::SVM                    trainingModelJointsRotAxisA;
    GRT::ClassificationData     trainingDataJointsRotAxisA;
    GRT::UINT                   label; // does this need to be global?
    GRT::UINT                   lbl; // does this need to be global?
    bool                        trainModelsNow;
    string                  trainingModelJointsPosABSfileName;
    string                  trainingDataJointsPosABSfileName;
    string                  trainingModelJointsPosRelfileName;
    string                  trainingDataJointsPosRelfileName;
    string                  trainingModelJointsRotAxisAfileName;
    string                  trainingDataJointsRotAxisAfileName;
    string                  testFileName;
    string                  testFileModelName;


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

    ////////////////////
    // image handling //
    ////////////////////
    imgEditor                   *imgEdtr;
    vector < string >           directoriesAll;
    vector < string >           directoriesToLoad;
    void                        loadImages(bool load);
    bool                        loadImagesNow;
    int                         maxFilesToLoad;
    int                         nFilesToLoad;
    void                        invertImage(ofImage* imgPTRlocal);
    void                        invertImage(ofImage &imgREF);
    void                        invertImage(ofPixels &imgPX, ofTexture &imgTEX);
    
    
    // kinect
    vector< ofPoint >           trackedUserCentersProjective;
    vector< vector< ofPoint > > trackedUserJointsPosABS;
    vector< vector< double > >  trackedUserJointsPosABSDouble;
    vector< vector< ofPoint > > trackedUserJointsPosRel;
    vector< vector< double > >  trackedUserJointsPosRelDouble;
    vector< vector< ofPoint > > trackedUserJointsRotAxisA;
    vector< vector< double > >  trackedUserJointsRotAxisADouble;
    void                        setupTestUserJoints();
    vector< vector< ofPoint > > testUserJoints;
    
    // image display
    string                  img_name;
    vector< string >        imageNames;
    vector <ofImage>        images; //TODO: delete ofImage version of images (replace with imagesPTRs).
    vector <ofImage*>       imagesPTRs;
    map<string, ofImage*>   imagesMap;
    
};

#endif


