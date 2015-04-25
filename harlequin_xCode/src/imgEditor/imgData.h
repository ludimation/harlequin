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
    
    // properties
    string                          myBaseName, mySavePath;
    vector< ofImage* >              myImgs;
    map< string, float >            myImgsPathScaleMap; // map of paths and their perspective scales relative to the joints skeleton
    int                             myJointsCount;
    vector< MSAjoint* >             myJoints;
    vector< vector< MSAjoint* > >   myTrnJointSets;
    bool                            myJointsEdited; // flag to check to see if the joints have been edited yet
    int                             myJointSize, myTrainingDataJointsSize;

    // obj creator
    imgData() {
        // declare variables
        myBaseName, mySavePath = "";
        myJointsCount = 15;
        myJointsEdited = false;
        myJointSize = 10;
        myTrainingDataJointsSize = 5;
        
        // populate gui joints vector
        for(int jnt = 0; jnt < myJointsCount; ++jnt) {
            MSAjoint *obj = new MSAjoint();
            obj->set(830 + (jnt*myJointSize + 5), myJointSize*3, myJointSize, myJointSize);
            // set user-specified joint color
            obj->setColors(0xFFFF00, 0x00FF00, 0xFF00FF);
            obj->enableMouseEvents();
            obj->setDraggable(true);
            myJoints.push_back(obj);
        }
    };
    // obj destructor
    ~imgData() {
        
        // clean up images
        for (int i = 0; i < myImgs.size(); ++i) {
            delete myImgs[i];
            myImgs[i] = NULL;
        }
        vector<ofImage*>().swap(myImgs);

        // clean up user-specified joints
        for (int jnt = 0; jnt < myJointsCount; ++jnt) {
            // user-set joints
            delete myJoints[jnt]; myJoints[jnt] = NULL;
        }
        vector<MSAjoint*>().swap(myJoints);
        
        // clean up training joint sets
        for (int set = 0; set < myTrnJointSets.size(); ++set) {
            eraseTrnData(set);
        }
        vector<vector<MSAjoint*> >().swap(myTrnJointSets);
    };
    
    // core functions
    bool open(map <string, vector<string> >::iterator it_, string imageJointDataDirectory_) {
        // try to load the XML file
        // if it exists, set imgData properties to match
        // otherwise, use default data (should already be set up?
    };
    
    void draw() {
       // bring in draw image functionality from imgEditor
    };
    
    bool save() {
        // create XML object
        // populate it with imgData property values
        // save it to the save directory with myBaseName + ".xml"
    };
    
    // training data manipulation functions
    void pushTrnData(vector< MSAjoint* > tJoints_) {
        // if the number of pushed joints matches the expected number of joints
        if (tJoints_.size() != myJointsCount) {
            cout << "imgData::pushTrnData(vector<MSAjoint *> tJoints_) -- tJoints_.size() != myJointsCount " << endl;
            cout << " -- tJoints_.size() = " << ofToString(tJoints_.size()) << ", myJointsCount = " << myJointsCount << endl;
            return;
        }
        
        // create a vector of MSAjoint objects for each joint
        vector<MSAjoint*> tJoints;
        for (int jnt = 0; jnt < myJointsCount; ++jnt) {
            MSAjoint *obj = new MSAjoint();
            obj->setPosition3D(tJoints_[jnt]->x, tJoints_[jnt]->y, tJoints_[jnt]->z);
            obj->setSize(myTrainingDataJointsSize, myTrainingDataJointsSize);
            tJoints.push_back(obj);
        }
        // push the vector to the vector of training joint sets
        myTrnJointSets.push_back(tJoints);
    };
    
    void setTrnDataVisibilty(int i_, bool visible_) {
        // if specified training set exists
        if (myTrnJointSets.size() < i_) {
            cout << "imgData::showTrnData(int i_) -- there is no training joint set i_ = " << ofToString(i_) << endl;
            cout << " -- myTrnJointSets.size() = " << ofToString(myTrnJointSets.size()) << endl;
            return;
        }
        
        for (int jnt = 0; jnt < myJointsCount; ++jnt) {
            if (visible_) {
                myTrnJointSets[i_][jnt]->enableAppEvents();
            } else {
                myTrnJointSets[i_][jnt]->disableAppEvents();
            }
        }
    };
    
    void eraseTrnData(int i_) {
        // if specified training set exists
        if (myTrnJointSets.size() < i_) {
            cout << "imgData::eraseTrnData(int i_) -- there is no training joint set i_ = " << ofToString(i_) << endl;
            cout << " -- myTrnJointSets.size() = " << ofToString(myTrnJointSets.size()) << endl;
            return;
        }
        
        for (int jnt = 0; jnt < myJointsCount; ++jnt) {
            delete myTrnJointSets[i_][jnt]; myTrnJointSets[i_][jnt] = NULL;
        }
        
    };
    
    void setJointsFromTrainingJointSet(int i_) {
        // if specified training set exists
        if (myTrnJointSets.size() < i_) {
            cout << "imgData::setJointsFromJointSet(int i_) -- there is no training joint set i_ = " << ofToString(i_) << endl;
            cout << " -- myTrnJointSets.size() = " << ofToString(myTrnJointSets.size()) << endl;
            return;
        }
        
        // copy positions from training set to user-specified joints vector
        for (int jnt = 0; jnt < myJointsCount; ++jnt) {
            myJoints[jnt]->setPosition3D(myTrnJointSets[i_][jnt]->x, myTrnJointSets[i_][jnt]->y, myTrnJointSets[i_][jnt]->z);
        }
        
        // set a flag to make sure user-specified skeleton gets saved
        myJointsEdited = true;
    };
    
private:

    // internal data

};

//#endif
