#include "testApp.h"

// what is this? Why is it re-defining ofxOpenNIUser?
class myUser : public ofxOpenNIUser {
public:
    void test(){
        cout << "test" << endl;
    }
};

///////////
// TODO:
////////////
//  - set labels of entries in DB to match loaded image names
//  - display data associated with current image
//  - translation, scale, and rotation controls for current image
//  - relative joint positions // haven't done these yet. Are they necessary?
//  - joint rotations (axis-angles?)
//  - L1 norm
//  - L2 norm
//  - filters for file tags (drawing, photo, male, female, dancer, pedestrian, public figure, etc.)
//  - save image positioning data
//      > rotation
//      > scale (might need to just store corners so that image can be scaled independent of file size changes
//  - clean out old oni recording functionality? Doesn't seem useful at this point. Could write a separate app for recording and include bone data for testing input.
//  - optimize image loading/display by creating an image buffer
//  - query for short list of potential images (instead of returning just one as we do now)
//  - loop through list performing bone-by-bone comparison?


//--------------------------------------------------------------
void testApp::setup() {
    testFileName = "testJointData.txt";
    testFileModelName = "testJointData_model.txt";
    
    GRT::SVM svm(GRT::SVM::LINEAR_KERNEL);
    
    trainingData.setDatasetName("harlequin");
    trainingData.setNumDimensions(48);
    
    img_name = "1.jpg";
    label = 1;
    
    // TODO: test these to make sure they work
    trainingData.loadDatasetFromFile(ofToDataPath(testFileName));
    svm.loadModelFromFile(ofToDataPath(testFileModelName));
    
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
    
    
    //    file.open(ofToDataPath(testFileName), ofFile::ReadWrite, false);
    //    file.create();
    //    testFileBuff = file.readToBuffer();
    
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
    
    trackedUserJointsDouble.clear();
    
    for (int i = 0; i<trackedUserJoints.size(); ++i) {
        trackedUserJointsDouble.push_back(trackedUserJoints[i][0]);
        trackedUserJointsDouble.push_back(trackedUserJoints[i][1]);
        trackedUserJointsDouble.push_back(trackedUserJoints[i][2]);
    }
}

