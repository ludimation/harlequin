//
//  ofApp.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    ////////////////////
    // image handlers //
    ////////////////////
    imgEdtr = new imgEditor();
    imgEdtr->setup("settings/imgEditorGuiSettings.xml", "images", "imageJointData");
    GRTEdtr = new GRTEditor();
    GRTEdtr->setup("settings/GRTEditorGuiSettings.xml", "images", "imageJointData", "imageTagData", "imageGRTData");
    //    // move to image handler
    //    imageScale = 2.0f; // TODO: scale image dynamically based on artist-specified data per image
    
    
    //////////////////////
    // kinect interface //
    //////////////////////
    kinectInterface = new kinectIO();
    kinectInterface->setup();
    
    //////////////////
    // general init //
    //////////////////
    drawNextFrameMilliseconds = 0;
    // gui settings
    drawFrameRate   = 30;
    drawMirrored    = false;
    imgInvertColors = false; // TODO: move imgInvertColors to imgEditor
    imgColorsAreInverted = false; // TODO: move imgColorsAreInverted to imgEditor
    // debug
    verdana.loadFont(ofToDataPath("verdana.ttf"), 10);
    
    
    //////////////
    // OSC INIT //
    //////////////
    sender.setup(HOST, PORT);
    myHost = HOST;
    myPort = ofToString(PORT);
    
    
    /////////////////////
    // Initialize GUIS //
    /////////////////////
    setupGuis();
    
    
    
    ///////////////////////////
    // initial display state //
    ///////////////////////////
    setDisplayState('d'); // start in installation mode by default (other options are 't' / 'd' for training / debug modes) // this load gui and guiColor settings, so it should appear after those are created
}

//--------------------------------------------------------------
void ofApp::exit(){
    
    // kinects
    kinectInterface->exit();
    delete kinectInterface;
    
    // clean up image editor
    imgEdtr->exit();
    delete imgEdtr;
    
    // gui
    delete gui;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //    if (trainModelsNow) trainModelsNow = GRTEdtr->trainModel();
    
    kinectInterface->update();
    
    imgEdtr->update(kinectInterface->getUserJoints());
}

