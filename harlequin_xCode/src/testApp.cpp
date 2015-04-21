#include "testApp.h"


// what is this? Why is it re-defining ofxOpenNIUser?
class myUser : public ofxOpenNIUser {
public:
    void test(){
        cout << "test" << endl;
    }
};

///////////
// TODO: //
///////////
//  - fix GUI implementation of blendmodes (radio buttons)
//  - experiment with "nearest" method of label prediction for more movement
//  - redesign image / training system to be more malleable
//  - do not display the same image more than once every 3 frames
//  - slow down frame rate to 30, or even as low as 12 or so?
//  - display data associated with current image
//  - save image positioning data —- translation, scale, and rotation controls for current image
//      > rotation
//      > scale (might need to just store corners or dimensions so that image can be scaled independent of file size changes
//  - joint rotations (axis-angles?)
//  - filters for file tags (drawing, photo, male, female, dancer, pedestrian, public figure, etc.)
//      > simply use separate directory structures?
//  - query for short list of potential images (instead of returning just one as we do now)
//      > could be done in pre-processing and save a list of images closest to the each image


//--------------------------------------------------------------
void testApp::setup() {
    
    //////////////////
    // Image Editor //
    //////////////////
    imgEditor = new harlequinIMGEditor();
    imgEditor->setup();
    
    ///////////////////////////
    // Image Data Properties //
    ///////////////////////////
    trainedImagesDirectory = "images/hand_drawings/LT/";
//    trainedImagesDirectory = "images/hand_drawings/V/";
    // TODO: make image directory selection dynamic (1. include text input in GUI, 2. separate out data loading into a function that can be called when target data set is changed)
    directoriesAll.push_back(trainedImagesDirectory + "_540"); // i = 0
    directoriesAll.push_back(trainedImagesDirectory + "_1080"); // i = 1
    //
    // declare local variables
    string          directoryPath;
    ofDirectory     dir;
    int             nFiles;
    
    //
    nFilesToLoad = 64; // for testing purposes only (quick load)
    directoryPath = directoriesAll[0]; // loading _540 images
    imageScale = 2.0f; // TODO: scale image dynamically based on artist-specified data per image
    nFiles = dir.listDir(directoryPath);
    maxFilesToLoad = dir.size();

    
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
    

    //////////////
    // OSC INIT //
    //////////////
    sender.setup(HOST, PORT);
    myHost = HOST;
    myPort = ofToString(PORT);
    
    
    /////////////////////
    // Initialize GUIS //
    /////////////////////
    //
    //------------
    // Main GUI --
    //////////////
    gui = new ofxUISuperCanvas("harlequin");
    ofAddListener(gui -> newGUIEvent, this, &testApp::guiEvent);
    gui -> addSpacer();
    gui -> addTextArea("text", "'h' to hide this panel", OFX_UI_FONT_SMALL);
    //    gui -> addLabel("text", "'h' to hide this panel");
    gui -> addSpacer();
    //
    // Switch display modes
    gui -> addLabel("application mode: ");
    vector<string> appModes; appModes.push_back("i"); appModes.push_back("d"); appModes.push_back("t");
    ofxUIRadio *radioAppMode = gui -> addRadio("application mode", appModes, OFX_UI_ORIENTATION_HORIZONTAL);
    gui -> addTextArea("text", "'i', 'd' or 't' to switch between 'interactive', 'debug' and 'training' modes", OFX_UI_FONT_SMALL);
    gui -> addSpacer();
    //
    // FPS
    gui -> addFPSSlider("fps");
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
    gui -> addSpacer();
    //
    // Kinect
    gui -> addTextArea("text", "'k' to connect to kinect");
    gui -> addToggle("kinected", &kinected);
    gui -> addSpacer();
    gui -> addTextArea("text", "'m' to mirror kinect input");
    gui -> addToggle("mirror image", &drawMirrored);
    gui -> addToggle("draw depth image", &drawDepth);
    gui -> addToggle("draw depth behind", &drawDepthBehind);
    gui -> addToggle("draw skeletons", &drawSkeletons);
    gui -> addToggle("add joints 2 MSG", &drawJoints2MSG);
    //
    // Debug Messages
    gui -> addToggle("draw MSG", &drawMSG);
    gui -> addSpacer();
    //
    // OSC Toggles
    gui -> addToggle("setup OSC", &setupOSC);
    gui -> addTextInput("host", "address")->setAutoClear(false);
    gui -> addTextInput("port", "port")->setAutoClear(false);
    gui -> addToggle("send OSC", &sendOSC);
    gui -> addSpacer();
    //
    // Load files
    gui -> addIntSlider("number of files to load", 0, maxFilesToLoad, &nFilesToLoad);
    gui -> addLabelToggle("load images", &loadImagesNow);
    gui -> addSpacer();
    //
    // Save Settings
    gui -> addLabelButton("save main settings", false);
    gui -> autoSizeToFitWidgets();
    //
    //-------------
    // Color GUI --
    ///////////////
    guiColor = new ofxUISuperCanvas("harlequin colors");
    ofAddListener(guiColor -> newGUIEvent, this, &testApp::guiEvent);
    guiColor -> addSpacer();
    //
    guiColor -> addLabel("image color settings", OFX_UI_FONT_MEDIUM);
    vector< string > vnamesBlendIMG; vnamesBlendIMG.push_back("i0"); vnamesBlendIMG.push_back("iA"); vnamesBlendIMG.push_back("i+"); vnamesBlendIMG.push_back("i-"); vnamesBlendIMG.push_back("i*"); vnamesBlendIMG.push_back("iS");
    ofxUIRadio *radioBlendIMG = guiColor -> addRadio("image blend mode", vnamesBlendIMG, OFX_UI_ORIENTATION_HORIZONTAL);
    guiColor -> addToggle("invert image", &imgInvertColors);
    guiColor -> addSlider("image red",   0.0, 255.0, &imgRed   );
    guiColor -> addSlider("image green", 0.0, 255.0, &imgGreen );
    guiColor -> addSlider("image blue",  0.0, 255.0, &imgBlue  );
    guiColor -> addSlider("image alpha", 0.0, 255.0, &imgAlpha );
    guiColor -> addSpacer();
    //
    guiColor -> addLabel("depth image settings", OFX_UI_FONT_MEDIUM);
    vector< string > vnamesDepthCLR; vnamesDepthCLR.push_back("PSYCHEDELIC_SHADES"); vnamesDepthCLR.push_back("PSYCHEDELIC"); vnamesDepthCLR.push_back("RAINBOW"); vnamesDepthCLR.push_back("CYCLIC_RAINBOW"); vnamesDepthCLR.push_back("BLUES"); vnamesDepthCLR.push_back("BLUES_INV"); vnamesDepthCLR.push_back("GREY"); vnamesDepthCLR.push_back("STATUS");
    ofxUIRadio *radioMode = guiColor -> addRadio("depth color mode", vnamesDepthCLR, OFX_UI_ORIENTATION_VERTICAL);
    vector< string > vnamesBlendDEPTH; vnamesBlendDEPTH.push_back("d0"); vnamesBlendDEPTH.push_back("dA"); vnamesBlendDEPTH.push_back("d+"); vnamesBlendDEPTH.push_back("d-"); vnamesBlendDEPTH.push_back("d*"); vnamesBlendDEPTH.push_back("dS");
    ofxUIRadio *radioBlendDepth = guiColor -> addRadio("depth blend mode", vnamesBlendDEPTH, OFX_UI_ORIENTATION_HORIZONTAL);
    guiColor -> addSlider("depth red",   0.0, 255.0, &depthRed   );
    guiColor -> addSlider("depth green", 0.0, 255.0, &depthGreen );
    guiColor -> addSlider("depth blue",  0.0, 255.0, &depthBlue  );
    guiColor -> addSlider("depth alpha", 0.0, 255.0, &depthAlpha );
    guiColor -> addSpacer();
    //
    guiColor -> addLabel("skeleton drawing settings", OFX_UI_FONT_MEDIUM);
    vector< string > vnamesBlendSKEL; vnamesBlendSKEL.push_back("s0"); vnamesBlendSKEL.push_back("sA"); vnamesBlendSKEL.push_back("s+"); vnamesBlendSKEL.push_back("s-"); vnamesBlendSKEL.push_back("s*"); vnamesBlendSKEL.push_back("sS");
    ofxUIRadio *radioBlendSkel = guiColor -> addRadio("skeleton blend mode", vnamesBlendSKEL, OFX_UI_ORIENTATION_HORIZONTAL);
    guiColor -> addSlider("skel red",   0.0, 255.0, &skelRed   );
    guiColor -> addSlider("skel green", 0.0, 255.0, &skelGreen );
    guiColor -> addSlider("skel blue",  0.0, 255.0, &skelBlue  );
    guiColor -> addSlider("skel alpha", 0.0, 255.0, &skelAlpha );
    guiColor -> addSpacer();
    //
    // Save Settings
    guiColor -> addLabelButton("save color settings", false);
    guiColor -> autoSizeToFitWidgets();
    

    /////////////////////////
    // Initialize settings //
    /////////////////////////
    drawNextFrameMilliseconds = 0;
    setDisplayState('d'); // start in installation mode by default (other options are 't' / 'd' for training / debug modes) // this load gui and guiColor settings, so it should appear after those are created
    
    ///////////////
    // Test Data //
    ///////////////
    // user joints for testing while Kinect is offline
    setupTestUserJoints();
}

