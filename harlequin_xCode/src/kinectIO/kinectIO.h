//
//  kinectIO.h
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#pragma once

#include "ofxOpenNI.h"

//#ifndef harlequin_kinectIO_h
//#define harlequin_kinectIO_h

class kinectIO { // can I extend ofxOpenNI or the ofxKinectForWindows depending on the platform?
    
public:
    
    kinectIO();
    ~kinectIO();
    
    void setup();
    void exit();
    void update();
    void drawDepth(int x_, int y_, int w_, int h_);
    void drawSkeletons(int x_, int y_, int w_, int h_);
    
    // core kinect functions
    bool setupKinects(bool drawMirrored);
    bool stopKinects(); // TODO: proper implementation of stopping kinects and starting kinects again. doesn't seem to work properly after kinects have been stopped.
    void userEvent(ofxOpenNIUserEvent & event);
    void gestureEvent(ofxOpenNIGestureEvent & event);
    void setMirror(bool drawMirrored_);
    void setDepthColoring(DepthColoring depthColorMode_);
    
    // joints handling
    vector< vector< ofPoint > > getUserJoints();
    vector< vector< double > >  getUserJointsDoubles();
    vector< ofPoint >           getTrackedUserCentersProjective();
    vector< vector< ofPoint > > setupTestUserJoints();
    vector< vector< double > >  convertUserJointToDouble(vector< vector < ofPoint > > ofPointJoints_);

    // properties
    ofxOpenNI       *openNIdevice;
    ofxOpenNIUser   nTrackedUser;
    bool            initialized;
    // joints vectors
    vector< ofPoint >           trackedUserCentersProjective;
    vector< vector< ofPoint > > trackedUserJointsPosABS;
    vector< vector< double > >  trackedUserJointsPosABSDouble;
    vector< vector< ofPoint > > trackedUserJointsPosRel;
    vector< vector< double > >  trackedUserJointsPosRelDouble;
    vector< vector< ofPoint > > trackedUserJointsRotAxisA;
    vector< vector< double > >  trackedUserJointsRotAxisADouble;
    vector< vector< ofPoint > > testUserJoints;
    vector< vector< double > >  testUserJointsDouble;

    
private:
    // internal data
    // internal functions
};



//#endif


///////////////////
// OPEN NI NOTES //
///////////////////
// Commandline application that does ONI to AVI conversion which would be perfect for this project - http://kirilllykov.github.io/blog/2013/03/19/convert-openni-star-dot-oni-files-into-avi/
// Other possible sources for info
// - http://stackoverflow.com/questions/tagged/kinect?page=2&sort=newest&pagesize=15
// - http://stackoverflow.com/questions/23467476/kinect-skeleton-algorithm-from-depth-images?answertab=active#tab-top
//
// Couldn't find any openFrameworks axis angles, but maybe ofNode might be of use: http://www.openframeworks.cc/documentation/3d/ofNode.html
//
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
