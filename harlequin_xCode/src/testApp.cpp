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
//  - do not display the same image more than once every 3 frames
//  - slow down frame rate to 30, or even as low as 12 or so?
//  - make data image name-labeled so we can just read all of the images in a directory and include them in the data set
//  - display data associated with current image
//  - save image positioning data —- translation, scale, and rotation controls for current image
//      > rotation
//      > scale (might need to just store corners or dimensions so that image can be scaled independent of file size changes
//  - joint rotations (axis-angles?)
//  - filters for file tags (drawing, photo, male, female, dancer, pedestrian, public figure, etc.)
//  - query for short list of potential images (instead of returning just one as we do now)


//--------------------------------------------------------------
void testApp::setup() {
    
    ///////////////////////////////////
    // Training Data and Model Setup //
    ///////////////////////////////////
    trainingDataJointsPosABSfileName    = "JointsPosABSdata.txt";
    trainingModelJointsPosABSfileName   = "JointsPosABSmodel.txt";
    trainingDataJointsPosRelfileName    = "JointsPosReldata.txt";
    trainingModelJointsPosRelfileName   = "JointsPosRelmodel.txt";
    trainingDataJointsRotAxisAfileName  = "JointsRotAxisAdata.txt";
    trainingModelJointsRotAxisAfileName = "JointsRotAxisAmodel.txt";
    //
    GRT::SVM trainingModelJointsPosABS(GRT::SVM::LINEAR_KERNEL);
    GRT::SVM trainingModelJointsPosRel(GRT::SVM::LINEAR_KERNEL);
    GRT::SVM trainingModelJointsRotAxisA(GRT::SVM::LINEAR_KERNEL);
    //
    trainingDataJointsPosABS.setDatasetName("harlequinPosABS");
    trainingDataJointsPosABS.setNumDimensions(45);
    trainingDataJointsPosRel.setDatasetName("harlequinPosRel");
    trainingDataJointsPosRel.setNumDimensions(45);
    trainingDataJointsRotAxisA.setDatasetName("harlequinRotAxisA");
    trainingDataJointsRotAxisA.setNumDimensions(45);
    //
    //
    trainingDataJointsPosABS.loadDatasetFromFile(ofToDataPath(trainingDataJointsPosABSfileName));
    trainingModelJointsPosABS.loadModelFromFile(ofToDataPath(trainingModelJointsPosABSfileName)); // TODO: this doesn't seem to work
    trainingModelJointsPosABS.train(trainingDataJointsPosABS); // TODO: put this somewhere that works (doesn't seem to work here in startup())
    trainingDataJointsPosRel.loadDatasetFromFile(ofToDataPath(trainingDataJointsPosRelfileName));
    trainingModelJointsPosRel.loadModelFromFile(ofToDataPath(trainingModelJointsPosRelfileName));
    trainingModelJointsPosRel.train(trainingDataJointsPosRel);
    trainingDataJointsRotAxisA.loadDatasetFromFile(ofToDataPath(trainingDataJointsRotAxisAfileName));
    trainingModelJointsRotAxisA.loadModelFromFile(ofToDataPath(trainingModelJointsRotAxisAfileName));
    trainingModelJointsRotAxisA.train(trainingDataJointsRotAxisA);
    
    //////////////////
    // Kinect Setup //
    //////////////////
    //    setupKinects();
    
    ///////////////
    // Load font //
    ///////////////
    verdana.loadFont(ofToDataPath("verdana.ttf"), 10);
    
    ///////////////////////////////
    // Artist specified settings //
    ///////////////////////////////
    drawFrameRate   = 30;
    drawMirrored    = false;

    /////////////////////////
    // Initialize settings //
    /////////////////////////
    drawNextFrameMilliseconds = 0;
    setDisplayState('i'); // start in installation mode by default (other options are 't' / 'd' for training / debug modes)
    
    /////////////////
    // Load images //
    /////////////////
    //
    // declare local variables
    string          directoryPath;
    ofDirectory     dir;
    int             nFiles;
    //
    // initialize file related properties
    directoriesAll.push_back("images/_540");
    directoriesAll.push_back("images/_1080");
    nFilesToLoad = 64; // for testing purposes only (quick load)
    directoryPath = directoriesAll[0];
    nFiles = dir.listDir(directoryPath);
    maxFilesToLoad = dir.size();
    
    /////////////////////
    // Initialize GUIS //
    /////////////////////
    //
    // Main GUI
    /////////////
    gui = new ofxUISuperCanvas("harlequin");
    //
    // FPS
    gui -> addFPSSlider("fps");
    gui -> addSpacer();
    gui -> addIntSlider("number of files to load", 1, maxFilesToLoad, &nFilesToLoad);
    gui -> addLabelToggle("load images", &loadImagesNow);
    gui -> addSpacer();
    gui -> addTextArea("text", "'+' or '-' to change frame rate");
    gui -> addIntSlider("set fps", 1, 60, &drawFrameRate);
    gui -> addSpacer();
    //
    // Background Color
    gui -> addTextArea("text", "background color");
    gui -> addSlider("red",   0.0, 255.0,    &bgRed   );
    gui -> addSlider("green", 0.0, 255.0,    &bgGreen );
    gui -> addSlider("blue",  0.0, 255.0,    &bgBlue  );
    //
    // Kinect
    gui -> addTextArea("text", "'k' to connect to kinect");
    gui -> addToggle("kinected", &kinected);
    gui -> addSpacer();
    gui -> addTextArea("text", "'m' to mirror kinect input");
    gui -> addToggle("mirror image", &drawMirrored);
    gui -> addSpacer();
    gui -> addTextArea("text", "'i', 'd' or 't' to switch between 'interactive', 'debug' and 'training' modes");
    gui -> addSpacer();
    gui -> addToggle("draw depth image", &drawDepth);
    gui -> addToggle("draw skeletons", &drawSkeletons);
    gui -> addToggle("drawJoints2MSG", &drawJoints2MSG);
    gui -> addSpacer();
    //
    // Debug Messages
    gui -> addToggle("draw MSG", &drawMSG);
    gui -> addSpacer();
    gui -> addTextArea("text", "'h' to hide this panel");
    gui -> addSpacer();
    //
    // Save Settings
    gui -> addLabelButton("save main settings", false);
    gui -> autoSizeToFitWidgets();
    gui -> loadSettings("guiSettings_" + ofToString(displayState) + ".xml");
    ofAddListener(gui -> newGUIEvent, this, &testApp::guiEvent);
    //
    // Color GUI
    /////////////
    guiColor = new ofxUISuperCanvas("harlequin_colors");
    //
    guiColor -> addLabel("image blend mode", OFX_UI_FONT_MEDIUM);
        vector<string> vnamesBlend; vnamesBlend.push_back("0"); vnamesBlend.push_back("a"); vnamesBlend.push_back("+"); vnamesBlend.push_back("-"); vnamesBlend.push_back("*"); vnamesBlend.push_back("scrn");
        ofxUIRadio *radioBlendIMG = guiColor -> addRadio("image blend mode", vnamesBlend, OFX_UI_ORIENTATION_HORIZONTAL);
        radioBlendIMG -> activateToggle("0");
    guiColor -> addTextArea("text", "image tint color");
        guiColor -> addSlider("red",   0.0, 255.0, &imgRed   );
        guiColor -> addSlider("green", 0.0, 255.0, &imgGreen );
        guiColor -> addSlider("blue",  0.0, 255.0, &imgBlue  );
        guiColor -> addSlider("alpha",   0.0, 255.0, &imgAlpha );
    guiColor -> addSpacer();
    //
    guiColor -> addLabel("depth color mode", OFX_UI_FONT_MEDIUM);
        vector<string> vnamesDepthCLR; vnamesDepthCLR.push_back("PSYCHEDELIC_SHADES"); vnamesDepthCLR.push_back("PSYCHEDELIC"); vnamesDepthCLR.push_back("RAINBOW"); vnamesDepthCLR.push_back("CYCLIC_RAINBOW"); vnamesDepthCLR.push_back("BLUES"); vnamesDepthCLR.push_back("BLUES_INV"); vnamesDepthCLR.push_back("GREY"); vnamesDepthCLR.push_back("STATUS");
        ofxUIRadio *radioMode = guiColor -> addRadio("depth color mode", vnamesDepthCLR, OFX_UI_ORIENTATION_VERTICAL);
        radioMode -> activateToggle("BLUES_INV");
    guiColor -> addLabel("depth blend mode", OFX_UI_FONT_MEDIUM);
        ofxUIRadio *radioBlendDepth = guiColor -> addRadio("depth blend mode", vnamesBlend, OFX_UI_ORIENTATION_HORIZONTAL);
        radioBlendDepth -> activateToggle("0");
    guiColor -> addTextArea("text", "depth tint color");
        guiColor -> addSlider("red",   0.0, 255.0, &depthRed   );
        guiColor -> addSlider("green", 0.0, 255.0, &depthGreen );
        guiColor -> addSlider("blue",  0.0, 255.0, &depthBlue  );
        guiColor -> addSlider("alpha",   0.0, 255.0, &depthAlpha );
    guiColor -> addSpacer();
    //
    guiColor -> addLabel("skeleton blend mode", OFX_UI_FONT_MEDIUM);
        ofxUIRadio *radioBlendSkel = guiColor -> addRadio("skel blend mode", vnamesBlend, OFX_UI_ORIENTATION_HORIZONTAL);
        radioBlendSkel -> activateToggle("0");
    guiColor -> addTextArea("text", "skeleton tint color");
        guiColor -> addSlider("red",   0.0, 255.0, &skelRed   );
        guiColor -> addSlider("green", 0.0, 255.0, &skelGreen );
        guiColor -> addSlider("blue",  0.0, 255.0, &skelBlue  );
        guiColor -> addSlider("alpha",   0.0, 255.0, &skelAlpha );
    guiColor -> addSpacer();
    //
    // Save Settings
    guiColor -> addLabelButton("save color settings", false);
    guiColor -> autoSizeToFitWidgets();
    guiColor -> loadSettings("guiSettings_" + ofToString(displayState) + "_color.xml");
    guiColor -> setPosition(ofGetWidth() - 350, 0);
    ofAddListener(guiColor -> newGUIEvent, this, &testApp::guiEvent);
}

