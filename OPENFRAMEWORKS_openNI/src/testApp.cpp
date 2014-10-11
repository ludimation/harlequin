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
    trainingDataJointsPosABSfileName = "JointsPosABSData.txt";
    trainingModelJointsPosABSfileName = "JointsPosABSmodel.txt";
    
    GRT::SVM trainingModelJointsPosABS(GRT::SVM::LINEAR_KERNEL);
    
    trainingDataJointsPosABS.setDatasetName("harlequin");
    trainingDataJointsPosABS.setNumDimensions(45);
    
    
    // TODO: test these to make sure they work
    trainingDataJointsPosABS.loadDatasetFromFile(ofToDataPath(trainingDataJointsPosABSfileName));
    trainingModelJointsPosABS.loadModelFromFile(ofToDataPath(trainingModelJointsPosABSfileName)); // TODO: this doesn't seem to work
    trainingModelJointsPosABS.train(trainingDataJointsPosABS); // TODO: put this somewhere that works (doesn't seem to work here in startup())
    
    setupKinects();
    
    verdana.loadFont(ofToDataPath("verdana.ttf"), 10);
    
    displayState = 'd'; //start in debug mode
    
    
    //    file.open(ofToDataPath(trainingDataJointsPosABSfileName), ofFile::ReadWrite, false);
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
    trackedUserJointsPosABS.clear();
    trackedUserJointsPosABSDouble.clear();
    trackedUserJointsPosRel.clear();
    trackedUserJointsPosRelDouble.clear();
    trackedUserJointsRotAxisA.clear();
    trackedUserJointsRotAxisADouble.clear();
    trackedUserCentersProjective.clear();

    // create some local vectors for user position storage
    vector< ofPoint >   singleUserJointsPosABS;
    vector< double >    singleUserJointsPosABSDoubles;
    vector< ofPoint >   singleUserJointsPosRel;
    vector< double >    singleUserJointsPosRelDoubles;
    vector< ofPoint >   singleUserJointsAxisA;
    vector< double >    singleUserJointsAxisADoubles;
    
    // build joint position vectors
    if (openNIPlayer.getNumTrackedUsers()) {
        for (int j = 0; j < openNIPlayer.getNumTrackedUsers(); ++j) {
            singleUserJointsPosABS.clear();
            singleUserJointsPosRel.clear();
            singleUserJointsAxisA.clear();

            // store center positions in both world space and projective space
//            ofPoint userJCenter = openNIPlayer.getTrackedUser(j).getCenter();
//            ofPoint userJcenterProjective = openNIPlayer.worldToProjective(userJCenter);
            ofPoint userJCenter = openNIPlayer.getTrackedUser(j).joints[0].getProjectivePosition();
            ofPoint userJcenterProjective = userJCenter;
            trackedUserCentersProjective.push_back(
                                                   userJcenterProjective *
                                                   ofPoint(
                                                           float( ofGetWidth() ) / 640.0f,
                                                           float( ofGetHeight()  ) / 480.0f,
                                                           1
                                                           )
                                                   );
                                                                            //*/
//            trackedUserCentersProjective.push_back(
//                                                   userJcenterProjective *
//                                                   ofPoint(
//                                                           float( ofGetWidth() ) / 800.0f,
//                                                           float( ofGetHeight()  ) / 600.0f,
//                                                           1
//                                                           )
//                                                   );
            
            //            singleUserJointsPosABS.push_back(userCenter); // TODO: clean up?
            for (int i = 0; i < openNIPlayer.getTrackedUser(j).joints.size(); ++i) {
                ofPoint jointIworldPos = openNIPlayer.getTrackedUser(j).joints[i].getWorldPosition();
                singleUserJointsPosABS.push_back(jointIworldPos);
                singleUserJointsPosRel.push_back(jointIworldPos - userJCenter);
                // TODO: singleUserJointsAxisA.push_back(findAxisAngle(userCenter, jointIworldPos));
            }
            trackedUserJointsPosABS.push_back(singleUserJointsPosABS);
            trackedUserJointsPosRel.push_back(singleUserJointsPosRel);
            trackedUserJointsPosRel.push_back(singleUserJointsAxisA);
        }
    }
    
    // build training vectors based on joint positions
    if (trackedUserJointsPosABS.size())
    {
        for (int j = 0; j < trackedUserJointsPosABS.size(); ++j) {
            singleUserJointsPosABSDoubles.clear();
            singleUserJointsPosRelDoubles.clear();
            singleUserJointsAxisADoubles.clear();
            for (int i = 0; i < trackedUserJointsPosABS[j].size(); ++i) {
                for (int axis = 0; axis < 3; ++axis)
                {
                    // axis = {0,1,2} which correlates to ofPoint {x, y, z}
                    singleUserJointsPosABSDoubles.push_back(trackedUserJointsPosABS[j][i][axis]);
                    // TODO: singleUserJointsPosRelDoubles.push_back(trackedUserJointsPosRel[j][i][axis]);
                    // TODO: singleUserJointsAxisA.push_back(trackedUserJointsPosRel[j][i][axis]);
                }
            }
            trackedUserJointsPosABSDouble.push_back(singleUserJointsPosABSDoubles);
            trackedUserJointsPosRelDouble.push_back(singleUserJointsPosRelDoubles);
            trackedUserJointsRotAxisADouble.push_back(singleUserJointsAxisADoubles);
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){

    ofImage img;
    ofPoint jointsCenterProjective;
    ofPoint imgRef;
    ofPoint screenCenter = ofVec3f(ofGetWidth()/2.0f, ofGetHeight()/2.0f, 1.0f);

    bool drawDepth;
    bool drawSkeletons;
    bool drawJoints2MSG;
    bool drawMSG;
    
    // Build debug message string
    string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
    
    switch (displayState) {
        case 'i':
            
            drawMSG = true;
            drawJoints2MSG = true;
            drawDepth = true;
            drawSkeletons = true;
            
            // manage style and drawing matrix
            ofSetBackgroundColor(255, 245, 235); // light tan BG
            ofPushStyle();
            ofPushMatrix();
            ofSetColor(255, 255, 255); // white drawing color
            
            for (int j = 0; j < trackedUserJointsPosABSDouble.size(); ++j) {
                
                // draw image(s)
                if (trainingModelJointsPosABS.predict(trackedUserJointsPosABSDouble[j]))
                {
                    label = trainingModelJointsPosABS.getPredictedClassLabel();
                    cout << "predicted label:" << trainingModelJointsPosABS.getPredictedClassLabel() << endl;
                    
                    if (label > imageNames.size())
                    {
                        label = ofRandom(0, imageNames.size() - 1);
                    }
                    
                    img_name = imageNames[label];
                    cout << "img_name = " << img_name << endl;
//                    img_name = ofToString(label) + ".jpg";
                }
                else
                {
                    cout << "trainingModelJointsPosABS could not predict" << endl;
                }
                
                // TODO: find another image if image could not be loaded?
                //if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; }

                if (openNIPlayer.getNumTrackedUsers() >= j) {
                    jointsCenterProjective = trackedUserCentersProjective[j];
                    //                    jointsCenterProjective = openNIPlayer.getTrackedUser(j).getCenter(); // ranges {(±500),(-350+250),(200–2,000?)}
                    cout << "jointsCenterProjective = trackedUserCentersProjective[j];" << endl;
                } else {
                    jointsCenterProjective = ofVec3f(screenCenter.x, screenCenter.y, 1400.0f);
                    cout << "jointsCenterProjective = ofVec3f(screenCenter.x, screenCenter.y, 1400.0f);" << endl;
                }
                
                // set image to draw
                img = images[label];
                
                // calculate reference points for drawing
                imgRef.z = 1500.0f / float( jointsCenterProjective.z ); // scale
                float xOffset = float( img.width  ) * imgRef.z / 2.0f;
                float yOffset = float( img.height  ) * imgRef.z / 2.0f;
                imgRef.x = jointsCenterProjective.x - xOffset; // left side
                imgRef.y = jointsCenterProjective.y - yOffset; // top side
//                imgRef.x = jointsCenterProjective.x - (img.width  * imgRef.z / 2.0f);
//                imgRef.y = jointsCenterProjective.y - (img.height * imgRef.z / 2.0f);
                
                // draw image at position and scale relative to center of screen and image
                img.draw(imgRef.x,
                         imgRef.y,
                         img.width * imgRef.z,
                         img.height * imgRef.z);
                
                // Build debug message string
                msg = msg + "\n jointsCenterProjective = " + ofToString(jointsCenterProjective);
                msg = msg + "\n xOffset = " + ofToString(xOffset);
                msg = msg + "\n yOffset = " + ofToString(yOffset);
                msg = msg + "\n imgRef = " + ofToString(imgRef);
                msg = msg + "\n sceenCenter = " + ofToString(screenCenter);
            }
            
            // reset drawing matrix and style
            ofPopMatrix();
            ofPopStyle();
            
            drawMSG = true;
            drawJoints2MSG = true;
            drawDepth = true;
            drawSkeletons = true;

            break;
            
        case 'd':
        default:

            drawMSG = true;
            drawJoints2MSG = true;
            drawDepth = true;
            drawSkeletons = true;

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
            
            img.draw(
                     (ofGetWidth() - img.width * imgRatio) / 2.0f,
                     (ofGetHeight() - img.height * imgRatio) / 2.0f,
                     img.width * imgRatio,
                     img.height * imgRatio
                     );
            
            // reset drawing matrix and style
            ofPopMatrix();
            ofPopStyle();

            break;
    }
    
    if (drawSkeletons or drawDepth){
        ofPushStyle();
        //  openNIRecorder.drawDebug(0, 0);
        if (drawDepth)
        {
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            openNIPlayer.drawDepth(0.0f, 0.0f, float( ofGetWidth() ), float( ofGetHeight() ));
        }
        if (drawSkeletons)
        {
            ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
            openNIPlayer.drawSkeletons(0.0f, 0.0f, float( ofGetWidth() ), float( ofGetHeight() ));
        }
        
        ofPopStyle();
    }
    
    if (drawJoints2MSG) {
        // add bone data for tracked user to display message
        if (trackedUserJointsPosABS.size() > 0) {
            // display joint data
            for (int j=0; j < trackedUserJointsPosABS.size(); ++j) {
                msg = msg + "\n====\n== User[" + ofToString(j) + "]\n----";
                for (int i=0; i < trackedUserJointsPosABS[j].size(); ++i) {
                    msg = msg + "\n    joint[" + ofToString(i) + "] = " + ofToString(trackedUserJointsPosABS[j][i]);
                }
                msg = msg + "\n====";
            }
        }
        // if (trackedUserJointsPosABS.size()) msg = msg + "\n" + ofToString(trackedUserJointsPosABS);
    }

    if (drawMSG) {
        // draw debug message
        verdana.drawString(msg, 20, 20);
        cout << msg << endl;
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
    bool saveData;
    bool saveModel;
    
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
                trainingDataJointsPosABS.addSample(label, trackedUserJointsPosABSDouble[0]);
            } else {
                // TODO: display some kind of error message that says data can only be saved in debug mode?
            }
            
            break;
        case '<':
        case ',':
        case '[':
           
            saveData = true;
            
            if (displayState == 'i') break; // do not train data during installation mode
            
            // display previous image in database
            if (label > 0) label--;
            img_name = imageNames[label];

            // openNIPlayer.previousFrame();
            break;
        case '>':
        case '.':
        case ']':
            
            saveData = true;
            
            if (displayState == 'i') break; // do not train data during installation mode

            // display next image in database
            if (label < imageNames.size()) label++;
            img_name = imageNames[label];

            // openNIPlayer.nextFrame();
            break;
        case 'r': // random image
            
            saveData = true;

            if (displayState == 'i') break; // do not train data during installation mode
            
            // display next image in database
            label = getRandomExcluding(0, imageNames.size() - 1, label);
            img_name = imageNames[label];

            break;
        case 's': // NOTE: Moved save functionality here to minimize lagging during data building phase
            if (displayState == 'i') break; // do not train data during installation mode
            
            saveData = true;
            saveModel = true;
            
            break;
        case 'c': // TODO: clean  up?
            if (displayState == 'i') break; // do not train data during installation mode

            saveData = true;
            saveModel = true;
            
            if (trainingModelJointsPosABS.predict(trackedUserJointsPosABSDouble[0]))
            {
                label = trainingModelJointsPosABS.getPredictedClassLabel();
                cout << "label:" << trainingModelJointsPosABS.getPredictedClassLabel() << endl;
                img_name = imageNames[label] + ".jpg";
            }
            else
            {
                cout << "trainingModelJointsPosABS could not predict" << endl;
            }
            break;
        case 'i':
            // switch displayState to "installation"
            saveData = true;
            saveModel = true;

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
        case 'k':
            setupKinects(); // TODO: debug this, doesn't seem to work properly.
            break;
        case 't':
            //            openNIRecorder.toggleRegister();
            break;
    }
    
    if (saveData) trainingDataJointsPosABS.saveDatasetToFile(ofToDataPath(trainingDataJointsPosABSfileName));
    
    if (saveModel) {
        trainingModelJointsPosABS.train(trainingDataJointsPosABS);
        trainingModelJointsPosABS.saveModelToFile(ofToDataPath(trainingModelJointsPosABSfileName));
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

void testApp::setupKinects() {
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
}

int testApp::getRandomExcluding(int min, int max, int i) {
    // TODO: Might want to simply hand over a vector of things to select from, and a second vector of filters to exclude?
    
    // exceptions
    if (max - min <= 1 and i <= max and i >= min) return -1;
    
    int value = i;

    while (value == i) {
        value = ofRandom(min, max);
    }
    
    return value;
}
