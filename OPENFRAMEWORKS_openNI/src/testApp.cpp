#include "testApp.h"

// what is this? Why is it re-defining ofxOpenNIUser?
class myUser : public ofxOpenNIUser {
public:
    void test(){
        cout << "test" << endl;
    }
};

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

//--------------------------------------------------------------
void testApp::setup() {
    
    GRT::ClassificationData trainingData;
    trainingData.loadDatasetFromCSVFile("training-data.csv");
    
    GRT::SVM svm(GRT::SVM::LINEAR_KERNEL);
    svm.train(trainingData);
    svm.saveModelToFile("data-model.txt");
    
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // TODO:
    
//    openNIRecorder.setup();
//    openNIRecorder.addDepthGenerator();
//    openNIRecorder.addImageGenerator();
//    openNIRecorder.setRegister(true);
//    openNIRecorder.setMirror(true);
//    openNIRecorder.addUserGenerator();
//    openNIRecorder.setMaxNumUsers(2);
//    openNIRecorder.start();

    openNIPlayer.setup();
    openNIPlayer.addDepthGenerator();
    openNIPlayer.addImageGenerator();
    openNIPlayer.setRegister(true);
    openNIPlayer.setMirror(true);
    openNIPlayer.addUserGenerator();
    openNIPlayer.setMaxNumUsers(2); // could be as many as 4?
	openNIPlayer.start();
    
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
}

//--------------------------------------------------------------
void testApp::update(){
//    openNIRecorder.update();
    openNIPlayer.update();

    //nTrackedUser = openNIPlayer.getTrackedUser(1);
    if (openNIPlayer.getNumTrackedUsers() > 0) {
        // clear joint data for next iteration
        trackedUserJoints.clear();

        trackedUserJoints.push_back(openNIPlayer.getTrackedUser(0).getCenter());
        for (int i = 0; i < openNIPlayer.getTrackedUser(0).joints.size(); ++i) {
            trackedUserJoints.push_back(openNIPlayer.getTrackedUser(0).joints[i].getWorldPosition());
        }
    }
    
    // do something with joint data
    
    /*nTrackedUsers = openNIPlayer.getNumTrackedUsers();
    
    for(int i = 0;  i < nTrackedUsers; ++i){
        // get joint data for tracked user
        // display joint data
    }*/

    
    //    int	getNumTrackedUsers();
    //    xn::UserGenerator& getUserGenerator();
    //    xn::IRMetaData& getIRMetaData(); // ???
    //    ofxOpenNIJoint & getJoint(Joint joint);
    //    ofPoint & getCenter(); // origin/root location?
    /* ofxOpenNIUser(const ofxOpenNIUser& other){
     if(this != &other){
     
     
     //setup();
     center = other.center;
     joints = other.joints;
     limbs = other.limbs;
     */
    
    // vector<ofxOpenNIJoint> joints;
    
    /*    ofxOpenNIUser& operator=(const ofxOpenNIUser& other){
     if(this != &other){
     
     //clear();
     //setup();
     
     center = other.center;
     joints = other.joints;
     limbs = other.limbs;
     */


}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
    
    ofPushMatrix();
    
//    openNIRecorder.drawDebug(0, 0);
    openNIPlayer.drawDebug(0, 240);
    openNIPlayer.drawSkeletons(0, 240);
    

//    ofxOpenNIUser::drawSkeleton();
//    ofxOpenNI::drawSkeletons();
//    ofxOpenNI::getUserGenerator()


    ofPushMatrix();
    
	ofSetColor(0, 255, 0);
	string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
    
    ///*
    if (trackedUserJoints.size() > 0) {
        // display joint data
        for (int i=0; i < trackedUserJoints.size(); ++i) {
            msg = msg + "\n joint[" + ofToString(i) + "] = " + ofToString(trackedUserJoints[i]);
        }
    }
     //*/
    
	verdana.drawString(msg, 20, 20);

    cout << msg;

}

//--------------------------------------------------------------
void testApp::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void testApp::gestureEvent(ofxOpenNIGestureEvent & event){
    ofLogNotice() << event.gestureName << getGestureStatusAsString(event.gestureStatus) << "from device" << event.deviceID << "at" << event.timestampMillis;
}

//--------------------------------------------------------------
void testApp::exit(){
//    openNIRecorder.stop();
    openNIPlayer.stop();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

    int cloudRes = -1;
    string fileName = "test_20140501_1904.oni";
    switch (key) {
        case ' ':
//            if(!openNIRecorder.isRecording()){
////                openNIRecorder.startRecording(ofToDataPath("test"+ std::to_string(ofGetSystemTimeMicros()/1000)+".oni"));
//                openNIRecorder.startRecording(ofToDataPath(fileName));
//            }else{
//                openNIRecorder.stopRecording();
//            }
            break;
        case 'p':
            openNIPlayer.startPlayer(fileName);
            break;
        case '/':
            openNIPlayer.setPaused(!openNIPlayer.isPaused());
            break;
        case 'm':
            openNIPlayer.firstFrame();
            break;
        case '<':
        case ',':
            openNIPlayer.previousFrame();
            break;
        case '>':
        case '.':
            openNIPlayer.nextFrame();
            break;
        case 'x':
//            openNIRecorder.stop();
            openNIPlayer.stop();
            break;
        case 't':
//            openNIRecorder.toggleRegister();
            break;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}