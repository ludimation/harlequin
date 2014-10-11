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
//  - multiple user input (multiple harlequins on-screen at the same time)
//  - make data image name-labeled so we can just read all of the images in a directory and include them in the data set
//  - do not display the same image more than once every 3 frames
//  - slow down frame rate to 30, or even as low as 12 or so?
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
    
    
    // TODO: test these to make sure they work
    trainingData.loadDatasetFromFile(ofToDataPath(testFileName));
    svm.loadModelFromFile(ofToDataPath(testFileModelName)); // TODO: this doesn't seem to work
    svm.train(trainingData); // TODO: put this somewhere that works (doesn't seem to work here in startup())
    
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
    openNIPlayer.setMaxNumUsers(4); // was 2
	openNIPlayer.start();
    
    verdana.loadFont(ofToDataPath("verdana.ttf"), 10);
    
    displayState = 'd'; //start in debug mode
    
    
    //    file.open(ofToDataPath(testFileName), ofFile::ReadWrite, false);
    //    file.create();
    //    testFileBuff = file.readToBuffer();
    
    // read the directory for the images
    // we know that they are named in seq
    ofDirectory dir;
    
    //int nFiles;
    int nFiles = dir.listDir("images");
    ofImage imgTMP;
    string filePath;
    imageNames.clear();
//    int maxFilesToLoad = dir.numFiles();
    int maxFilesToLoad = 64;
    
    if(nFiles) {
        for(int i=0; i < maxFilesToLoad; i++) {
            
            // add the image name to a list
            filePath = dir.getPath(i);
            imageNames.push_back(filePath);
            if (imgTMP.loadImage(filePath)) images.push_back(imgTMP);
            //            images.push_back(ofImage());
            //            images.back().loadImage(filePath);
            cout << "loading image [" << ofToString(i) << "/" << ofToString(maxFilesToLoad) << "] : " << filePath << endl;
        }
        
    } else printf("Could not find \"images\" directory\n");
    label = 0;
    img_name = imageNames[label];

}

