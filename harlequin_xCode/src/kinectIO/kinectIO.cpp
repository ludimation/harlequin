//
//  kinectIO.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "kinectIO.h"

//--------------------------------------------------------------
void kinectIO::setup() {
    initialized = false;
}

//--------------------------------------------------------------
void kinectIO::exit() {
    if (initialized) stopKinects();
    delete openNIdevice;
}

//--------------------------------------------------------------
void kinectIO::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void kinectIO::gestureEvent(ofxOpenNIGestureEvent & event){
    ofLogNotice() << event.gestureName << getGestureStatusAsString(event.gestureStatus) << "from device" << event.deviceID << "at" << event.timestampMillis;
}


//--------------------------------------------------------------
bool kinectIO::setupKinects(bool drawMirrored) {
    ofSetLogLevel(OF_LOG_VERBOSE);

    
    openNIdevice->setup();
    openNIdevice->addDepthGenerator();
    openNIdevice->addImageGenerator();
    openNIdevice->setRegister(true);
    openNIdevice->setMirror(drawMirrored);
    openNIdevice->addUserGenerator();
    openNIdevice->setMaxNumUsers(4); // was 2
    openNIdevice->setDepthColoring(COLORING_BLUES_INV);
    openNIdevice->start();
    
    ofSetLogLevel(OF_LOG_WARNING);
    
    initialized = true;
    
    return true;
}

bool kinectIO::stopKinects() {
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    //    openNIPlayer.removeDepthGenerator();
    //    openNIPlayer.removeImageGenerator();
    //    openNIPlayer.setRegister(false);
    //    openNIPlayer.removeUserGenerator();
    openNIdevice->stop();
    ofSetLogLevel(OF_LOG_WARNING);
    
    return true;
}

vector< vector<ofPoint> > kinectIO::setupTestUserJoints() { //TODO: make setting test user joints data more data-driven––load an external XML file or atual positions from a GRT joint data file
    vector< ofPoint > tempSingleUserJoints;
    vector< vector <ofPoint> > testUserJoints;
    
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
    
    return testUserJoints;
}