void testApp::loadImages(bool load) {

    if (load) {
        
        /////////////////
        // load images //
        /////////////////
        // declare local variables
        string          directoryPath;
        string          filePath;
        ofDirectory     dir;
        int             nFilesInDir;
        int             nFilesLoaded = images.size();
        ofImage         imgTMP;
        //
        // initialize
        directoryPath = directoriesAll[0];
        nFilesInDir = dir.listDir(directoryPath);
        maxFilesToLoad = dir.size();//TODO: update maximum for GUI slider named "number of files to load"
        images.resize(nFilesToLoad);
//        imageNames.resize(nFilesToLoad);
        imgTMP.setCompression(OF_COMPRESS_ARB); // OF_COMPRESS_NONE || OF_COMPRESS_SRGB || OF_COMPRESS_ARB
        //
        // load files
        if(nFilesInDir) {
            for(int i = nFilesLoaded; i < maxFilesToLoad; ++i) {
                if (nFilesLoaded >= nFilesToLoad) break;

                // add the image name to a list
                filePath = dir.getPath(i);
                if (imgTMP.loadImage(filePath))
                {
//                    imageNames[nFilesLoaded] = filePath;
                    images[nFilesLoaded] = imgTMP;
                    nFilesLoaded++;
                    cout << "loaded image [" << ofToString(nFilesLoaded) << "/" << ofToString(nFilesToLoad) << "] : " << filePath << endl;
                }
            }
        } else cout << "Could not find \"" << ofToString(directoryPath) << " directory\n" << endl;
        //
        // Select image to start with
        label = 0;
        if (imageNames.size()) img_name = imageNames[label];
        // TODO: use "img_name" to stream images from HD (can set a global vairable called streamFromSSD to determine whether or not to stream images every frame or use our current pre-loading method
        // TODO: clear out the imageNames array code after the data handling re-structuring takes place
        
        
        /////////////////////////////////////////
        // load training data and setup models //
        /////////////////////////////////////////
        trainingDataJointsPosABSfileName    = trainedImagesDirectory + "JointsPosABSdata.txt"; // TODO: make these relative to selected directories
        trainingModelJointsPosABSfileName   = trainedImagesDirectory + "JointsPosABSmodel.txt";
        trainingDataJointsPosRelfileName    = trainedImagesDirectory + "JointsPosReldata.txt";
        trainingModelJointsPosRelfileName   = trainedImagesDirectory + "JointsPosRelmodel.txt";
        trainingDataJointsRotAxisAfileName  = trainedImagesDirectory + "JointsRotAxisAdata.txt";
        trainingModelJointsRotAxisAfileName = trainedImagesDirectory + "JointsRotAxisAmodel.txt";
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
        trainingDataJointsPosRel.loadDatasetFromFile(ofToDataPath(trainingDataJointsPosRelfileName));
        trainingDataJointsRotAxisA.loadDatasetFromFile(ofToDataPath(trainingDataJointsRotAxisAfileName));
        cout << "loaded training data" << endl;
        
        trainModelsNow = true;

        // TODO: implement the suggested pipeline setup
        GRT::SVM trainingModelJointsPosABS(GRT::SVM::LINEAR_KERNEL);
        GRT::SVM trainingModelJointsPosRel(GRT::SVM::LINEAR_KERNEL);
        GRT::SVM trainingModelJointsRotAxisA(GRT::SVM::LINEAR_KERNEL);
    
    }
    else
    {
        // clear image arrays to unload all images
//        imageNames.clear();
        images.clear();
        // TODO: clean up prediction data & models as well
        cout << "images unloaded -- images.size() = "<< images.size() << endl;
    }
}

