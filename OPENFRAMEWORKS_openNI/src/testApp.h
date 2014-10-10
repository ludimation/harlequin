#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "GRT.h"

class testApp : public ofBaseApp {

public:
    
	void setup();
	void update();
	void draw();
    void exit();
    
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

    //	ofxOpenNI               openNIRecorder;
    ofxOpenNI               openNIPlayer;
    ofxOpenNIUser           nTrackedUser;
//    vector<ofPoint>         trackedUserJoints;
    vector< vector< ofPoint > > trackedUserJoints;
    int                     nTrackedUsers;
    char                    displayState;
    vector <ofImage>        images;
    ofFile                  file;
    string                  testJointData;
    string                  testFileName;
    string                  testFileModelName;
    ofBuffer                testFileBuff;
    
    ofTrueTypeFont verdana;
    
    void userEvent(ofxOpenNIUserEvent & event);
    void gestureEvent(ofxOpenNIGestureEvent & event);

private:
    GRT::SVM svm;
    GRT::ClassificationData trainingData;
    GRT::UINT label;
    GRT::UINT lbl;
    vector< vector< double > >     trackedUserJointsDouble;
//    vector<double> trackedUserJointsDouble;
    
    string img_name;
    vector< string >        imageNames;
    
};

#endif


// Commandline application that does ONI to AVI conversion which would be perfect for this project - http://kirilllykov.github.io/blog/2013/03/19/convert-openni-star-dot-oni-files-into-avi/
// Other possible sources for info
// - http://stackoverflow.com/questions/tagged/kinect?page=2&sort=newest&pagesize=15
// - http://stackoverflow.com/questions/23467476/kinect-skeleton-algorithm-from-depth-images?answertab=active#tab-top

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
