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
//  - calculate image positions, scales, and rotations based on user locations


//--------------------------------------------------------------
void testApp::setup() {
    testFileName = "testJointData.txt";
    
    GRT::SVM svm(GRT::SVM::LINEAR_KERNEL);
    
    trainingData.setDatasetName("harlequin");
    trainingData.setNumDimensions(48);
    
    img_name = "1.jpg";
    label = 1;
    
    //trainingData.loadDatasetFromFile(testFileName);
    
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

    switch (displayState) {
        case 'i':
            
            // manage style and drawing matrix
            ofPushStyle();
            ofPushMatrix();
            ofSetColor(255, 255, 255); // white drawing color
            ofSetBackgroundColor(255, 245, 235); // light tan BG
            
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
            img.draw(300,0, img.width * 0.5f, img.height * 0.5f);
            ofPopStyle();
            break;
            
        case 'd':
        default:

            if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; }
            img.draw(300,0, img.width * 0.5f, img.height * 0.5f);

            ofPopStyle();
            ofPushStyle();
            ofPushMatrix();
            
            //    openNIRecorder.drawDebug(0, 0);
            openNIPlayer.drawDebug(0, 240);
            openNIPlayer.drawSkeletons(0, 240);
            
            ofPushMatrix();
            
            ofSetColor(255, 255, 255);
            string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
            
            // add bone data for tracked user to display message
            if (trackedUserJoints.size() > 0) {
                // display joint data
                for (int i=0; i < trackedUserJoints.size(); ++i) {
                    msg = msg + "\n joint[" + ofToString(i) + "] = " + ofToString(trackedUserJoints[i]);
                }
            }
            
            msg = msg + "\n" + ofToString(trackedUserJoints);
 //           msg = msg + "\n" + testFileBuff.getText();
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
            openNIPlayer.startPlayer(fileName);
            break;
        case 'b':
            break;
        case 's':
            if (displayState == 'd'){
                // TODO:  save built data into the database for the displayed image
                // testFileBuff.append(ofToString(trackedUserJoints) + "\n");
                
                trainingData.addSample(label, trackedUserJointsDouble);
            } else {
                // TODO: display some kind of error message that says data can only be saved in debug mode?
            }
            
            trainingData.saveDatasetToFile(testFileName);
            svm.train(trainingData);
            svm.saveModelToFile("testJointData_model.txt");
            break;
        case '[':
            label--;
            img_name = ofToString(label) + ".jpg";
            break;
        case ']':
            label++;
            img_name = ofToString(label) + ".jpg";
            break;
        case 'S':
            //TODO: save test data
            // fill the buffer with something important
            //fileWritten = file.writeFromBuffer(testFileBuff);
//            file << testFileBuff.getText();
//            file = testFileBuff.getText();
//            file << testFileBuff.getText();
//            file.close();
//            cout << "file written";
            
//            trainingData.saveDatasetToFile(testFileName);
            
//            file.open(ofToDataPath(testFileName), ofFile::ReadWrite, false);
//            file.create();
//            testFileBuff = file.readToBuffer();
            break;
        case 'c':
            svm.train(trainingData);
            svm.saveModelToFile("testJointData_model.txt");
            
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