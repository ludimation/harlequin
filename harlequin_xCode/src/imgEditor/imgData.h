//
//  imgData.h
//  harlequin
//
//  Created by david allen on 4/15/15.
//
//

#pragma once

//#ifndef harlequin_imgData_h
//#define harlequin_imgData_h

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "MSAjoint.h"

class imgData { // can I extend ": public ofImage {//" ?
    
public:
    
    // functions
    bool open(map <string, vector<string> >::iterator it);
    void draw(); // draw image
    bool save();
    
    void pushTrnData(vector< MSAjoint* > tJoints);
    void setJointsFromJointSet(int i);
    void showTrnData(int i);
    void hideTrnData(int i);
    void eraseTrnData(int i);
    // properties
    
private:
    // internal data
    string                          myBaseName, mySavePath;
    vector< ofImage* >              myImgs;
    map< string, float >            myImgsPathScaleMap; // map of paths and their perspective scales relative to the joints skeleton
    vector< MSAjoint* >             myJoints;
    vector< vector< MSAjoint* > >   myTrnJointSets;
    
};

//#endif
