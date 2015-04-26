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
    map <string, vector<string> >::iterator it;
    string                          myBaseName;
    string                          mySavePath;
    vector< ofImage* >              myImgs;
    bool                            imgsMirrored;
    int                             currentImgIndex;
    map< string, float >            myImgsPathScaleMap; // map of paths and their perspective scales relative to the joints skeleton
    float                           jointsScale;
    int                             myJointsCount;
    vector< MSAjoint* >             myJoints;
    vector< vector< MSAjoint* > >   myTrnJointSets;
    int                             myTrnJointSetsSize;
    bool                            myJointsEdited;
    bool                            dragging; // flag to check to see if the joints have been edited yet
    int                             myJointSize;
    int                             myTrainingDataJointSize;
    ofVec2f                         myAnchorInPercentages;

    // obj creator
    imgData() {
        // declare variables
        myBaseName = "";
        mySavePath = "";
        myJointsCount = 15;
        myJointsEdited = false;
        dragging = false;
        myJointSize = 10;
        myTrainingDataJointSize = 5;
        myAnchorInPercentages.set(0.5f, 0.5f);
        currentImgIndex = 0.0f;
        myTrnJointSetsSize = 0;
        imgsMirrored = false;
        
        // populate gui joints vector
        for(int jnt = 0; jnt < myJointsCount; ++jnt) {
            MSAjoint *obj = new MSAjoint();
            obj->set(830 + (jnt * (myJointSize + 5)), myJointSize*3, myJointSize, myJointSize);
            // set user-specified joint color
            obj->setColors(0xFFFF00, 0x00FF00, 0xFF00FF);
            obj->enableMouseEvents();
            obj->setDraggable(true);
            myJoints.push_back(obj);
        }
        
        // add mousepressed listener AFTER creating joints above so that it executes after one of them was pressed
        ofAddListener(ofEvents().mousePressed, this, &imgData::mousePressed);
        ofAddListener(ofEvents().mouseReleased, this, &imgData::mouseReleased);
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
    bool open(map <string, vector<string> >::iterator it_, string imageJointDataDirectory_, float jointsScale_) {
        // try to load the XML file
        // if it exists, set imgData properties to match
        // otherwise, use default data (should already be set up?
        
//        cout << "imgData::open() -- it->first = " << it->first << "; it->second = " << ofToString(it->second) << endl;
//        cout << "imgData::open() -- it_->first = " << it_->first << "; it_->second = " << ofToString(it_->second) << endl;
//        it = it_;
//    
//        mySavePath = imageJointDataDirectory_;
//        myBaseName = it->first;
//        currentImgIndex = it->second.size() -1;
//        jointsScale = jointsScale_;
//        for (int imgPathIndex = 0; imgPathIndex <it->second.size(); ++imgPathIndex) {
//            ofImage *img = new ofImage();
//            string path = it->second[imgPathIndex];
//            if (img->loadImage(path)) {
//                img->setAnchorPercent(myAnchorInPercentages.x, myAnchorInPercentages.y);
//                myImgs.push_back(img);
//                
//                // calculate imageScale
//                float imgRatioX;
//                float imgRatioY;
//                float imgScale;
//                if (img->width) {
//                    imgRatioX = float(ofGetWidth()) / float(img->width);
//                } else {
//                    imgRatioX = 1.0f;
//                }
//                if (img->height) {
//                    imgRatioY = float(ofGetHeight()) / float(img->height);
//                } else {
//                    imgRatioY = 1.0f;
//                }
//                if (imgRatioX < imgRatioY) {
//                    imgScale = imgRatioX;
//                } else {
//                    imgScale = imgRatioY;
//                }
//
//                myImgsPathScaleMap[path]= imgScale;
//            } else {
//                cout << "imgData::open() -- could not load image at path = " << path << endl;
//            }
//        }
    };
    
    void draw(bool drawMirrored_) {
        // store image scale for quick reference
        float imgScale = myImgsPathScaleMap[it->second[currentImgIndex]];
        // mirror image if necessary // TODO: is there a way to do this with a transformation instead?
        if (imgsMirrored != drawMirrored_) {
            for (int imgIndx = 0; imgIndx < myImgs.size(); ++imgIndx) {
                myImgs[imgIndx]->mirror(0, true);
            }
            imgsMirrored = drawMirrored_;
        }
        
        // draw image
        ofPushStyle();
        myImgs[currentImgIndex]->draw(
                                        ofGetWidth() / 2
                                      , ofGetHeight() / 2
                                      , myImgs[currentImgIndex]->width * imgScale
                                      , myImgs[currentImgIndex]->height * imgScale
                                      );
        ofPopStyle();

    };
    
    bool save() {
        // create XML object
        // populate it with imgData property values
        // save it to the save directory with myBaseName + ".xml"
        
        // undo affect of jointsScale by dividing MSAjoint positions / jointsScale;
    };
    
    // training data manipulation functions
    void mousePressed(ofMouseEventArgs &e) {
        
//        cout << "imgData::mousePressed() -- executing" << endl;
        
        int x = e.x;
        int y = e.y;
        int button = e.button;
        int jntBeingDragged = -1;
        
        // cycle through myJoints to see if any of them are being dragged
        for (int jnt = 0; jnt < myJoints.size(); ++jnt) {
            if (myJoints[jnt] -> getDragging()) {
                jntBeingDragged = jnt;
                
//                cout << " -- myJoints[" << ofToString(jnt) << "] -> getDragging() = " << ofToString(myJoints[jnt] -> getDragging()) << endl;
            }
        }
//        cout << " -- jntBeingDragged = " << ofToString(jntBeingDragged) << endl;
        
        // cycle through myJoints again to send mousePressed message so they all drag together
        if (jntBeingDragged != -1) {
            for (int jnt = 0; jnt < myJoints.size(); ++jnt) {
                if (jnt != jntBeingDragged) myJoints[jnt]->onPress(x, y, button);
            }
            dragging = true;
            myJointsEdited = true;
        }
    }
    
    void mouseReleased(ofMouseEventArgs &e) {
        if (dragging) {
            dragging = false;
//            // update the anchor of image (as percentage) in data based on position of root joint relative to the center of the image
//            float xPct = (myJoints[0]->x - (ofGetWidth()/2.0f)) /  * (float)myImgs[currentImgIndex]->width / (float)ofGetWidth();
//            float yPct = 0.0f;
        }
    }

    void pushTrnData(vector< MSAjoint* > &tJoints_) {
//        // if the number of pushed joints matches the expected number of joints
//        if (tJoints_.size() != myJointsCount) {
//            cout << "imgData::pushTrnData(vector<MSAjoint *> tJoints_) -- tJoints_.size() != myJointsCount " << endl;
//            cout << " -- tJoints_.size() = " << ofToString(tJoints_.size()) << ", myJointsCount = " << myJointsCount << endl;
//            return;
//        }
//        
//        // create a vector of MSAjoint objects for each joint
//        vector<MSAjoint*> tJoints;
//        for (int jnt = 0; jnt < myJointsCount; ++jnt) {
//            MSAjoint *obj = new MSAjoint();
//            obj->setPosition3D(tJoints_[jnt]->x, tJoints_[jnt]->y, tJoints_[jnt]->z);
//            obj->setSize(myTrainingDataJointSize, myTrainingDataJointSize);
//            tJoints.push_back(obj);
//        }
//        // push the vector to the vector of training joint sets
//        myTrnJointSets.push_back(tJoints);

        //        int myTrnJointSetsSize;
        myTrnJointSetsSize++;
        
        cout << "imgData::pushTrnData(vector<MSAjoint *> &tJoints_) -- executed" << endl;
    };
    
    int getTrnDataSize () {
        return 0;
//        return myTrnJointSetsSize;
    }
    
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
        myTrnJointSetsSize--;
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
