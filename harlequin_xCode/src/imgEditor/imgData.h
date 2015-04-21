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

class imgData { // can I extend ": public ofImage {//" ?
    
public:
    
    // functions
    bool                            open(string path);
    bool                            save();
    void                            draw(); // draw image
    ofImage*                        getOFImagePTR();
    
    void                            pushTrainingData(vector< ofPoint > tJoints, ofImage tImg);
    void                            eraseTrainingData(int ti);
    ofImage                         getTrainingIMG(int ti);
    vector< ofPoint >               getTrainingJointSet(int ti);
    // properties
    ofColor                         drawColor;
    ofBlendMode                     drawMode;
    
private:
    // internal data
    string                          path;
    ofImage                         IMG;
    vector< ofImage >               trainingIMGS;
    vector< vector< ofPoint > >     trainingJointSets;
    
};

//#endif