//--------------------------------------------------------------
void ofApp::setupGuis() {
    /////////////////////
    // Initialize GUIS //
    /////////////////////
    //
    //------------
    // Main GUI --
    //////////////
    gui = new ofxUISuperCanvas("harlequin");
    ofAddListener(gui -> newGUIEvent, this, &ofApp::guiEvent);
    gui -> addSpacer();
    gui -> addTextArea("text", "'h' to hide this panel", OFX_UI_FONT_SMALL);
    //    gui -> addLabel("text", "'h' to hide this panel");
    gui -> addSpacer();
    //
    // Switch display modes
    gui -> addLabel("application mode: "); // TODO: Make this more like the editors where there's a "displayModeEditor", "levelEditor", or "roomEditor" that handles larger settings files which can be changed between
    vector<string> appModes; appModes.push_back("i"); appModes.push_back("d"); appModes.push_back("t");
    ofxUIRadio *radioAppMode = gui -> addRadio("application mode", appModes, OFX_UI_ORIENTATION_HORIZONTAL);
    vector< ofxUIToggle*> toggles = radioAppMode -> getToggles();
    toggles[0]->bindToKey('i');
    toggles[0]->bindToKey('I');
    toggles[1]->bindToKey('d');
    toggles[1]->bindToKey('D');
    toggles[2]->bindToKey('t');
    toggles[2]->bindToKey('T');
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
    gui -> addTextArea("text", "'k' to kinect, 'x' to diskinnect");
    ofxUIToggle* toggleKinected = gui -> addToggle("kinected", &kinected);
    toggleKinected->bindToKey('k');
    toggleKinected->bindToKey('K');
    toggleKinected->bindToKey('x');
    toggleKinected->bindToKey('X');
    gui -> addSpacer();
    gui -> addTextArea("text", "'m' to mirror kinect input");
    ofxUIToggle* mirrorImageToggle = gui -> addToggle("mirror image", &drawMirrored);
    mirrorImageToggle -> bindToKey('m');
    mirrorImageToggle -> bindToKey('M');
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
    gui -> addTextInput("host", "address") -> setAutoClear(false);
    gui -> addTextInput("port", "port") -> setAutoClear(false);
    gui -> addToggle("send OSC", &sendOSC);
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
    ofAddListener(guiColor -> newGUIEvent, this, &ofApp::guiEvent);
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
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e) {
    bool noCallbackForWidget = false;
    string nameStr = e.widget->getName();
    int kind = e.widget->getKind();
    /*  */ if(nameStr == "invert image") {
        // TODO: debug "invert images" only when toggle is updated.
        //        if (imgColorsAreInverted != imgInvertColors) {
        //            for (int imgI=0; imgI < images.size(); ++imgI){
        //                ofImage* imgTMP = &images[imgI];
        //                invertImage(imgTMP);
        //            }
        //            imgColorsAreInverted = imgInvertColors;
        //        }
    } else if (nameStr == "mirror image") {
        kinectInterface->setMirror(drawMirrored);
        
    } else if (nameStr == "kinected") {
        if (kinected && !kinectsInitialized) // || ofGetKeyPressed('k')?
        {
            kinectsInitialized = kinectInterface->setupKinects(drawMirrored);
            kinected = kinectsInitialized;
        }
        else if (!kinected)
        {
            kinected = !kinectInterface->stopKinects();
//            delete kinectInterface; kinectInterface = NULL;
//            kinectInterface = new kinectIO();
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
        
        
    } else if (nameStr == "application mode" ||nameStr=="i"||nameStr=="d"||nameStr=="t") {
        ofxUIRadio *radioAppMode;
        if (kind == OFX_UI_WIDGET_RADIO) radioAppMode = (ofxUIRadio *) e.widget;
        else radioAppMode = (ofxUIRadio *) e.widget -> getParent();
        switch (radioAppMode -> getValue()) {
            case 0: // interactive
                if (displayState != 'i') {
                    displayStateString = "interactive";
                    setDisplayState('i');
                }
                break;
                
            case 1: // debug
                if (displayState != 'd') {
                    displayStateString = "debug";

                    setDisplayState('d');
                }
                break;
                
            case 2: // training
                if (displayState != 't') {
                    displayStateString = "training";
                    setDisplayState('t');
                }
                break;
                
            default:
                break;
        }
        

    } else if (nameStr == "save main settings") {
        gui -> saveSettings("guiSettings_" + ofToString(displayState) + ".xml");
    } else if (nameStr == "save color settings") {
        guiColor -> saveSettings("guiSettings_" + ofToString(displayState) + "_color.xml");
        
        
    } else if (nameStr == "image blend mode"||nameStr == "i0"||nameStr == "iA"||nameStr == "i+"||nameStr == "i-"||nameStr == "i*"||nameStr == "iS") {
        ofxUIRadio *radio;
        if (nameStr == "image blend mode") {
            radio = (ofxUIRadio *) e.widget;
        } else {
            radio = (ofxUIRadio *) e.widget->getParent();
        }
        imgBlendMode = radio -> getValue();
    } else if (nameStr == "depth color mode"||nameStr == "PSYCHEDELIC_SHADES"||nameStr == "PSYCHEDELIC"||nameStr == "RAINBOW"||nameStr == "CYCLIC_RAINBOW"||nameStr == "BLUES"||nameStr == "BLUES_INV"||nameStr == "GREY"||nameStr == "STATUS") {
        ofxUIRadio *radio;
        if (nameStr == "depth color mode") {
            radio = (ofxUIRadio *) e.widget;
        } else {
            radio = (ofxUIRadio *) e.widget->getParent();
        }
        depthColorMode = radio -> getValue();
        kinectInterface->setDepthColoring((DepthColoring)depthColorMode);
    } else if (nameStr == "depth blend mode"||nameStr == "d0"||nameStr == "dA"||nameStr == "d+"||nameStr == "d-"||nameStr == "d*"||nameStr == "dS") {
        ofxUIRadio *radio;
        if (nameStr == "depth blend mode") {
            radio = (ofxUIRadio *) e.widget;
        } else {
            radio = (ofxUIRadio *) e.widget->getParent();
        }
        depthBlendMode = radio -> getValue();
    } else if (nameStr == "skleton blend mode"||nameStr == "s0"||nameStr == "sA"||nameStr == "s+"||nameStr == "s-"||nameStr == "s*"||nameStr == "sS") {
        ofxUIRadio *radio;
        if (nameStr == "depth blend mode") {
            radio = (ofxUIRadio *) e.widget;
        } else {
            radio = (ofxUIRadio *) e.widget->getParent();
        }
        skelBlendMode = radio -> getValue();
        
        
    } else {
        // default
        noCallbackForWidget = true;
    }
    
    // debug
    if(ofGetLogLevel() == OF_LOG_VERBOSE){
        if (noCallbackForWidget) {
            cout << "[verbose] ofApp::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
        } else {
            cout << "[verbose] ofApp::guiEvent(ofxUIEventArgs &e) -- gui element name = " << nameStr << endl;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofBackground(bgRed, bgGreen, bgBlue);
    
    string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
    ofPoint jointsCenterProjective;
    ofPoint imgRefPoint;
    ofPoint imgPTRRefPoint;
    ofPoint screenCenter = ofVec3f(ofGetWidth()/2.0f, ofGetHeight()/2.0f, 1.0f);
    vector< vector< double > > trackedUserJointsPosABSDouble = kinectInterface -> getUserJointsDoubles();
    vector< vector< double > > trackedUserJointsPosRelDouble = kinectInterface -> getUserJointsDoubles();
    vector< ofPoint > trackedUserCentersProjective = kinectInterface -> getTrackedUserCentersProjective();

    
    // idle until it's time to draw the next frame
    while (ofGetElapsedTimeMillis() < drawNextFrameMilliseconds) {
        // TODO: put streaming events here
        //  - should this be moved to the end of the "update()" function?
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
        kinectInterface->drawDepth(0.0f, 0.0f, float( ofGetWidth() ), float( ofGetHeight() ));
        
        ofPopStyle();
    }
    
    // manage image drawing call style and drawing matrix
    ofPushStyle();
    ofPushMatrix();
    
    
    // draw images
    switch (displayState) {
        case 'i':
        case 'd': // debug
            
            // set colors
            ofSetColor(imgRed, imgGreen, imgBlue, imgAlpha);
            ofEnableBlendMode((ofBlendMode)imgBlendMode);
            
            // TODO: change data type for GUI variables like imBlendMode et al, and cast data as proper format when changed by the GUI
            
            for (int j = 0; j < trackedUserJointsPosABSDouble.size(); ++j) {
                
//                // select label: Relative Position model
//                if (trainingModelJointsPosRel.predict(trackedUserJointsPosRelDouble[j]) && images.size())
//                {
//                    label = trainingModelJointsPosRel.getPredictedClassLabel();
//                    //                    cout << "predicted label:" << label << endl;
//                    
//                    if (label > images.size()) // if predicted label image hasn't been loaded, display a random image
//                    {
//                        label = ofRandom(0, images.size() - 1);
//                        //                        cout << "predicted label is too high —- images.size() = " << ofToString(images.size()) << "; label = " << label << endl;
//                        //                        cout << " - displaying a random image instead." << endl;
//                    }
//                    
//                    //                    img_name = imageNames[label];
//                    //                    cout << "img_name = " << img_name << endl;
//                }
//                else
//                {
//                    //                    img_name = "";
//                    cout << "trainingModelJointsPosRel could not predict" << endl;
//                }
                
                label = ofRandom(0, images.size() - 1);
                
                // TODO: implement SSD option selection GUI & implemnet loading images directly from HD
                //if (img->loadImage(img_name)) { cout << "img loaded" << endl; } else { cout << "img not loaded" << endl; //find another image if image could not be loaded}
                
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
                    ofImage img;
                    img = images[label];
                    ofImage* imgPTR = new ofImage();
                    imgPTR = imagesPTRs[label];
                    
                    // calculate reference points for drawing
                    if(jointsCenterProjective.z != 0) { // scale
                        imgRefPoint.z = 1500.0f / float( jointsCenterProjective.z);
                    } else {
                        imgRefPoint.z = 1.0f;
                    }
                    float xOffset = float( img.width  ) * imgRefPoint.z * imageScale / 2.0f;
                    float yOffset = float( img.height  ) * imgRefPoint.z * imageScale / 2.0f;
                    float xOffsetPTR = float( imgPTR->width *2 ) * imgRefPoint.z * imageScale / 2.0f;
                    float yOffsetPTR = float( imgPTR->height *2 ) * imgRefPoint.z * imageScale / 2.0f;
                    imgRefPoint.x = jointsCenterProjective.x - xOffset; // left side
                    imgRefPoint.y = jointsCenterProjective.y - yOffset; // top side
                    imgPTRRefPoint.x = jointsCenterProjective.x - xOffsetPTR;
                    imgPTRRefPoint.y = jointsCenterProjective.y - yOffsetPTR;
                    
                    img.mirror(0, drawMirrored);
                    imgPTR->mirror(0, drawMirrored);
                    if (imgInvertColors) {
                        invertImage(img);
                        invertImage(imgPTR);
                    }
                    
                    // TODO: image drawing should be z-sorted so further images draw behind closer ones
                    // draw image at position and scale relative to center of screen and image
                    // ofSetDepthTest()?
                    //                    img.draw(imgRefPoint.x,
                    //                             imgRefPoint.y,
                    //                             img.width * imgRefPoint.z * imageScale,
                    //                             img.height * imgRefPoint.z * imageScale);
                    //                    imgPTR->update();
                    
                    // image pointer drawing tests
                    //                    bikersPTR->draw(0, 100);
                    //                    bikersPTR->draw(100, 100, 50, 50);
                    
                    imgPTR->draw(imgPTRRefPoint.x,
                                 imgPTRRefPoint.y,
                                 imgPTR->width * imgPTRRefPoint.z * imageScale,
                                 imgPTR->height * imgPTRRefPoint.z * imageScale);
                    
                    // Build debug message string
                    msg = msg + "\n jointsCenterProjective = " + ofToString(jointsCenterProjective);
                    msg = msg + "\n xOffset = " + ofToString(xOffset);
                    msg = msg + "\n yOffset = " + ofToString(yOffset);
                    msg = msg + "\n imgRef = " + ofToString(imgRefPoint);
                    msg = msg + "\n sceenCenter = " + ofToString(screenCenter);
                }
            }
            
            break;
            
        case 't': // training // move these to an "if (flag) statement" so that flags can be set in GUI, and get wrid of this complicated switch statement
        default:
            
            // set colors
            ofSetColor(imgRed, imgGreen, imgBlue, imgAlpha);
            ofEnableBlendMode((ofBlendMode)imgBlendMode);
            
            // draw image editor image
            imgEdtr->draw(drawMirrored);
            
            break;
    }
    // reset drawing matrix and style
    ofPopMatrix();
    ofPopStyle();
    
    if (drawSkeletons or drawDepth){
        ofPushStyle();
        //  openNIRecorder.drawDebug(0, 0);
        if (drawDepth and !drawDepthBehind)
        {
            ofEnableBlendMode((ofBlendMode)depthBlendMode);
            ofSetColor(depthRed, depthGreen, depthBlue, depthAlpha);
            kinectInterface->drawDepth(0, 0, ofGetWidth(), ofGetHeight());
        }
        if (drawSkeletons)
        {
            ofSetColor(skelRed, skelGreen, skelBlue, skelAlpha);
            ofEnableBlendMode((ofBlendMode)skelBlendMode);
            kinectInterface->drawSkeletons(0, 0, ofGetWidth(), ofGetHeight());
        }
        
        ofPopStyle();
    }
    
    if (drawJoints2MSG) {
        // add bone data for tracked user to display message
        vector< vector< ofPoint > > trackedUserJoints = kinectInterface->getUserJoints();
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
        vector< ofPoint > trackedUsrCntrsPrjctv = kinectInterface->getTrackedUserCentersProjective();
        if (trackedUsrCntrsPrjctv.size() > 0) {
            for (int i=0; i < trackedUsrCntrsPrjctv.size(); ++i) {
                ofxOscMessage m;
                m.setAddress("/POS" + ofToString(i) );
                m.addFloatArg( trackedUsrCntrsPrjctv[i].x );
                m.addFloatArg( trackedUsrCntrsPrjctv[i].y );
                m.addFloatArg( trackedUsrCntrsPrjctv[i].z );
                sender.sendMessage(m);
            }
        }
        
        
    }
    
    
    ofPopStyle();
}


//--------------------------------------------------------------
void ofApp::invertImage(ofImage* imgPTRlocal) {
    ofPixels imgPX = imgPTRlocal->getPixelsRef();
    ofTexture imgTEX = imgPTRlocal->getTextureReference();
    invertImage(imgPX, imgTEX);
}

void ofApp::invertImage(ofImage &imgREF) {
    // TODO: optimize image inversions. seems to slow down FPS to ~ 22 from ~55 on my ancient MacBook Pro
    ofPixels imgPX = imgREF.getPixelsRef();
    ofTexture imgTEX = imgREF.getTextureReference();
    invertImage(imgPX, imgTEX);
}

void ofApp::invertImage(ofPixels &imgPX, ofTexture &imgTEX) {
    int iPX = 0;
    while ( iPX < imgPX.size() ) {
        imgPX[iPX] = 512.0f - imgPX[iPX];
        iPX++;
    }
    imgTEX.loadData(imgPX);
}


//--------------------------------------------------------------
void ofApp::setDisplayState(char newState) {
    bool undefinedState = false;
    
    imgEdtr->disable();
    GRTEdtr->disable();
    //            // save & train model before switching modes?
    //            GRTEdtr->saveData();
    //            GRTEdtr->saveModel();
    
    switch (newState) {
        case 't': // training
            
            imgEdtr->enable();
            GRTEdtr->enable();
            
            break;
            
        case 'd': // debug // male
            
            break;
            
        case 'i': // interactive // female
            
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
void ofApp::keyPressed(int key){
    
        // initialize flags
        bool displayStateKeyed;
    
        // process key pressed
        switch (key) {
    
            case 'h':
                gui->toggleVisible();
                guiColor->toggleVisible();
                GRTEdtr;
                break;
    //
    //        case 'b': // NOTE: updated to 'b' for BUILD DATA
    //            if (displayState == 'i') break; // do not train data during installation mode
    //
    //            // Store data to associate with currently displayed image
    //            //  - joint positions (0–15) — 0 = center, 1–15 = joints
    //
    //            if ((displayState == 'd' || displayState == 't') && trackedUserJointsPosABS.size()){
    //                trainingDataJointsPosABS.addSample(label, trackedUserJointsPosABSDouble[0]);
    //                trainingDataJointsPosRel.addSample(label, trackedUserJointsPosRelDouble[0]);
    //                trainingDataJointsRotAxisA.addSample(label, trackedUserJointsRotAxisADouble[0]);
    //            } else {
    //                // TODO: display some kind of error message that says data can only be saved in training mode?
    //            }
    //
    //            break;
    //
    //
    //        case 's': // NOTE: Moved save functionality here to minimize lagging during data building phase
    //            if (displayState == 'i') break; // do not train data during installation mode
    //
    //            GRTEdtr->saveData();
    //            GRTEdtr->saveModel();
    //
    //            break;
    //
    //        case '=': // increase drawFrameRate
    //            // fall through (intentional)
    //        case '+': // increase drawFrameRate
    //
    //            if (drawFrameRate < 90) drawFrameRate = drawFrameRate + 5;
    //            break;
    //
    //        case '_': // decrease drawFrameRate
    //            // fall through (intentional)
    //        case '-': // decrease drawFrameRate
    //            
    //            if (drawFrameRate > 5) drawFrameRate = drawFrameRate - 5;
    //            break;
    //
    //        case 'k':
    //            break;
    //            
    //        case 'f':
    //            ofToggleFullscreen();
    //            windowResized();
    //            break;
                
            default:
                cout << "Unrecognized key press = " << key << endl;
                // TODO: plan error sound when receiving unrecognized key presses.
                break;
        }
        
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}


//--------------------------------------------------------------
void ofApp::windowResized(){
    windowResized(ofGetWidth(), ofGetHeight());
}

void ofApp::windowResized(int w, int h){
    if (guiColor){
        // reset guiColor location relative to right of window
        guiColor -> setPosition(w - 220, 0);
    }
}