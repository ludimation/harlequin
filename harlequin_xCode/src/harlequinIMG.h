//
//  harlequinIMG.h
//  harlequin
//
//  Created by david allen on 4/15/15.
//
//

#pragma once

//#ifndef harlequin_harlequinIMG_h
//#define harlequin_harlequinIMG_h

class harlequinIMG { // can I extend ": public ofImage {" ?
    
public:
    
    // functions
    void                            setup(string path);
    void                            draw();
    ofImage                         getIMG();
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