void testApp::loadImages(bool load) {
    imageNames.clear();
    images.clear();
    
    if (load) {
        // declare local variables
        string          directoryPath;
        string          filePath;
        ofDirectory     dir;
        int             nFiles;
        ofImage         imgTMP;
        //
        // initialize
        directoryPath = directoriesAll[0];
        nFiles = dir.listDir(directoryPath);
        maxFilesToLoad = dir.size();
        images.resize(maxFilesToLoad);
        imgTMP.setCompression(OF_COMPRESS_ARB); // OF_COMPRESS_NONE || OF_COMPRESS_SRGB || OF_COMPRESS_ARB
        //
        // load files
        if(nFiles) {
            for(int i=0; i < nFilesToLoad; i++) {
                
                // add the image name to a list
                filePath = dir.getPath(i);
                imageNames.push_back(filePath);
                if (imgTMP.loadImage(filePath)) images[i] = imgTMP;
                
                cout << "loading image [" << ofToString(i+1) << "/" << ofToString(nFilesToLoad) << "] : " << filePath << endl;
            }
        } else cout << "Could not find \"" << ofToString(directoryPath) << " directory\n" << endl;
        //
        // Select image to start with
        label = 0;
        img_name = imageNames[label]; // TODO: use this variable to stream images from HD (can set a global vairable called streamFromSSD to determine whether or not to stream images every frame or use our current pre-loading method
    }
    else
    {
        cout << "images unloaded -- imageNames.size() = " << imageNames.size() << "; images.size() = "<< images.size() << endl;
    }
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    
    if(name == "load images")
    {
        loadImages(loadImagesNow);
        //ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        //loadImagesNow = toggle->getValue();
    }
    if (name == "mirror image")
    {
        // reverse the effects of pressing the toggle before calling the keypressed function in order to avoid a double-toggle
        drawMirrored = !drawMirrored;
        keyPressed('m');
    }
    if (name == "kinected")
    {
        if (kinected)
        {
            keyPressed('k');
        }
        else
        {
            keyPressed('x');
        }
    }
    if (name == "image blend mode")
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        imgBlendMode = radio -> getValue();
    }
    if (name == "depth color mode")
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        depthColorMode = radio -> getValue();
    }
    if (name == "depth blend mode")
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        depthBlendMode = radio -> getValue();
    }
    if (name == "skeleton blend mode")
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        skelBlendMode = radio -> getValue();
    }
    if (name == "save main settings")
    {
        gui -> saveSettings("guiSettings_" + ofToString(displayState) + ".xml");
    }
    if (name == "save color settings")
    {
        gui -> saveSettings("guiSettings_" + ofToString(displayState) + "_color.xml");
    }
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
    vector< ofPoint >   singleUserJointsRotAxisA;
    vector< double >    singleUserJointsRotAxisADoubles;
    
    // build joint position vectors
    if (openNIPlayer.getNumTrackedUsers()) {
        for (int j = 0; j < openNIPlayer.getNumTrackedUsers(); ++j) {
            singleUserJointsPosABS.clear();
            singleUserJointsPosRel.clear();
            singleUserJointsRotAxisA.clear();

            // store center positions in both world space and projective space
            ofPoint userJCenter = openNIPlayer.getTrackedUser(j).getCenter();
            ofPoint userJcenterProjective = openNIPlayer.worldToProjective(userJCenter);
//            ofPoint userJcenterProjective = openNIPlayer.getTrackedUser(j).joints[0].getProjectivePosition();
            trackedUserCentersProjective.push_back(
                                                   userJcenterProjective *
                                                   ofPoint(
                                                           float( ofGetWidth() ) / 640.0f,
                                                           float( ofGetHeight()  ) / 480.0f,
                                                           1
                                                           )
                                                   );

            for (int i = 0; i < openNIPlayer.getTrackedUser(j).joints.size(); ++i) {
                ofPoint jointIworldPos = openNIPlayer.getTrackedUser(j).joints[i].getWorldPosition();
                singleUserJointsPosABS.push_back(jointIworldPos);
                singleUserJointsPosRel.push_back(jointIworldPos - userJCenter);
                singleUserJointsRotAxisA.push_back(jointIworldPos);
                // todo: singleUserJointsAxisA.push_back(findAxisAngle(userJCenter, jointIworldPos));
            }
            trackedUserJointsPosABS.push_back(singleUserJointsPosABS);
            trackedUserJointsPosRel.push_back(singleUserJointsPosRel);
            trackedUserJointsRotAxisA.push_back(singleUserJointsRotAxisA);
        }
    }
    
    // build training vectors based on joint positions
    if (trackedUserJointsPosABS.size())
    {
        for (int j = 0; j < trackedUserJointsPosABS.size(); ++j) {
            singleUserJointsPosABSDoubles.clear();
            singleUserJointsPosRelDoubles.clear();
            singleUserJointsRotAxisADoubles.clear();
            for (int i = 0; i < trackedUserJointsPosABS[j].size(); ++i) {
                for (int axis = 0; axis < 3; ++axis)
                {
                    // axis = {0,1,2} which correlates to ofPoint {x, y, z}
                    singleUserJointsPosABSDoubles.push_back(trackedUserJointsPosABS[j][i][axis]);
                    singleUserJointsPosRelDoubles.push_back(trackedUserJointsPosRel[j][i][axis]);
                    singleUserJointsRotAxisADoubles.push_back(trackedUserJointsRotAxisA[j][i][axis]);
                }
            }
            trackedUserJointsPosABSDouble.push_back(singleUserJointsPosABSDoubles);
            trackedUserJointsPosRelDouble.push_back(singleUserJointsPosRelDoubles);
            trackedUserJointsRotAxisADouble.push_back(singleUserJointsRotAxisADoubles);
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(bgRed, bgGreen, bgBlue);
    
    // idle until it's time to draw the next frame
    while (ofGetElapsedTimeMillis() < drawNextFrameMilliseconds) {
        // TODO: put streaming events here
        //  - should this be moved to the "update()" function?
    }
    // calculate new time to wait until drawing next frame
    if (drawFrameRate != 0) {
        drawNextFrameMilliseconds = ofGetElapsedTimeMillis() + 1000 / drawFrameRate;
    }
    
    // draw depth
    if (drawDepth and drawDepthBehind){
        ofPushStyle();

        ofEnableBlendMode(OF_BLENDMODE_ADD); // TODO: implement depthBlendMode
        ofSetColor(depthRed, depthGreen, depthBlue, depthAlpha);
        openNIPlayer.drawDepth(0.0f, 0.0f, float( ofGetWidth() ), float( ofGetHeight() ));
        
        ofPopStyle();
    }

    
    ofPoint jointsCenterProjective;
    ofPoint imgRefPoint;
    ofPoint screenCenter = ofVec3f(ofGetWidth()/2.0f, ofGetHeight()/2.0f, 1.0f);
    
    // Build debug message string
    string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
    
    // draw images
    switch (displayState) {
        case 'i':
            
            // manage style and drawing matrix
            ofPushStyle();
            ofPushMatrix();

            // set colors
            ofSetColor(imgRed, imgGreen, imgBlue, imgAlpha);
            ofBackground(bgRed, bgGreen, bgBlue, bgAlpha);
            ofEnableBlendMode(OF_BLENDMODE_DISABLED); //TODO: implement imgBlendMode
            
            for (int j = 0; j < trackedUserJointsPosABSDouble.size(); ++j) {

                // select label: Relative Position model
                if (trainingModelJointsPosRel.predict(trackedUserJointsPosRelDouble[j]) && imageNames.size())
                {
                    label = trainingModelJointsPosRel.getPredictedClassLabel();
                    //                    cout << "predicted label:" << trainingModelJointsPosRel.getPredictedClassLabel() << endl;
                    
                    if (label > imageNames.size())
                    {
                        label = ofRandom(0, imageNames.size() - 1);
                        cout << "predicted label is too high for imageNames.size() = " << ofToString(imageNames.size()) << endl;
                    }
                    
                    img_name = imageNames[label];
                    //                    cout << "img_name = " << img_name << endl;
                    //                    img_name = ofToString(label) + ".jpg";
                }
                else
                {
                    img_name = "";
                    cout << "trainingModelJointsPosRel could not predict" << endl;
                }
                
                // TODO: find another image if image could not be loaded?
                //if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; }

                if (openNIPlayer.getNumTrackedUsers() >= j) {
                    jointsCenterProjective = trackedUserCentersProjective[j];
                    //                    cout << "jointsCenterProjective = trackedUserCentersProjective[j];" << endl;
                } else {
                    jointsCenterProjective = ofVec3f(screenCenter.x, screenCenter.y, 1400.0f);
                    //                    cout << "jointsCenterProjective = ofVec3f(screenCenter.x, screenCenter.y, 1400.0f);" << endl;
                }
                
                if (imageNames.size())
                {
                    // set image to draw
                    img = images[label];
                    
                    // calculate reference points for drawing
                    if(jointsCenterProjective.z != 0) { // scale
                        imgRefPoint.z = 1500.0f / float( jointsCenterProjective.z);
                    } else {
                        imgRefPoint.z = 1.0f;
                    }
                    float xOffset = float( img.width  ) * imgRefPoint.z / 2.0f;
                    float yOffset = float( img.height  ) * imgRefPoint.z / 2.0f;
                    imgRefPoint.x = jointsCenterProjective.x - xOffset; // left side
                    imgRefPoint.y = jointsCenterProjective.y - yOffset; // top side
                    
                    // TODO: image drawing should be z-sorted so further images draw behind closer ones
                    // draw image at position and scale relative to center of screen and image
                    img.draw(imgRefPoint.x,
                             imgRefPoint.y,
                             img.width * imgRefPoint.z,
                             img.height * imgRefPoint.z);
                    
                    // Build debug message string
                    msg = msg + "\n jointsCenterProjective = " + ofToString(jointsCenterProjective);
                    msg = msg + "\n xOffset = " + ofToString(xOffset);
                    msg = msg + "\n yOffset = " + ofToString(yOffset);
                    msg = msg + "\n imgRef = " + ofToString(imgRefPoint);
                    msg = msg + "\n sceenCenter = " + ofToString(screenCenter);
                }
            }
            
            // reset drawing matrix and style
            ofPopMatrix();
            ofPopStyle();
            
            break;
            
        case 'd': // debug
        case 't': // training
        default:

            // manage style and drawing matrix
            ofPushStyle();
            ofPushMatrix();
            
            // set colors
            ofSetColor(imgRed, imgGreen, imgBlue, imgAlpha);
            ofBackground(bgRed, bgGreen, bgBlue, bgAlpha);
            ofEnableBlendMode(OF_BLENDMODE_DISABLED); // TODO: implement imgBlendMode

            // draw image(s)
            // if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; }
            if (imageNames.size())
            {
                img = images[label];
                float imgRatioX;
                float imgRatioY;
                float imgRatio;

                if (img.width) {
                    imgRatioX = float(ofGetWidth()) / float(img.width);
                } else {
                    imgRatioX = 1.0f;
                }

                if (img.height) {
                    imgRatioY = float(ofGetHeight()) / float(img.height);
                } else {
                    imgRatioY = 1.0f;
                }
                
                if (imgRatioX < imgRatioY) {
                    imgRatio = imgRatioX;
                } else {
                    imgRatio = imgRatioY;
                }
                
                img.draw(
                         (ofGetWidth() - img.width * imgRatio) / 2.0f,
                         (ofGetHeight() - img.height * imgRatio) / 2.0f,
                         img.width * imgRatio,
                         img.height * imgRatio
                         );
            }
            
            // reset drawing matrix and style
            ofPopMatrix();
            ofPopStyle();

            break;
    }
    
    if (drawSkeletons or drawDepth){
        ofPushStyle();
        //  openNIRecorder.drawDebug(0, 0);
        if (drawDepth and !drawDepthBehind)
        {
            ofEnableBlendMode(OF_BLENDMODE_ADD); //TODO: implement depthBlendMode
            ofSetColor(depthRed, depthGreen, depthBlue, depthAlpha);
            openNIPlayer.drawDepth(0.0f, 0.0f, float( ofGetWidth() ), float( ofGetHeight() ));
        }
        if (drawSkeletons)
        {
            ofSetColor(skelRed, skelGreen, skelBlue, skelAlpha);
            ofEnableBlendMode(OF_BLENDMODE_SUBTRACT); //TODO: implement skelBlendMode
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
        //        cout << msg << endl;
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

    delete gui;
}

//--------------------------------------------------------------
void testApp::setDisplayState(char newState) {
    bool undefinedState = false;
    
    switch (newState) {
        case 't': // training
            // fall through (intentional)
        case 'd': // debug
            // fall through (intentional)
        case 'i':

            // save & train model before switching modes
            saveData();
            saveModel();
            
            break;

        default:
            undefinedState = true;
            break;
    }
    

    if (!undefinedState && gui)
    {
        displayState = newState;
        gui -> loadSettings("guiSettings_" + ofToString(displayState) + ".xml");
        guiColor -> loadSettings("guiSettings_" + ofToString(displayState) + "_color.xml");
        guiColor -> setPosition(ofGetWidth() - 350, 0);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

    // string fileName = "test_20140501_1904.oni";
    int cloudRes = -1;
    bool fileWritten;
    string testJointBuff;
    
    ofImage img;
    
    switch (key) {
            
        case 'h':
            gui ->toggleVisible();
            guiColor ->toggleVisible();
            break;
            
        case 'b': // NOTE: updated to 'b' for BUILD DATA
            if (displayState == 'i') break; // do not train data during installation mode

            // Store data to associate with currently displayed image
            //  - joint positions (0–15) — 0 = center, 1–15 = joints
            
            if (displayState == 'd'){
                trainingDataJointsPosABS.addSample(label, trackedUserJointsPosABSDouble[0]);
                trainingDataJointsPosRel.addSample(label, trackedUserJointsPosRelDouble[0]);
                trainingDataJointsRotAxisA.addSample(label, trackedUserJointsRotAxisADouble[0]);
            } else {
                // TODO: display some kind of error message that says data can only be saved in debug mode?
            }
            
            break;
        
        case '<':
        case ',':
        case '[':
           
            saveData();
            
            if (displayState == 'i') break; // do not train data during installation mode
            
            // display previous image in database
            if (label > 0) label--;
            img_name = imageNames[label];

            break;
        
        case '>':
        case '.':
        case ']':
            
            saveData();
            
            if (displayState == 'i') break; // do not train data during installation mode

            // display next image in database
            if (label < imageNames.size()) label++;
            img_name = imageNames[label];

            break;
        
        case 'r': // random image
            
            saveData();

            if (displayState == 'i') break; // do not train data during installation mode
            
            // display random image from database
            label = getRandomExcluding(0, imageNames.size() - 1, label);
            img_name = imageNames[label];

            break;
        
        case 's': // NOTE: Moved save functionality here to minimize lagging during data building phase
            if (displayState == 'i') break; // do not train data during installation mode
            
            saveData();
            saveModel();
            
            break;
            
        case 'c':
            if (displayState == 'i') break; // do not train data during installation mode

            saveData();
            saveModel();
            
            // TODO:
            //  - separate out into a class so it can be called from both here and draw()
            //  - include prediction mode as one of the arguments
            
            // select label: Relative Position model
            if (trainingModelJointsPosRel.predict(trackedUserJointsPosRelDouble[0]))
            {
                label = trainingModelJointsPosRel.getPredictedClassLabel();
                
                if (label > imageNames.size())
                {
                    label = ofRandom(0, imageNames.size() - 1);
                    cout << "predicted label is too high for imageNames.size() = " << ofToString(imageNames.size()) << endl;
                }
                
                img_name = imageNames[label];
            }
            else
            {
                cout << "trainingModelJointsPosRel could not predict" << endl;
            }
            break;
        
        case 'i': // interactive mode
            // fall through (intentional)
        case 't': // training
            // fall through (intentional)
        case 'd': // debug
            
            setDisplayState(key);

            break;
            
        case '=': // increase drawFrameRate
            // fall through (intentional)
        case '+': // increase drawFrameRate
            
            if (drawFrameRate < 90) drawFrameRate = drawFrameRate + 5;
            
            break;
            
        case '_': // decrease drawFrameRate
            // fall through (intentional)
        case '-': // decrease drawFrameRate
            
            if (drawFrameRate > 5) drawFrameRate = drawFrameRate - 5;
            
            break;

        case 'm':
            
            drawMirrored = !drawMirrored;
            openNIPlayer.setMirror(drawMirrored);
            
            break;
        
        case 'x':

            stopKinects();
            kinected = false;
            break;
        
        case 'k':
            setupKinects(); // TODO: debug this, doesn't seem to work properly after kinects have been stopped.
            kinected = true;
            break;
    }
}

//--------------------------------------------------------------
void testApp::saveData(){
    trainingDataJointsPosABS.saveDatasetToFile(ofToDataPath(trainingDataJointsPosABSfileName));
    trainingDataJointsPosRel.saveDatasetToFile(ofToDataPath(trainingDataJointsPosRelfileName));
    trainingDataJointsRotAxisA.saveDatasetToFile(ofToDataPath(trainingDataJointsRotAxisAfileName));
}

//--------------------------------------------------------------
void testApp::saveModel(){
    trainingModelJointsPosABS.train(trainingDataJointsPosABS);
    trainingModelJointsPosABS.saveModelToFile(ofToDataPath(trainingModelJointsPosABSfileName));
    
    trainingModelJointsPosRel.train(trainingDataJointsPosRel);
    trainingModelJointsPosRel.saveModelToFile(ofToDataPath(trainingModelJointsPosRelfileName));
    
    trainingModelJointsRotAxisA.train(trainingDataJointsRotAxisA);
    trainingModelJointsRotAxisA.saveModelToFile(ofToDataPath(trainingModelJointsRotAxisAfileName));
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
    
    //    if (kinectsInitialized) {
    //        openNIPlayer.start();
    //        return;
    //    }
    
    openNIPlayer.setup();
    openNIPlayer.addDepthGenerator();
    openNIPlayer.addImageGenerator();
    openNIPlayer.setRegister(true);
    openNIPlayer.setMirror(drawMirrored);
    openNIPlayer.addUserGenerator();
    openNIPlayer.setMaxNumUsers(4); // was 2
    openNIPlayer.setDepthColoring(COLORING_BLUES_INV);
    openNIPlayer.start();
    
    kinectsInitialized = true;
}
void testApp::stopKinects() {
    ofSetLogLevel(OF_LOG_VERBOSE);
    
//    openNIPlayer.removeDepthGenerator();
//    openNIPlayer.removeImageGenerator();
//    openNIPlayer.setRegister(false);
//    openNIPlayer.removeUserGenerator();
    openNIPlayer.stop();
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
