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


//--------------------------------------------------------------
void testApp::setup() {
    
    GRT::ClassificationData trainingData;
    trainingData.loadDatasetFromCSVFile("training-data.csv");
    
    GRT::SVM svm(GRT::SVM::LINEAR_KERNEL);
    svm.train(trainingData);
    svm.saveModelToFile("data-model.txt");
    
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // TODO: clean up?
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
    
    verdana.loadFont(ofToDataPath("verdana.ttf"), 10);
    
    displayState = 'd'; //start in debug mode
    
    testFileName = "testJointData.csv";
    
    file.open(ofToDataPath(testFileName), ofFile::ReadWrite, false);
//    file.create();
    testFileBuff = file.readToBuffer();
    
    // read the directory for the images
    // we know that they are named in seq
    ofDirectory dir;
    
    int nFiles;
//    int nFiles = dir.listDir("images");
    if(nFiles) {
        
        for(int i=0; i<dir.numFiles(); i++) {
            
            // add the image to the vector
            string filePath = dir.getPath(i);
            images.push_back(ofImage());
            images.back().loadImage(filePath);
        }
        
        // TODO: set indexes of entries in DB to match loaded images
    }
    else printf("Could not find \"images\" directory\n");

}

//--------------------------------------------------------------
void testApp::update(){
    //    openNIRecorder.update();
    openNIPlayer.update();

    // clear joint data for next iteration
    trackedUserJoints.clear();

    // build joint position vector
    // TODO: make trackedUserJoints a vector of vectors, and allocate 4 (or max users) joints vectors within it
    if (openNIPlayer.getNumTrackedUsers() > 0) {
        for (int j = 0; j < openNIPlayer.getNumTrackedUsers(); ++j) {
            trackedUserJoints.push_back(openNIPlayer.getTrackedUser(0).getCenter());
            for (int i = 0; i < openNIPlayer.getTrackedUser(0).joints.size(); ++i) {
                trackedUserJoints.push_back(openNIPlayer.getTrackedUser(0).joints[i].getWorldPosition());
            }
            
        }
    }

    // TODO: create vector of data for image(s) to be displayed
    // build data associated with current tracked joint positions (L1 or L2 norm) to use for query
    // query for short list of potential images
    // loop through list performing bone-by-bone comparison?
    // calculate image positions, scales, and rotations based on user locations
}

//--------------------------------------------------------------
void testApp::draw(){
    switch (displayState) {
        case 'i':
            //TODO: display graphics in "installation" mode
            // draw image(s)
            break;
        case 'd':
        default:
            ofSetColor(255, 255, 255);
            
            ofPushMatrix();
            
            //    openNIRecorder.drawDebug(0, 0);
            openNIPlayer.drawDebug(0, 240);
            openNIPlayer.drawSkeletons(0, 240);
            
            ofPushMatrix();
            
            ofSetColor(0, 255, 0);
            string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
            
            // add bone data for tracked user to display message
            if (trackedUserJoints.size() > 0) {
                // display joint data
                for (int i=0; i < trackedUserJoints.size(); ++i) {
                    msg = msg + "\n joint[" + ofToString(i) + "] = " + ofToString(trackedUserJoints[i]);
                }
            }
            
            msg = msg + "\n" + ofToString(trackedUserJoints);
            msg = msg + "\n" + testFileBuff.getText();
            // cout << msg;
            
            verdana.drawString(msg, 20, 20);
            
            ofPopMatrix();
            ofPopMatrix();
            
            break;
    }
    
    
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
    bool fileWritten;
    string testJointBuff;
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
        case 'b':
            // TODO:  build & display data to associate with currently displayed image
            //          - joint positions (0–15)
            //          - relative joint positions
            //          - joint rotations (axis-angles?)
            //          - L1 norm
            //          - L2 norm
            //          - image positioning data
            //              > rotation
            //              > scale (might need to just store corners so that image can be scaled independent of file size changes
            break;
        case 's':
            if (displayState == 'd'){
                // TODO:  save built data into the database for the displayed image
                testFileBuff.append(ofToString(trackedUserJoints) + "\n");
            } else {
                // TODO: display some kind of error message that says data can only be saved in debug mode?
            }
            break;
            
        case 'S':
            //TODO: save test data
            // fill the buffer with something important
            //fileWritten = file.writeFromBuffer(testFileBuff);
//            file << testFileBuff.getText();
//            file = testFileBuff.getText();
            file << testFileBuff.getText();
            file.close();
            cout << "file written";
//            file.open(ofToDataPath(testFileName), ofFile::ReadWrite, false);
//            file.create();
//            testFileBuff = file.readToBuffer();
            break;
        case 'i':
            // switch displayState to "installation"
            // fall through (intentional)
        case 'd':
            // switch displayState to "debug"
            displayState = key;
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
            // TODO: display next image in database
            //          - include bone data
            break;
        case '>':
        case '.':
            openNIPlayer.nextFrame();
            // TODO: display previous image in database
            //          - including any existing bone data and calculation
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
    // TODO: drag image around. Filters:
    //          - no modifiers = position dragging
    //          - sift = rotation
    //          - ctrl = scale
    //          - opt = reference point
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    // TODO: start dragging
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    // TODO: stop dragging
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}