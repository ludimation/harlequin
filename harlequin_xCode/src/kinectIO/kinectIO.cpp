//
//  kinectIO.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "kinectIO.h"

//--------------------------------------------------------------
kinectIO::kinectIO () {
    
}

//--------------------------------------------------------------
kinectIO::~kinectIO () {
    delete openNIdevice; openNIdevice = NULL;
}

//--------------------------------------------------------------
void kinectIO::setup() {
    initialized = false;
    
    openNIdevice = new ofxOpenNI();
    testUserJoints = setupTestUserJoints();
    testUserJointsDouble = convertUserJointToDouble(testUserJoints);
}

//--------------------------------------------------------------
void kinectIO::exit() {
    if (initialized) stopKinects();
    delete openNIdevice;
}

void kinectIO::update() {
    if (!initialized) return;
    
    openNIdevice->update();
    
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
    if (initialized) {
        numUsers = openNIdevice->getNumTrackedUsers();
        if (numUsers) numJoints = openNIdevice->getTrackedUser(0).joints.size();
    } else {
        numUsers = 3;
        numJoints = 15;
    }
    
    // build joint position vectors
    for (int j = 0; j < numUsers; j++) {
        singleUserJointsPosABS.clear();
        singleUserJointsPosRel.clear();
        singleUserJointsRotAxisA.clear();
        
        if (initialized) {
            // store center positions in both world space and projective space
            ofPoint userJCenter = openNIdevice->getTrackedUser(j).getCenter();
            ofPoint userJcenterProjective = openNIdevice->worldToProjective(userJCenter);
            //            ofPoint userJcenterProjective = openNIdevice->getTrackedUser(j).joints[0].getProjectivePosition(); // TODO: use the root position of the hips for relative joint data instead?
            trackedUserCentersProjective.push_back(
                                                   userJcenterProjective *
                                                   ofPoint(
                                                           float( ofGetWidth() ) / 640.0f,
                                                           float( ofGetHeight()  ) / 480.0f,
                                                           1
                                                           )
                                                   );
            for (int i = 0; i < numJoints; ++i) {
                ofPoint jointIworldPos = openNIdevice->getTrackedUser(j).joints[i].getWorldPosition();
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
void kinectIO::drawDepth(int x_, int y_, int w_, int h_) {
    if (!initialized) return;
    openNIdevice->drawDepth(x_, y_, w_, h_);
}

//--------------------------------------------------------------
void kinectIO::drawSkeletons(int x_, int y_, int w_, int h_) {
    if (!initialized) return;
    openNIdevice->drawSkeletons(x_, y_, w_, h_);
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
void kinectIO::setMirror(bool drawMirrored_) {
    if (!initialized) return;
    openNIdevice->setMirror(drawMirrored_);
}

//--------------------------------------------------------------
void kinectIO::setDepthColoring(DepthColoring depthColorMode_) {
    if (!initialized) return;
    openNIdevice->setDepthColoring(depthColorMode_);
}

//--------------------------------------------------------------
bool kinectIO::setupKinects(bool drawMirrored_) {
    ofSetLogLevel(OF_LOG_VERBOSE);

    
    openNIdevice->setup();
    openNIdevice->addDepthGenerator();
//    openNIdevice->addImageGenerator();
    openNIdevice->setRegister(true);
    openNIdevice->setMirror(drawMirrored_);
    openNIdevice->addUserGenerator();
    openNIdevice->setMaxNumUsers(4); // was 2
    openNIdevice->setDepthColoring(COLORING_BLUES_INV);
    openNIdevice->start();
    
    ofSetLogLevel(OF_LOG_WARNING);
    
    initialized = true;
    
    return true;
}

//--------------------------------------------------------------
bool kinectIO::stopKinects() {
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    openNIdevice->removeDepthGenerator();
//    openNIdevice->removeImageGenerator();
    openNIdevice->setRegister(false);
    openNIdevice->removeUserGenerator();
    openNIdevice->stop();
    ofSetLogLevel(OF_LOG_WARNING);
    
    initialized = false;
    
    return true;
}

//--------------------------------------------------------------
vector< vector< ofPoint > > kinectIO::getUserJoints() {
    if (initialized) {
        return trackedUserJointsPosRel;
    } else return testUserJoints;
}

//--------------------------------------------------------------
vector< vector< double > >  kinectIO::getUserJointsDoubles() {
    if (initialized) {
        return trackedUserJointsPosRelDouble;
    } else return testUserJointsDouble;
}

//--------------------------------------------------------------
vector< ofPoint > kinectIO::getTrackedUserCentersProjective() {
    return trackedUserCentersProjective;
}


//--------------------------------------------------------------
vector< vector<ofPoint> > kinectIO::setupTestUserJoints() { //TODO: make setting test user joints data more data-driven––load an external XML file or atual positions from a GRT joint data file
    vector< ofPoint > joints;
    vector< vector <ofPoint> > jointSets;
    
    // first user pose //
    joints.push_back(ofPoint(    3.44307  ,   240.111  ,  138.728    ));
    joints.push_back(ofPoint(  -13.2565   ,   429.373  ,  192.71     ));
    joints.push_back(ofPoint(  -24.0613   ,   621.484  ,  261.898    ));
    joints.push_back(ofPoint(   77.3954   ,   472.38   ,   69.9734   ));
    joints.push_back(ofPoint(  297.974    ,   561.992  ,  -55.9768   ));
    joints.push_back(ofPoint(  311.435    ,   845.369  ,   38.7029   ));
    joints.push_back(ofPoint( -103.908    ,   386.367  ,  315.448    ));
    joints.push_back(ofPoint( -197.592    ,   409.68   ,  654.682    ));
    joints.push_back(ofPoint( -211.142    ,   682.998  ,  768.707    ));
    joints.push_back(ofPoint(   74.8465   ,    76.8003 ,   10.6802   ));
    joints.push_back(ofPoint(   15.6446   ,  -316.114  , -112.928    ));
    joints.push_back(ofPoint(    7.12648  ,  -683.95   , -240.483    ));
    joints.push_back(ofPoint(  -34.5611   ,    24.8962 ,  158.811    ));
    joints.push_back(ofPoint(  -96.6418   ,  -371.297  ,   47.5833   ));
    joints.push_back(ofPoint( -147.889    ,  -748.943  ,  -32.6917   ));
    jointSets.push_back(joints);
    joints.clear();
    
    // second test user pose //
    joints.push_back(ofPoint(-130.336    ,    246.013     ,   101.129     ));
    joints.push_back(ofPoint(-155.394    ,    434.483     ,   166.061     ));
    joints.push_back(ofPoint(-180.079    ,    625.777     ,   241.777     ));
    joints.push_back(ofPoint(-281.048    ,    385.199     ,   260.62      ));
    joints.push_back(ofPoint(-519.791    ,    186.235     ,   210.358     ));
    joints.push_back(ofPoint(-797.064    ,    161.881     ,    83.4888    ));
    joints.push_back(ofPoint( -29.7404   ,    483.767     ,    71.501     ));
    joints.push_back(ofPoint( 230.731    ,    417.279     ,  -129.414     ));
    joints.push_back(ofPoint( 467.356    ,    500.446     ,  -311.709     ));
    joints.push_back(ofPoint(-178.519    ,     28.816     ,    91.3145    ));
    joints.push_back(ofPoint( 156.576    ,   -411.119     ,    86.2422    ));
    joints.push_back(ofPoint( 517.507    ,   -391.487     ,   -73.8542    ));
    joints.push_back(ofPoint( -32.0368   ,     86.2691    ,   -18.9189    ));
    joints.push_back(ofPoint( -76.0083   ,   -319.293     ,   -83.0317    ));
    joints.push_back(ofPoint( -10.8147   ,   -708.284     ,  -110.032     ));
    jointSets.push_back(joints);
    joints.clear();
    
    // third test user pose //
    joints.push_back(ofPoint(     -19.6065   ,   166.533    ,    68.4897     ));
    joints.push_back(ofPoint(    -185.775    ,   278.772    ,    31.0986     ));
    joints.push_back(ofPoint(    -362.205    ,   390.868    ,    -8.01172    ));
    joints.push_back(ofPoint(    -279.693    ,   154.538    ,    75.5608     ));
    joints.push_back(ofPoint(    -278.583    ,  -108.422    ,    59.2761     ));
    joints.push_back(ofPoint(    -163.701    ,  -100.122    ,  -189.903      ));
    joints.push_back(ofPoint(     -91.8562   ,   403.005    ,   -13.3635     ));
    joints.push_back(ofPoint(     167.173    ,   480.856    ,    24.5776     ));
    joints.push_back(ofPoint(     264.332    ,   254.25     ,   -97.1309     ));
    joints.push_back(ofPoint(      90.3639   ,   -20.0428   ,   132.486      ));
    joints.push_back(ofPoint(     151.953    ,  -427.526    ,   172.202      ));
    joints.push_back(ofPoint(     356.617    ,  -749.295    ,   232.936      ));
    joints.push_back(ofPoint(     202.759    ,   128.632    ,    79.2761     ));
    joints.push_back(ofPoint(      93.9081   ,  -249.735    ,   -48.7756     ));
    joints.push_back(ofPoint(     -11.0111   ,  -615.832    ,  -112.657      ));
    jointSets.push_back(joints);
    
    return jointSets;
}

//--------------------------------------------------------------
vector< vector<double> > kinectIO::convertUserJointToDouble(vector< vector < ofPoint > > ofPointJoints_) {
    vector< vector<double> > doubleJointsSets;
    for(int set=0; set<ofPointJoints_.size(); ++set ) {
        vector<double> doubleJoints;
        for(int jnt=0; jnt<ofPointJoints_[set].size(); ++jnt) {
            doubleJoints.push_back(ofPointJoints_[set][jnt].x);
            doubleJoints.push_back(ofPointJoints_[set][jnt].y);
            doubleJoints.push_back(ofPointJoints_[set][jnt].z);
        }
        doubleJointsSets.push_back(doubleJoints);
    }
    
    return doubleJointsSets;
}