void testApp::trainModels()
{
    
    // TODO: loading models only works when called outside startup()——could be fixed by implementing the suggested pipeline setup?
    trainingModelJointsPosABS.loadModelFromFile(ofToDataPath(trainingModelJointsPosABSfileName));
    trainingModelJointsPosRel.loadModelFromFile(ofToDataPath(trainingModelJointsPosRelfileName));
    trainingModelJointsRotAxisA.loadModelFromFile(ofToDataPath(trainingModelJointsRotAxisAfileName));

    // TODO: training model data only works when called outside startup()——could this also be fixed by implementing the suggested pipeline setup?
//    trainingModelJointsPosABS.train(trainingDataJointsPosABS);
//    trainingModelJointsPosRel.train(trainingDataJointsPosRel);
//    trainingModelJointsRotAxisA.train(trainingDataJointsRotAxisA);
    
    cout << "trained models" << endl;

    trainModelsNow = false;
}

void testApp::guiEvent(ofxUIEventArgs &e) {
    string nameStr = e.widget->getName();
    int kind = e.widget->getKind();
    
    /*  */ if(nameStr == "load images") {
        loadImages(loadImagesNow);
        //ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        //loadImagesNow = toggle->getValue();
    } else if(nameStr == "invert image") {
        // TODO: debug "invert images" only when toggle is updated.
//        if (imgColorsAreInverted != imgInvertColors) {
//            for (int imgI=0; imgI < images.size(); ++imgI){
//                ofImage* imgTMP = &images[imgI];
//                invertImage(imgTMP);
//            }
//            imgColorsAreInverted = imgInvertColors;
//        }
    } else if (nameStr == "mirror image") {
        // reverse the effects of pressing the toggle before calling the keypressed function in order to avoid a double-toggle
        drawMirrored = !drawMirrored;
        keyPressed('m');
        
        
    } else if (nameStr == "kinected") {
        if (kinected)
        {
            keyPressed('k');
        }
        else
        {
            keyPressed('x');
        }
        
        
    } else if (nameStr == "host") {
        ofxUITextInput *ti = (ofxUITextInput *) e.widget;
        if(ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER "<< nameStr<<": ";
            myHost = ti->getTextString();
            cout << myPort << endl;
        }
        else if(ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
            myHost = ti->getTextString();
            cout << myPort << endl;
        }
        //
    } else if(nameStr == "port") {
        ofxUITextInput *ti = (ofxUITextInput *) e.widget;
        if(ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER" << nameStr << ": " ;
            myPort = ti->getTextString();
            cout << myPort << endl;
        }
        else if(ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
            myPort = ti->getTextString();
        }
       
        
    } else if (nameStr == "application mode") {
        ofxUIRadio *radioAppMode = (ofxUIRadio *) e.widget;
        switch (radioAppMode -> getValue()) {
            case 0: // interactive
                if (displayState != 'i') setDisplayState('i');
                break;
                
            case 1: // debug
                if (displayState != 'd') setDisplayState('d');
                break;
                
            case 2: // training
                if (displayState != 't') setDisplayState('t');
                break;
                
            default:
                break;
        }
//    } else if (nameStr == "i" || nameStr == "d" || nameStr == "t") {
//        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
//        if (toggle -> getValue()) //do something
//            ;
//            
    } else if (nameStr == "save main settings") {
        gui -> saveSettings("guiSettings_" + ofToString(displayState) + ".xml");
    } else if (nameStr == "save color settings") {
        guiColor -> saveSettings("guiSettings_" + ofToString(displayState) + "_color.xml");
        
        
    } else if (nameStr == "image blend mode") {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        imgBlendMode = radio -> getValue();
    } else if (nameStr == "i0"||nameStr == "iA"||nameStr == "i+"||nameStr == "i-"||nameStr == "i*"||nameStr == "iS") {
        ofxUIToggle * toggle = (ofxUIToggle *) e.widget;
        /**/ if (nameStr == "i0" && toggle -> getValue()) imgBlendMode = 0;
        else if (nameStr == "iA" && toggle -> getValue()) imgBlendMode = 1;
        else if (nameStr == "i+" && toggle -> getValue()) imgBlendMode = 2;
        else if (nameStr == "i-" && toggle -> getValue()) imgBlendMode = 3;
        else if (nameStr == "i*" && toggle -> getValue()) imgBlendMode = 4;
        else if (nameStr == "iS" && toggle -> getValue()) imgBlendMode = 5;
    } else if (nameStr == "depth color mode"||nameStr == "PSYCHEDELIC_SHADES"||nameStr == "PSYCHEDELIC"||nameStr == "RAINBOW"||nameStr == "CYCLIC_RAINBOW"||nameStr == "BLUES"||nameStr == "BLUES_INV"||nameStr == "GREY"||nameStr == "STATUS") {
        if (nameStr == "depth color mode") {
            ofxUIRadio *radio = (ofxUIRadio *) e.widget;
            depthColorMode = radio -> getValue();
        } else {
            ofxUIToggle * toggle = (ofxUIToggle *) e.widget;
            /**/ if (nameStr == "PSYCHEDELIC_SHADES"    && toggle -> getValue()) depthColorMode = 0;
            else if (nameStr == "PSYCHEDELIC"           && toggle -> getValue()) depthColorMode = 1;
            else if (nameStr == "RAINBOW+"              && toggle -> getValue()) depthColorMode = 2;
            else if (nameStr == "CYCLIC_RAINBOW-"       && toggle -> getValue()) depthColorMode = 3;
            else if (nameStr == "BLUES*"                && toggle -> getValue()) depthColorMode = 4;
            else if (nameStr == "BLUES_INV"             && toggle -> getValue()) depthColorMode = 5;
            else if (nameStr == "GREY"                  && toggle -> getValue()) depthColorMode = 6;
            else if (nameStr == "STATUS"                && toggle -> getValue()) depthColorMode = 7;
        }
        openNIPlayer.setDepthColoring((DepthColoring)depthColorMode);
    } else if (nameStr == "depth blend mode") {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        depthBlendMode = radio -> getValue();
    } else if (nameStr == "d0"||nameStr == "dA"||nameStr == "d+"||nameStr == "d-"||nameStr == "d*"||nameStr == "dS") {
        ofxUIToggle * toggle = (ofxUIToggle *) e.widget;
        /**/ if (nameStr == "d0" && toggle -> getValue()) depthBlendMode = 0;
        else if (nameStr == "dA" && toggle -> getValue()) depthBlendMode = 1;
        else if (nameStr == "d+" && toggle -> getValue()) depthBlendMode = 2;
        else if (nameStr == "d-" && toggle -> getValue()) depthBlendMode = 3;
        else if (nameStr == "d*" && toggle -> getValue()) depthBlendMode = 4;
        else if (nameStr == "dS" && toggle -> getValue()) depthBlendMode = 5;
    } else if (nameStr == "skleton blend mode") {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        skelBlendMode = radio -> getValue();
    } else if (nameStr == "s0"||nameStr == "sA"||nameStr == "s+"||nameStr == "s-"||nameStr == "s*"||nameStr == "sS") {
        ofxUIToggle * toggle = (ofxUIToggle *) e.widget;
        /**/ if (nameStr == "s0" && toggle -> getValue()) skelBlendMode = 0;
        else if (nameStr == "sA" && toggle -> getValue()) skelBlendMode = 1;
        else if (nameStr == "s+" && toggle -> getValue()) skelBlendMode = 2;
        else if (nameStr == "s-" && toggle -> getValue()) skelBlendMode = 3;
        else if (nameStr == "s*" && toggle -> getValue()) skelBlendMode = 4;
        else if (nameStr == "sS" && toggle -> getValue()) skelBlendMode = 5;
    
        
    } else { // default
        if(ofGetLogLevel() == OF_LOG_VERBOSE) cout << "[verbose] testApp::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
    }
    
}