//--------------------------------------------------------------
void testApp::draw(){

    ofImage img;
    ofPoint jointsCenter;
    ofPoint imgRef;
    ofPoint screenCenter = ofVec3f(WIDTH/2.0f, - HEIGHT/2.0f, 1.0f);
    if (openNIPlayer.getNumTrackedUsers() > 0) {
        jointsCenter = openNIPlayer.getTrackedUser(0).getCenter(); // ranges {(±500),(-350+250),(
    } else {
        jointsCenter = ofVec3f(0.0f, 0.0f, 1400.0f);
    }
    imgRef[2] = 700.0f / jointsCenter[2];
    imgRef[0] =   jointsCenter[0] - (img.width  * imgRef[2]) + screenCenter[0];
    imgRef[1] = - jointsCenter[1] - (img.height * imgRef[2]) + screenCenter[1];

    switch (displayState) {
        case 'i':
            
            // manage style and drawing matrix
            ofSetBackgroundColor(255, 245, 235); // light tan BG
            ofPushStyle();
            ofPushMatrix();
            ofSetColor(255, 255, 255); // white drawing color
            
            // draw image(s)
            if (svm.predict(trackedUserJointsDouble))
            {
                lbl = svm.getPredictedClassLabel();
                cout << "label:" << svm.getPredictedClassLabel() << endl;
                img_name = ofToString(lbl) + ".jpg";
            }
            else
            {
                cout << "svm could not predict" << endl;
            }
            
            if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; }
            
            // draw image at position and scale relative to center of screen and image
            img.draw(imgRef[0],
                     imgRef[1],
                     img.width * imgRef[2],
                     img.height * imgRef[2]);
            
            // reset drawing matrix and style
            ofPushMatrix();
            ofPopStyle();
            
            break;
            
        case 'd':
        default:

            // manage style and drawing matrix
            ofSetBackgroundColorHex(000000); // Black BG // TODO: fix this because it doesn't seem to be working
            ofPushStyle();
            ofPushMatrix();
            ofSetColor(255, 255, 255); // white drawing color

            // draw image(s)
            if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; }
            img.draw(300,0, img.width * 0.5f, img.height * 0.5f);

            // draw live input from kinect // TODO: make this multiply over image in BG so pose can still be clearly visible
            //  openNIRecorder.drawDebug(0, 0);
            openNIPlayer.drawDebug(0, 240);
            openNIPlayer.drawSkeletons(0, 240);
            
            // Build debug message string
            string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
            // add bone data for tracked user to display message
            if (trackedUserJoints.size() > 0) {
                // display joint data
                for (int i=0; i < trackedUserJoints.size(); ++i) {
                    msg = msg + "\n joint[" + ofToString(i) + "] = " + ofToString(trackedUserJoints[i]);
                }
            }
            if (trackedUserJoints.size()) msg = msg + "\n" + ofToString(trackedUserJoints);
            // cout << msg;
            
            // draw debug message
            verdana.drawString(msg, 20, 20);
            
            // reset drawing matrix and style
            ofPopMatrix();
            ofPopStyle();

            break;
    }

    ofPopStyle();
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

    // string fileName = "test_20140501_1904.oni";
    int cloudRes = -1;
    bool fileWritten;
    string testJointBuff;
    
    ofImage img;
    
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
            //openNIPlayer.startPlayer(fileName);
            break;
        case 'b': // NOTE: updated to 'b' for BUILD DATA
            if (displayState == 'i') break; // do not train data during installation mode

            // Store data to associate with currently displayed image
            //  - joint positions (0–15) — 0 = center, 1–15 = joints
            
            if (displayState == 'd'){
                trainingData.addSample(label, trackedUserJointsDouble);
            } else {
                // TODO: display some kind of error message that says data can only be saved in debug mode?
            }
            
            break;
        case '<':
        case ',':
        case '[':
            if (displayState == 'i') break; // do not train data during installation mode
            
            // display previous image in database
            label--;
            img_name = ofToString(label) + ".jpg";

            // openNIPlayer.previousFrame();
            break;
        case '>':
        case '.':
        case ']':
            if (displayState == 'i') break; // do not train data during installation mode

            // display next image in database
            label++;
            img_name = ofToString(label) + ".jpg";

            // openNIPlayer.nextFrame();
            break;
        case 's': // NOTE: Moved save functionality here to minimize lagging during data building phase
            if (displayState == 'i') break; // do not train data during installation mode
            
            trainingData.saveDatasetToFile(ofToDataPath(testFileName));
            svm.train(trainingData);
            svm.saveModelToFile(ofToDataPath(testFileModelName));
            
            break;
        case 'c': // TODO: clean  up?
            if (displayState == 'i') break; // do not train data during installation mode

            svm.train(trainingData);
            svm.saveModelToFile(ofToDataPath(testFileModelName));
            
            if (svm.predict(trackedUserJointsDouble))
            {
                lbl = svm.getPredictedClassLabel();
                cout << "label:" << svm.getPredictedClassLabel() << endl;
                img_name = ofToString(lbl) + ".jpg";
            }
            else
            {
                cout << "svm could not predict" << endl;
            }
            break;
        case 'i':
            // switch displayState to "installation"
            // fall through (intentional)
        case 'd':
            // switch displayState to "debug"
            displayState = key;
            break;
        case '/':
            // openNIPlayer.setPaused(!openNIPlayer.isPaused());
            break;
        case 'm':
            // openNIPlayer.firstFrame();
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
    //  - or maybe take a "tool" approach where 'w' = translate, 'e' = rotate, 'r' = scale
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