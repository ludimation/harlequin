#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "GRT.h"
#include "ofxUI.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 9000

class testApp : public ofBaseApp {

public:
    
	void setup();
	void update();
	void draw();
    void exit();
    
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void windowResized();
    
    void setupKinects();
    void stopKinects();
    int  getRandomExcluding(int min, int max, int i);
    
    void setDisplayState(char newState);
    
    void saveData();
    void saveModel();

    bool                    kinectsInitialized;
    //	ofxOpenNI               openNIRecorder;
    ofxOpenNI               openNIPlayer;
    ofxOpenNIUser           nTrackedUser;
    int                     nTrackedUsers;
    ofFile                  file;
    string                  trainedImagesDirectory;
    float                   imageScale;
    string                  trainingModelJointsPosABSfileName;
    string                  trainingDataJointsPosABSfileName;
    string                  trainingModelJointsPosRelfileName;
    string                  trainingDataJointsPosRelfileName;
    string                  trainingModelJointsRotAxisAfileName;
    string                  trainingDataJointsRotAxisAfileName;
    string                  testFileName;
    string                  testFileModelName;

    ofTrueTypeFont          verdana;
    
    void                    userEvent(ofxOpenNIUserEvent & event);
    void                    gestureEvent(ofxOpenNIGestureEvent & event);
    
    ofxUISuperCanvas        *gui;
    ofxUISuperCanvas        *guiColor;
    void                    guiEvent(ofxUIEventArgs &e);
    
    //OSC
    ofxOscSender sender;


private:
    
    // artist specified settings
    int                             drawFrameRate;
    int                             drawNextFrameMilliseconds;
    bool                            drawMirrored;
    char                            displayState;
    string                          displayStateString;
    float                           bgRed, bgGreen, bgBlue;
    int                             imgBlendMode;
    bool                            imgInvertColors;
    float                           imgRed, imgGreen, imgBlue, imgAlpha;
    // debug drawing flags
    bool                            drawDepth;
    bool                            drawDepthBehind;
    int                             depthColorMode;
    int                             depthBlendMode;
    float                           depthRed, depthGreen, depthBlue, depthAlpha;
    bool                            drawSkeletons;
    int                             skelBlendMode;
    float                           skelRed, skelGreen, skelBlue, skelAlpha;
    bool                            drawJoints2MSG;
    bool                            drawMSG;
    
    bool                            sendOSC;
    bool                            setupOSC;
    string                          myHost;
    string                          myPort;
    
    
    vector < string >               directoriesAll;
    vector < string >               directoriesToLoad;
    bool                            kinected;
    
    void                            loadImages(bool load);
    bool                            loadImagesNow;
    int                             maxFilesToLoad;
    int                             nFilesToLoad;
    ofImage                         img;
    void                            invertImage(ofImage &imgREF);
    
    GRT::SVM                        trainingModelJointsPosABS;
    GRT::ClassificationData         trainingDataJointsPosABS;
    GRT::SVM                        trainingModelJointsPosRel;
    GRT::ClassificationData         trainingDataJointsPosRel;
    GRT::SVM                        trainingModelJointsRotAxisA;
    GRT::ClassificationData         trainingDataJointsRotAxisA;
    GRT::UINT label;
    GRT::UINT lbl;
    bool                            trainModelsNow;
    void                            trainModels();
    

    vector< vector< ofPoint > >     trackedUserJointsPosABS;
    vector< vector< double > >      trackedUserJointsPosABSDouble;
    vector< vector< ofPoint > >     trackedUserJointsPosRel;
    vector< vector< double > >      trackedUserJointsPosRelDouble;
    vector< vector< ofPoint > >     trackedUserJointsRotAxisA;
    vector< vector< double > >      trackedUserJointsRotAxisADouble;
    
    vector< vector< ofPoint > >     testUserJoints;
    void                            setupTestUserJoints();
    
    vector< ofPoint >               trackedUserCentersProjective;
    
    string                  img_name;
    vector< string >        imageNames;
    vector <ofImage>        images;
    
    
    
};

#endif


// Commandline application that does ONI to AVI conversion which would be perfect for this project - http://kirilllykov.github.io/blog/2013/03/19/convert-openni-star-dot-oni-files-into-avi/
// Other possible sources for info
// - http://stackoverflow.com/questions/tagged/kinect?page=2&sort=newest&pagesize=15
// - http://stackoverflow.com/questions/23467476/kinect-skeleton-algorithm-from-depth-images?answertab=active#tab-top

// Couldn't find any openFrameworks axis angles, but maybe ofNode might be of use: http://www.openframeworks.cc/documentation/3d/ofNode.html