//--------------------------------------------------------------
void testApp::update(){
    //    openNIRecorder.update();
    openNIPlayer.update();

    // clear joint data for next iteration
    trackedUserJoints.clear(); // TODO: multiple user input — make trackedUserJoints a vector of vectors, and allocate 4 (or max users) joints vectors within it

    vector< ofPoint > singleUserJoints;

    // build joint position vector
    if (openNIPlayer.getNumTrackedUsers()) {
        for (int j = 0; j < openNIPlayer.getNumTrackedUsers(); ++j) {
            singleUserJoints.clear();
            singleUserJoints.push_back(openNIPlayer.getTrackedUser(j).getCenter());
            for (int i = 0; i < openNIPlayer.getTrackedUser(j).joints.size(); ++i) {
                singleUserJoints.push_back(openNIPlayer.getTrackedUser(j).joints[i].getWorldPosition());
//                singleUserJoints.push_back(openNIPlayer.getTrackedUser(j).joints[i].getWorldPosition() - singleUserJoints[0]);
            }
            trackedUserJoints.push_back(singleUserJoints);
            
//            trackedUserJoints.push_back(openNIPlayer.getTrackedUser(0).getCenter());
//            for (int i = 0; i < openNIPlayer.getTrackedUser(0).joints.size(); ++i) {
//                trackedUserJoints.push_back(openNIPlayer.getTrackedUser(0).joints[i].getWorldPosition());
//            }
            
        }
    }
    
    vector< double > singleUserDoubles;
    trackedUserJointsDouble.clear();
    
    if (trackedUserJoints.size())
    {
        for (int j = 0; j < trackedUserJoints.size(); ++j) {
            singleUserDoubles.clear();
            for (int i = 0; i < trackedUserJoints[j].size(); ++i) {
                singleUserDoubles.push_back(trackedUserJoints[j][i][0]); // x
                singleUserDoubles.push_back(trackedUserJoints[j][i][1]); // y
                singleUserDoubles.push_back(trackedUserJoints[j][i][2]); // z
            }
            trackedUserJointsDouble.push_back(singleUserDoubles);
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){

    ofImage img;
    ofPoint jointsCenter;
    ofPoint imgRef;
    ofPoint screenCenter = ofVec3f(ofGetWidth()/2.0f, ofGetHeight()/2.0f, 1.0f);

    // Build debug message string
    string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
    
    switch (displayState) {
        case 'i':
            
            // manage style and drawing matrix
            ofSetBackgroundColor(255, 245, 235); // light tan BG
            ofPushStyle();
            ofPushMatrix();
            ofSetColor(255, 255, 255); // white drawing color
            
            for (int j = 0; j < trackedUserJointsDouble.size(); ++j) {
                
                // draw image(s)
                if (svm.predict(trackedUserJointsDouble[j]))
                {
                    label = svm.getPredictedClassLabel();
                    cout << "predicted label:" << svm.getPredictedClassLabel() << endl;
                    
                    img_name = imageNames[label];
                    cout << "img_name = " << img_name << endl;
//                    img_name = ofToString(label) + ".jpg";
                }
                else
                {
                    cout << "svm could not predict" << endl;
                }
                
                // TODO: find another image if image could not be loaded?
                //if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; }

                if (openNIPlayer.getNumTrackedUsers() >= j) {
                    jointsCenter = openNIPlayer.getTrackedUser(j).getCenter(); // ranges {(±500),(-350+250),(200–2,000?)}
                } else {
                    jointsCenter = ofVec3f(0.0f, 0.0f, 1400.0f);
                }
                imgRef.z = 700.0f / jointsCenter.z; // (-50, -200, 1800)
                imgRef.x = screenCenter.x + jointsCenter.x - (img.width  * imgRef.z);
                imgRef.y = screenCenter.y - jointsCenter.y - (img.height * imgRef.z);
                
                img = images[label];
                // draw image at position and scale relative to center of screen and image
                img.draw(imgRef.x,
                         screenCenter.y - (img.height * imgRef.z / 2),
                         img.width * imgRef.z,
                         img.height * imgRef.z);
                
//                // draw image at position and scale relative to center of screen and image
//                img.draw(imgRef.x,
//                         imgRef.y,
//                         img.width * imgRef.z,
//                         img.height * imgRef.z);
                
                // Build debug message string
                msg = msg + "/n imgRef = " + ofToString(imgRef);
                msg = msg + "/n sceenCenter = " + ofToString(screenCenter);
            }

            // reset drawing matrix and style
            ofPushMatrix();
            ofPopStyle();
            
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            // draw live input from kinect // TODO: make this multiply over image in BG so pose can still be clearly visible
            //  openNIRecorder.drawDebug(0, 0);
            openNIPlayer.drawDepth(0, 0, ofGetWidth(), ofGetHeight());
            openNIPlayer.drawSkeletons(screenCenter.x, screenCenter.y, ofGetWidth(), ofGetHeight());
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
            // if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; }
            img = images[label];
            float imgRatioX = float(ofGetWidth()) / float(img.width);
            float imgRatioY = float(ofGetHeight()) / float(img.height);
            float imgRatio;
            if (imgRatioX < imgRatioY)
            {
                imgRatio = imgRatioX;
            } else        {
                imgRatio = imgRatioY;
            }
            
            // debug
            msg = msg + "\n" + "img.width = "       + ofToString(img.width);
            msg = msg + "\n" + "img.height = "      + ofToString(img.height);
            msg = msg + "\n" + "ofGetWidth() = "    + ofToString(ofGetWidth());
            msg = msg + "\n" + "ofGetHeight() = "   + ofToString(ofGetHeight());
            msg = msg + "\n" + "imgRatioX = "       + ofToString(imgRatioX);
            msg = msg + "\n" + "imgRatioY = "       + ofToString(imgRatioY);
            msg = msg + "\n" + "imgRatio = "        + ofToString(imgRatio);
    
            img.draw(
                     (ofGetWidth() - img.width * imgRatio) / 2.0f,
                     (ofGetHeight() - img.height * imgRatio) / 2.0f,
                     img.width * imgRatio,
                     img.height * imgRatio
                     );
            // debug
            msg = msg + "\nimg_name = " + img_name;
            msg = msg + "\n draw(" + ofToString((ofGetWidth()/2.0f) - (img.width * imgRatio));
            msg = msg + ", " + ofToString((ofGetHeight()/2.0f) - (img.height * imgRatio));
            msg = msg + ", " + ofToString(img.width * imgRatio);
            msg = msg + ", " + ofToString(img.height * imgRatio);
            msg = msg + ")";
            

            ofPushStyle();
                ofEnableBlendMode(OF_BLENDMODE_ADD);
                // draw live input from kinect // TODO: make this multiply over image in BG so pose can still be clearly visible
                //  openNIRecorder.drawDebug(0, 0);
                openNIPlayer.drawDebug(0, 240);
                openNIPlayer.drawSkeletons(0, 240);
            ofPopStyle();
            
            // add bone data for tracked user to display message
            if (trackedUserJoints.size() > 0) {
                // display joint data
                for (int j=0; j < trackedUserJoints.size(); ++j) {
                    msg = msg + "\n====\n== User[" + ofToString(j) + "]\n----";
                    for (int i=0; i < trackedUserJoints[j].size(); ++i) {
                        msg = msg + "\n    joint[" + ofToString(i) + "] = " + ofToString(trackedUserJoints[j][i]);
                    }
                    msg = msg + "\n====";
                }
            }
            // if (trackedUserJoints.size()) msg = msg + "\n" + ofToString(trackedUserJoints);
            // cout << msg;
            
            // reset drawing matrix and style
            ofPopMatrix();
            ofPopStyle();

            break;
    }

    // draw debug message
    verdana.drawString(msg, 20, 20);
    
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
                trainingData.addSample(label, trackedUserJointsDouble[0]);
            } else {
                // TODO: display some kind of error message that says data can only be saved in debug mode?
            }
            
            break;
        case '<':
        case ',':
        case '[':
            trainingData.saveDatasetToFile(ofToDataPath(testFileName));
            if (displayState == 'i') break; // do not train data during installation mode
            
            // display previous image in database
            if (label > 0) label--;
            img_name = imageNames[label];

            // openNIPlayer.previousFrame();
            break;
        case '>':
        case '.':
        case ']':
            trainingData.saveDatasetToFile(ofToDataPath(testFileName));
            if (displayState == 'i') break; // do not train data during installation mode

            // display next image in database
            if (label < imageNames.size()) label++;
            img_name = imageNames[label];

            // openNIPlayer.nextFrame();
            break;
        case 'r': // random image
            trainingData.saveDatasetToFile(ofToDataPath(testFileName));
            if (displayState == 'i') break; // do not train data during installation mode
            
            // display next image in database
            label = ofRandom(0, imageNames.size() - 1);
            img_name = imageNames[label];

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
            
            if (svm.predict(trackedUserJointsDouble[0]))
            {
                label = svm.getPredictedClassLabel();
                cout << "label:" << svm.getPredictedClassLabel() << endl;
                img_name = imageNames[label] + ".jpg";
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