#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "GRT.h"

class testApp : public ofBaseApp{

public:
    
	void setup();
	void update();
	void draw();
    void exit();
    
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

    //	ofxOpenNI               openNIRecorder;
    ofxOpenNI               openNIPlayer;
    ofxOpenNIUser           nTrackedUser;
    vector<ofPoint>         trackedUserJoints;
    //    vector<ofxOpenNIJoint>  trackedUserJoints;
    int                     nTrackedUsers;
    char                    displayState;
    vector <ofImage>        images;
    ofFile                  file;
    string                  testJointData;
    string                  testFileName;
    ofBuffer                testFileBuff;
    
    ofTrueTypeFont verdana;
    
    void userEvent(ofxOpenNIUserEvent & event);
    void gestureEvent(ofxOpenNIGestureEvent & event);

private:
    GRT::SVM svm;
    
};

#endif