//--------------------------------------------------------------
void testApp::update(){
    //    openNIRecorder.update();
    openNIPlayer.update();
    if (trainModelsNow) trainModels();

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
    
    // local variables for looping
    int numUsers;
    int numJoints;
    if (kinected) {
        numUsers = openNIPlayer.getNumTrackedUsers();
        if (numUsers) numJoints = openNIPlayer.getTrackedUser(0).joints.size();
    } else {
        numUsers = 3;
        numJoints = 15;
    }
    
    // build joint position vectors
    for (int j = 0; j < numUsers; j++) {
        singleUserJointsPosABS.clear();
        singleUserJointsPosRel.clear();
        singleUserJointsRotAxisA.clear();

        if (kinected) {
            // store center positions in both world space and projective space
            ofPoint userJCenter = openNIPlayer.getTrackedUser(j).getCenter();
            ofPoint userJcenterProjective = openNIPlayer.worldToProjective(userJCenter);
            //            ofPoint userJcenterProjective = openNIPlayer.getTrackedUser(j).joints[0].getProjectivePosition(); // TODO: use the root position of the hips for relative joint data instead?
            trackedUserCentersProjective.push_back(
                                                   userJcenterProjective *
                                                   ofPoint(
                                                           float( ofGetWidth() ) / 640.0f,
                                                           float( ofGetHeight()  ) / 480.0f,
                                                           1
                                                           )
                                                   );
            for (int i = 0; i < numJoints; ++i) {
                ofPoint jointIworldPos = openNIPlayer.getTrackedUser(j).joints[i].getWorldPosition();
                singleUserJointsPosABS.push_back(jointIworldPos);
                singleUserJointsPosRel.push_back(jointIworldPos - userJCenter);
                singleUserJointsRotAxisA.push_back(jointIworldPos);
                // TODO: calculate axis angles properly and store with something like singleUserJointsAxisA.push_back(findAxisAngle(userJCenter, jointIworldPos));
            }
        } else {
            for (int j = 0; j < numUsers; j++)
                trackedUserCentersProjective.push_back(ofPoint(
                                                               float( ofGetWidth() ) * (j+1.0f) / (numUsers+1.0f),
                                                               float( ofGetHeight()  ) / 2.0f,
                                                               2500.0f
                                                               )
                                                       );
            
            singleUserJointsPosABS = testUserJoints[j];
            singleUserJointsPosRel = testUserJoints[j];
            singleUserJointsRotAxisA = testUserJoints[j];
        }
        trackedUserJointsPosABS.push_back(singleUserJointsPosABS);
        trackedUserJointsPosRel.push_back(singleUserJointsPosRel);
        trackedUserJointsRotAxisA.push_back(singleUserJointsRotAxisA);
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
                    // axis = should be {0,1,2} which correlates to ofPoint {x, y, z}
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

        ofEnableBlendMode((ofBlendMode)depthBlendMode);
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
        case 'd': // debug
           
            // manage style and drawing matrix
            ofPushStyle();
            ofPushMatrix();

            // set colors
            ofSetColor(imgRed, imgGreen, imgBlue, imgAlpha);
            ofEnableBlendMode((ofBlendMode)imgBlendMode);
            // TODO: change data type for GUI variables like imBlendMode et al, and cast data as proper format when changed by the GUI
            
            for (int j = 0; j < trackedUserJointsPosABSDouble.size(); ++j) {

                // select label: Relative Position model
                if (trainingModelJointsPosRel.predict(trackedUserJointsPosRelDouble[j]) && images.size())
                {
                    label = trainingModelJointsPosRel.getPredictedClassLabel();
                    //                    cout << "predicted label:" << label << endl;
                    
                    if (label > images.size()) // if predicted label image hasn't been loaded, display a random image
                    {
                        label = ofRandom(0, images.size() - 1);
//                        cout << "predicted label is too high —- images.size() = " << ofToString(images.size()) << "; label = " << label << endl;
//                        cout << " - displaying a random image instead." << endl;
                    }
                    
//                    img_name = imageNames[label];
                    //                    cout << "img_name = " << img_name << endl;
                }
                else
                {
//                    img_name = "";
                    cout << "trainingModelJointsPosRel could not predict" << endl;
                }
                
                // TODO: implement SSD option selection GUI & implemnet loading images directly from HD
                //if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; //find another image if image could not be loaded}

                if (trackedUserCentersProjective.size() >= j+1) {
                    jointsCenterProjective = trackedUserCentersProjective[j];
                    //                    cout << "jointsCenterProjective = trackedUserCentersProjective[j];" << endl;
                } else {
                    jointsCenterProjective = ofVec3f(screenCenter.x, screenCenter.y, 1400.0f);
                    //                    cout << "jointsCenterProjective = ofVec3f(screenCenter.x, screenCenter.y, 1400.0f);" << endl;
                }
                
                if (images.size())
                {
                    // set image to draw
                    img = images[label];
                    
                    // calculate reference points for drawing
                    if(jointsCenterProjective.z != 0) { // scale
                        imgRefPoint.z = 1500.0f / float( jointsCenterProjective.z);
                    } else {
                        imgRefPoint.z = 1.0f;
                    }
                    float xOffset = float( img.width  ) * imgRefPoint.z * imageScale / 2.0f;
                    float yOffset = float( img.height  ) * imgRefPoint.z * imageScale / 2.0f;
                    imgRefPoint.x = jointsCenterProjective.x - xOffset; // left side
                    imgRefPoint.y = jointsCenterProjective.y - yOffset; // top side
                    
                    img.mirror(0, drawMirrored);
                    if (imgInvertColors) {
                        invertImage(img);
                    }
                    
                    // TODO: image drawing should be z-sorted so further images draw behind closer ones
                    // draw image at position and scale relative to center of screen and image
                    img.draw(imgRefPoint.x,
                             imgRefPoint.y,
                             img.width * imgRefPoint.z * imageScale,
                             img.height * imgRefPoint.z * imageScale);
                    
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
            
        case 't': // training
        default:

            // manage style and drawing matrix
            ofPushStyle();
            ofPushMatrix();
            
            // set colors
            ofSetColor(imgRed, imgGreen, imgBlue, imgAlpha);
            ofEnableBlendMode((ofBlendMode)imgBlendMode);

            // draw image(s)
            // if (img.loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; }
            if (images.size())
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
                
                img.mirror(0, drawMirrored);
                if (imgInvertColors) {
                    invertImage(img);
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
            ofEnableBlendMode((ofBlendMode)depthBlendMode);
            ofSetColor(depthRed, depthGreen, depthBlue, depthAlpha);
            openNIPlayer.drawDepth(0.0f, 0.0f, float( ofGetWidth() ), float( ofGetHeight() ));
        }
        if (drawSkeletons)
        {
            ofSetColor(skelRed, skelGreen, skelBlue, skelAlpha);
            ofEnableBlendMode((ofBlendMode)skelBlendMode);
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
    
    if(setupOSC){
        sender.setup(myHost, ofToInt(myPort));
        ofxOscMessage m;
        m.setAddress("/test" );
        sender.sendMessage(m);
        setupOSC = false;
    }
    
    if (sendOSC){
        if (trackedUserCentersProjective.size() > 0) {
            for (int i=0; i < trackedUserCentersProjective.size(); ++i) {
                ofxOscMessage m;
                m.setAddress("/POS" + ofToString(i) );
                m.addFloatArg( trackedUserCentersProjective[i].x );
                m.addFloatArg( trackedUserCentersProjective[i].y );
                m.addFloatArg( trackedUserCentersProjective[i].z );
                sender.sendMessage(m);
            }
        }
        
        
    }
    
    
    ofPopStyle();
}


//--------------------------------------------------------------
void testApp::invertImage(ofImage* imgPTRlocal) {
    ofPixels imgPX = imgPTRlocal->getPixelsRef();
    ofTexture imgTEX = imgPTRlocal->getTextureReference();
    invertImage(imgPX, imgTEX);
}

void testApp::invertImage(ofImage &imgREF) {
    // TODO: optimize image inversions. seems to slow down FPS to ~ 22 from ~55 on my ancient MacBook Pro
    ofPixels imgPX = imgREF.getPixelsRef();
    ofTexture imgTEX = imgREF.getTextureReference();
    invertImage(imgPX, imgTEX);    
}

void testApp::invertImage(ofPixels &imgPX, ofTexture &imgTEX) {
    int iPX = 0;
    while ( iPX < imgPX.size() ) {
        imgPX[iPX] = 512.0f - imgPX[iPX];
        iPX++;
    }
    imgTEX.loadData(imgPX);
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
            // TODO: log or cout an error messsage for undefined state change?
            undefinedState = true;
            break;
    }
    

    if (!undefinedState)
    {
        displayState = newState;
        
        // Load GUI settings // TODO: load setting xml files into memory to speed up switching states?
        if (gui) {
            gui -> loadSettings("guiSettings_" + ofToString(displayState) + ".xml");
        }
        if (guiColor) {
            guiColor -> loadSettings("guiSettings_" + ofToString(displayState) + "_color.xml");
            
            // run windowResized method to reset guiColor's position
            windowResized();
        }
    } else {
        // TODO: Throw display state change error if state is not recognized
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

    int cloudRes = -1;
    bool fileWritten;
    string testJointBuff;
    bool displayStateKeyed;
    
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
            
            if ((displayState == 'd' || displayState == 't') && trackedUserJointsPosABS.size()){
                trainingDataJointsPosABS.addSample(label, trackedUserJointsPosABSDouble[0]);
                trainingDataJointsPosRel.addSample(label, trackedUserJointsPosRelDouble[0]);
                trainingDataJointsRotAxisA.addSample(label, trackedUserJointsRotAxisADouble[0]);
            } else {
                // TODO: display some kind of error message that says data can only be saved in training mode?
            }
            
            break;
        
        case '<':
        case ',':
        case '[':
        case 'p': // previous
           
            saveData();
            
            if (displayState == 'i') break; // do not train data during installation mode
            
            // display previous image in database
            if (label > 0) label--;
//            img_name = imageNames[label];

            break;
        
        case '>':
        case ']':
        case 'n': // next
            
            saveData();
            
            if (displayState == 'i') break; // do not train data during installation mode

            // display next image in database
            if (label < images.size()-1) label++;
//            img_name = imageNames[label];

            break;
        
        case 'r': // random image
            
            saveData();

            if (displayState == 'i') break; // do not train data during installation mode
            
            // display random image from database
            label = getRandomExcluding(0, images.size() - 1, label);
//            img_name = imageNames[label];

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
                
                if (label > images.size())
                {
                    label = ofRandom(0, images.size() - 1);
                    //                        cout << "predicted label is too high —- images.size() = " << ofToString(images.size()) << "; label = " << label << endl;
                    //                        cout << " - displaying a random image instead." << endl;
                }
                
//                img_name = imageNames[label];
            }
            else
            {
                cout << "trainingModelJointsPosRel could not predict" << endl;
            }
            break;
        
        case 'i': // interactive mode
        case 't': // training
        case 'd': // debug
            
            displayStateKeyed = true;
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
            setupKinects();
            // TODO: proper implementation of stopping kinects and starting kinects again. doesn't seem to work properly after kinects have been stopped.
            kinected = true;
            break;
            
        case 'f':
            ofToggleFullscreen();
            windowResized();
            break;
            
        default:
            cout << "Unrecognized key press = " << key << endl;
            // TODO: plan error sound when receiving unrecognized key presses.
            break;
    }
    
    if (displayStateKeyed && key != displayState)
    {
        switch(key){
            case 'i': // interactive mode
                
                // TODO: clean out displayStateString code. nothing seems to reference it.
                displayStateString = "interactive";
                break;
                
            case 't': // training
                
                displayStateString = "training";
                break;
                
            case 'd': // debug
                
                displayStateString = "debug";
                break;
        }

        setDisplayState(key);
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
    // TODO: implement image metadata editing
    //      drag image around Filters:
    //          - no modifiers = position dragging
    //          - sift = rotation
    //          - ctrl = scale
    //          - opt = reference point
    //  - or maybe take a "tool" approach where 'w' = translate, 'e' = rotate, 'r' = scale
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(){
    windowResized(ofGetWidth(), ofGetHeight());
}

void testApp::windowResized(int w, int h){

    if (guiColor){
        // reset guiColor location relative to right of window
        guiColor -> setPosition(w - 220, 0);
    }
}

//--------------------------------------------------------------
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
    
    ofSetLogLevel(OF_LOG_WARNING);
}

void testApp::stopKinects() {
    ofSetLogLevel(OF_LOG_VERBOSE);
    
//    openNIPlayer.removeDepthGenerator();
//    openNIPlayer.removeImageGenerator();
//    openNIPlayer.setRegister(false);
//    openNIPlayer.removeUserGenerator();
    openNIPlayer.stop();
    ofSetLogLevel(OF_LOG_WARNING);
}

void testApp::setupTestUserJoints() { //TODO: make setting test user joints data more data-driven––load an external XML file or atual positions from a GRT joint data file
    vector< ofPoint > tempSingleUserJoints;
    
    // first user pose //
    tempSingleUserJoints.push_back(ofPoint(    3.44307  ,   240.111  ,  138.728    ));
    tempSingleUserJoints.push_back(ofPoint(  -13.2565   ,   429.373  ,  192.71     ));
    tempSingleUserJoints.push_back(ofPoint(  -24.0613   ,   621.484  ,  261.898    ));
    tempSingleUserJoints.push_back(ofPoint(   77.3954   ,   472.38   ,   69.9734   ));
    tempSingleUserJoints.push_back(ofPoint(  297.974    ,   561.992  ,  -55.9768   ));
    tempSingleUserJoints.push_back(ofPoint(  311.435    ,   845.369  ,   38.7029   ));
    tempSingleUserJoints.push_back(ofPoint( -103.908    ,   386.367  ,  315.448    ));
    tempSingleUserJoints.push_back(ofPoint( -197.592    ,   409.68   ,  654.682    ));
    tempSingleUserJoints.push_back(ofPoint( -211.142    ,   682.998  ,  768.707    ));
    tempSingleUserJoints.push_back(ofPoint(   74.8465   ,    76.8003 ,   10.6802   ));
    tempSingleUserJoints.push_back(ofPoint(   15.6446   ,  -316.114  , -112.928    ));
    tempSingleUserJoints.push_back(ofPoint(    7.12648  ,  -683.95   , -240.483    ));
    tempSingleUserJoints.push_back(ofPoint(  -34.5611   ,    24.8962 ,  158.811    ));
    tempSingleUserJoints.push_back(ofPoint(  -96.6418   ,  -371.297  ,   47.5833   ));
    tempSingleUserJoints.push_back(ofPoint( -147.889    ,  -748.943  ,  -32.6917   ));
    testUserJoints.push_back(tempSingleUserJoints);
    tempSingleUserJoints.clear();
    
    // second test user pose //
    tempSingleUserJoints.push_back(ofPoint(-130.336    ,    246.013     ,   101.129     ));
    tempSingleUserJoints.push_back(ofPoint(-155.394    ,    434.483     ,   166.061     ));
    tempSingleUserJoints.push_back(ofPoint(-180.079    ,    625.777     ,   241.777     ));
    tempSingleUserJoints.push_back(ofPoint(-281.048    ,    385.199     ,   260.62      ));
    tempSingleUserJoints.push_back(ofPoint(-519.791    ,    186.235     ,   210.358     ));
    tempSingleUserJoints.push_back(ofPoint(-797.064    ,    161.881     ,    83.4888    ));
    tempSingleUserJoints.push_back(ofPoint( -29.7404   ,    483.767     ,    71.501     ));
    tempSingleUserJoints.push_back(ofPoint( 230.731    ,    417.279     ,  -129.414     ));
    tempSingleUserJoints.push_back(ofPoint( 467.356    ,    500.446     ,  -311.709     ));
    tempSingleUserJoints.push_back(ofPoint(-178.519    ,     28.816     ,    91.3145    ));
    tempSingleUserJoints.push_back(ofPoint( 156.576    ,   -411.119     ,    86.2422    ));
    tempSingleUserJoints.push_back(ofPoint( 517.507    ,   -391.487     ,   -73.8542    ));
    tempSingleUserJoints.push_back(ofPoint( -32.0368   ,     86.2691    ,   -18.9189    ));
    tempSingleUserJoints.push_back(ofPoint( -76.0083   ,   -319.293     ,   -83.0317    ));
    tempSingleUserJoints.push_back(ofPoint( -10.8147   ,   -708.284     ,  -110.032     ));
    testUserJoints.push_back(tempSingleUserJoints);
    tempSingleUserJoints.clear();
    
    // third test user pose //
    tempSingleUserJoints.push_back(ofPoint(     -19.6065   ,   166.533    ,    68.4897     ));
    tempSingleUserJoints.push_back(ofPoint(    -185.775    ,   278.772    ,    31.0986     ));
    tempSingleUserJoints.push_back(ofPoint(    -362.205    ,   390.868    ,    -8.01172    ));
    tempSingleUserJoints.push_back(ofPoint(    -279.693    ,   154.538    ,    75.5608     ));
    tempSingleUserJoints.push_back(ofPoint(    -278.583    ,  -108.422    ,    59.2761     ));
    tempSingleUserJoints.push_back(ofPoint(    -163.701    ,  -100.122    ,  -189.903      ));
    tempSingleUserJoints.push_back(ofPoint(     -91.8562   ,   403.005    ,   -13.3635     ));
    tempSingleUserJoints.push_back(ofPoint(     167.173    ,   480.856    ,    24.5776     ));
    tempSingleUserJoints.push_back(ofPoint(     264.332    ,   254.25     ,   -97.1309     ));
    tempSingleUserJoints.push_back(ofPoint(      90.3639   ,   -20.0428   ,   132.486      ));
    tempSingleUserJoints.push_back(ofPoint(     151.953    ,  -427.526    ,   172.202      ));
    tempSingleUserJoints.push_back(ofPoint(     356.617    ,  -749.295    ,   232.936      ));
    tempSingleUserJoints.push_back(ofPoint(     202.759    ,   128.632    ,    79.2761     ));
    tempSingleUserJoints.push_back(ofPoint(      93.9081   ,  -249.735    ,   -48.7756     ));
    tempSingleUserJoints.push_back(ofPoint(     -11.0111   ,  -615.832    ,  -112.657      ));
    testUserJoints.push_back(tempSingleUserJoints);
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