//-- functions and properties which might be useful for recording skeletal data
//////////////////////
// ofxOpenNITypes.cpp
//////////////////////
//    void ofxOpenNIUser::drawSkeleton() {
//        ofPushStyle();
//        // DON'T NEED TO DRAW LIMBS ANYMORE!
//        //	for(int i = 0; i < limbs.size(); i++){
//        //		limbs[i].draw();
//        //	}
//        for(int i = 0; i < joints.size(); i++){
//            joints[i].draw();
//        }
//        ofPopStyle();
//    }
//
//    ofPixels & ofxOpenNIDepthThreshold::getDepthPixels(){
//        return depthPixels;
//    }
//
//    ofPixels & ofxOpenNIDepthThreshold::getMaskPixels(){
//        return maskPixels;
//    }
//
//////////////////////
// ofxOpenNI.cpp
//////////////////////
//
//
//    ofPixels& ofxOpenNI::getDepthPixels();
//    ofPixels& ofxOpenNI::getImagePixels();
//    xn::UserGenerator& ofxOpenNI::getUserGenerator();
//    drawSkeletons();
//    if(g_bIsDepthOn) drawDepth();
//    if(g_bIsHandsOn) drawHands();
//    if(g_bIsUserOn) drawSkeletons();
//    ofTranslate(getWidth(), 0.0f);
//    if(g_bIsImageOn || g_bIsInfraOn) drawImage();
// 	currentTrackedUsers[currentTrackedUserIDs[nID]].drawSkeleton();
//
//////////////////////
// ofxOpenNI.h
//////////////////////
//    bool setSkeletonProfile(XnSkeletonProfile profile);
//    XnSkeletonProfile getSkeletonProfile();
//    xn::UserGenerator& getUserGenerator();
//    xn::DepthGenerator& getDepthGenerator();
//    xn::ImageGenerator& getImageGenerator();
//    xn::DepthMetaData& getDepthMetaData();
//    xn::ImageMetaData& getImageMetaData();
//    ofEvent<ofxOpenNIUserEvent> userEvent;
//
//////////////////////
// ofxOpenNiUtils.h
//////////////////////
//    enum Joint
//    enum Limb
//    static inline ofPoint g_worldToProjective(const ofPoint& p)
//    static inline ofPoint g_projectiveToWorld(const ofPoint& p)
//    static inline ofPoint worldToProjective(const XnVector3D& p, xn::DepthGenerator & g_Depth)
//    inline Joint XnToOfJoint(XnSkeletonJoint type)
//    inline string getXNJointAsString(XnSkeletonJoint type)
//    inline string getJointAsString(Joint type)
//    inline string getLimbAsString(Limb type)
//    inline string getUserStatusAsString(UserStatusType type)
//    inline string getCalibrationStatusAsString(XnCalibrationStatus type)
//
//////////////////////
// ofxOpenNiTypes.h
//////////////////////
// class ofxOpenNIJoint
//      ofPoint & getProjectivePosition()
//      ofPoint & getWorldPosition()
// class ofxOpenNILimb
//    void draw() {
//
//        ofPushStyle();
//        if(isFound()){
//            // draw in green
//            ofSetColor(0, 255, 0);
//        }else{
//            // draw in red
//            ofSetColor(255, 0, 0);
//        }
//        ofSetLineWidth(5);
//        ofLine(ofVec2f(startJoint->getProjectivePosition()),ofVec2f(endJoint->getProjectivePosition()));
//        ofPopStyle();
//    }
// inline int numJointsInside(ofxOpenNIUser & user)
//
////////////////////////
// ofUtils --
////////////////////////
// ofGetDay()
// ofGetFrameNum()
// ofGetHours()
// ofGetMinutes()
// ofGetMonth()
// ofGetSeconds()
// ofGetSystemTimeMicros()
// ofGetYear()
// ofToString() // "the number is " + ofToString(10)
//////////////
// ofLog -- http://www.openframeworks.cc/documentation/utils/ofLog.html
//////////////
//class methods
//    ofLog()
//    setAutoSpace()
//    setChannel()
//global functions
//    ofGetLogLevel()
//    ofGetLogLevelName()
//    ofLogToConsole()
//    ofLogToFile()
//    ofSetLogLevel()
//    ofSetLoggerChannel()
///////////////
//
//    int	getNumTrackedUsers();
//    xn::UserGenerator& getUserGenerator();
//    xn::IRMetaData& getIRMetaData(); // ???
//    ofxOpenNIJoint & getJoint(Joint joint);
//    ofPoint & getCenter(); // origin/root location?
///* ofxOpenNIUser(const ofxOpenNIUser& other){
// if(this != &other){
//
//
// setup();
// center = other.center;
// joints = other.joints;
// limbs = other.limbs;
// */
//
// vector<ofxOpenNIJoint> joints;
//
///*    ofxOpenNIUser& operator=(const ofxOpenNIUser& other){
// if(this != &other){
//
// clear();
// setup();
//
// center = other.center;
// joints = other.joints;
// limbs = other.limbs;
// */
