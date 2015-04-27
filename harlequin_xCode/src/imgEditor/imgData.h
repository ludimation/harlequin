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

#include "ofxMSAInteractiveObject.h"
#include "ofxXmlSettings.h"
#include "MSAjoint.h"

#define		IMG_IDLE_COLOR          0xFFFFFF // white
#define		IMG_OVER_COLOR          0x888888 // grey
#define		IMG_DOWN_COLOR          0x000088 // dark blue
#define		MY_JNT_IDLE_COLOR       0xFFFF00 // yellow
#define		MY_JNT_OVER_COLOR       0xFF00FF // magenta
#define		MY_JNT_DOWN_COLOR       0xFFFF00 // cyan
#define     MY_ROOT_JNT_COLOR       0xFF8800 // orange
#define		TRNDATA_JNT_IDLE_COLOR  0x880088 // dark violet
#define		TRNDATA_JNT_OVER_COLOR  0xFFFFFF // white
#define		TRNDATA_JNT_DOWN_COLOR  0x00FF00 // dark green



class imgData : public ofxMSAInteractiveObject { // can I extend ": public ofImage {//" ?
    
public:
    
    // properties
    map <string, vector<string> >::iterator it;
    string                          myBaseName;
    string                          mySavePath;
    vector< ofImage* >              myImgs;
    int                             colorIdle, colorDown, colorOver;
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
        myJointSize = 15;
        myTrainingDataJointSize = 5;
        myAnchorInPercentages.set(0.5f, 0.35f);
        currentImgIndex = 0;
        myTrnJointSetsSize = myTrnJointSets.size();
        imgsMirrored = false;
        
        // handle interactive properties and functions
        setColors(IMG_IDLE_COLOR, IMG_OVER_COLOR, IMG_DOWN_COLOR);
        dragging = false;
        enableMouseEvents();
        setPosition(ofGetWidth()*myAnchorInPercentages.x, ofGetHeight()*myAnchorInPercentages.y);
        
        // populate gui joints vector
        for(int jnt = 0; jnt < myJointsCount; ++jnt) {
            MSAjoint *obj = new MSAjoint();
            obj->set(830 + (jnt * (myJointSize + 5)), myJointSize*3, myJointSize, myJointSize);
            // set user-specified joint color
            if (jnt == 0) { // set special properties for root joint
                obj->setSize(myJointSize*1.5f, myJointSize*1.5f);
                obj->setColors(MY_ROOT_JNT_COLOR, MY_ROOT_JNT_COLOR, MY_ROOT_JNT_COLOR);
            } else {
                obj->setColors(MY_JNT_IDLE_COLOR, MY_JNT_OVER_COLOR, MY_JNT_DOWN_COLOR);
                obj->enableMouseEvents();
                obj->setDraggable(true);
            }
            myJoints.push_back(obj);
        }
    };
    // obj destructor
    ~imgData() {
        
        // clean up images
        for (int i = 0; i < myImgs.size(); ++i) {
            delete myImgs[i]; myImgs[i] = NULL;
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
        // store arguments
        it = it_;
        mySavePath = imageJointDataDirectory_;
        myBaseName = it->first;
        currentImgIndex = it->second.size() -1;
        jointsScale = jointsScale_;
        
        // TODO: open() -- load XML file functionality
        // try to load the XML file
        // if it exists, set imgData properties to match
        
        // load images and calculate scales
        for (int imgPathIndex = 0; imgPathIndex <it->second.size(); ++imgPathIndex) {
            ofImage *img = new ofImage();
            string path = it->second[imgPathIndex];
            if (img->loadImage(path)) {
                img->setAnchorPercent(myAnchorInPercentages.x, myAnchorInPercentages.y);
                myImgs.push_back(img);
                
                // calculate imageScale
                float imgRatioX;
                float imgRatioY;
                float imgScale;
                if (img->width) {
                    imgRatioX = float(ofGetWidth()) / float(img->width);
                } else {
                    imgRatioX = 1.0f;
                }
                if (img->height) {
                    imgRatioY = float(ofGetHeight()) / float(img->height);
                } else {
                    imgRatioY = 1.0f;
                }
                if (imgRatioX < imgRatioY) {
                    imgScale = imgRatioX;
                } else {
                    imgScale = imgRatioY;
                }

                myImgsPathScaleMap[path]= imgScale;
            } else {
                cout << "imgData::open() -- could not load image at path = " << path << endl;
            }
        }
    };
    
    void draw(bool drawMirrored_) {
        // store image scale for quick reference
        float imgScale = myImgsPathScaleMap[it->second[currentImgIndex]]; // TODO: use ".at()" function instead? i.e.: myImgsPathScaleMap.at(it->second[currentImgIndex]);
        // mirror image if necessary // TODO: is there a way to do this with a transformation instead?
        if (imgsMirrored != drawMirrored_) {
            for (int imgIndx = 0; imgIndx < myImgs.size(); ++imgIndx) {
                myImgs[imgIndx]->mirror(0, true);
            }
            imgsMirrored = drawMirrored_;
        }
        
        // draw image
        ofPushStyle();
        
        if(isMousePressed()) ofSetHexColor(colorDown);
        else if(isMouseOver()) ofSetHexColor(colorOver);
        else ofSetHexColor(colorIdle);

        myImgs[currentImgIndex]->draw(
                                        x
                                      , y
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
    
    void scaleImages(float pct_) {
        for(int img = 0; img < myImgs.size(); ++img) {
            myImgsPathScaleMap[it->second[currentImgIndex]] *= pct_;
        }
    }
    
    void setColors(int colorIdle_, int colorOver_, int colorDown_) {
        colorIdle = colorIdle_;
        colorOver = colorOver_;
        colorDown = colorDown_;
    }

    void mousePressed(int x, int y, int button) {

        // cout << "imgData::mousePressed(x, y, button) -- executing" << endl;

        int jntBeingDragged = -1;

        // cycle through myJoints to see if any of them are being dragged
        for (int jnt = 0; jnt < myJoints.size(); ++jnt) {
            if (myJoints[jnt] -> getDragging()) {
                jntBeingDragged = jnt;
                
            }
        }
        
        // cycle through myJoints again to send mousePressed message so they all drag together
        if (jntBeingDragged != -1) {
            for (int jnt = 0; jnt < myJoints.size(); ++jnt) {
                if (jnt != jntBeingDragged) myJoints[jnt]->onPress(x, y, button);
            }
            myJointsEdited = true;
        }
    }
    
    void pushTrnData(vector< MSAjoint* > &tJoints_) {
        // if the number of pushed joints matches the expected number of joints
        if (tJoints_.size() != myJointsCount) {
            cout << "imgData::pushTrnData(vector<MSAjoint *> tJoints_) -- tJoints_.size() != myJointsCount " << endl;
            cout << " -- tJoints_.size() = " << ofToString(tJoints_.size()) << ", myJointsCount = " << myJointsCount << endl;
            return;
        }
        
        // capture position of root joint (all data will be relative to this joint's position)
        ofPoint rootPos(tJoints_[0]->x, tJoints_[0]->y, tJoints_[0]->z);
        // create a vector of MSAjoint objects for each joint
        vector<MSAjoint*> tJoints;
        for (int jnt = 0; jnt < myJointsCount; ++jnt) {
            MSAjoint *obj = new MSAjoint();
            float jntX = tJoints_[jnt]->x - rootPos.x   + this->x   ;
            float jntY = tJoints_[jnt]->y - rootPos.y   + this->y   ;
            float jntZ = tJoints_[jnt]->z - rootPos.z               ;
            obj->setPosition3D(jntX, jntY, jntZ);
            obj->setSize(myTrainingDataJointSize, myTrainingDataJointSize);
            obj->setColors(TRNDATA_JNT_IDLE_COLOR, TRNDATA_JNT_OVER_COLOR, TRNDATA_JNT_DOWN_COLOR);
            tJoints.push_back(obj);
        }
        // push the vector to the vector of training joint sets
        myTrnJointSets.push_back(tJoints);

        myTrnJointSetsSize = myTrnJointSets.size();
        
        // cout << "imgData::pushTrnData(vector<MSAjoint *> &tJoints_) -- executed" << endl;
        // cout << " -- myTrnJointSetsSize = " << ofToString(myTrnJointSetsSize) << endl;
    };
    
    int getTrnDataSize () {
        return myTrnJointSetsSize;
    }
    
    void setTrnDataVisibilty(int set_, bool visible_) {
        // if specified training set exists
        if (myTrnJointSets.size() < set_) {
            cout << "imgData::showTrnData(int i_) -- there is no training joint set i_ = " << ofToString(set_) << endl;
            cout << " -- myTrnJointSets.size() = " << ofToString(myTrnJointSets.size()) << endl;
            return;
        }
        
        for (int jnt = 0; jnt < myJointsCount; ++jnt) {
            if (visible_) {
                myTrnJointSets[set_][jnt]->enableAppEvents();
            } else {
                myTrnJointSets[set_][jnt]->disableAppEvents();
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
        myTrnJointSets.erase(myTrnJointSets.begin() + i_);
        myTrnJointSetsSize = myTrnJointSets.size();
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
    
    //    enum Joint { // joint reference from OpenNI code
    //
    //        // start at root joint
    //        JOINT_TORSO = 0,
    //        JOINT_NECK,
    //        JOINT_HEAD,
    //
    //        // left arm + shoulder
    //        JOINT_LEFT_SHOULDER,
    //        JOINT_LEFT_ELBOW,
    //        JOINT_LEFT_HAND,
    //
    //        // right arm + shoulder
    //        JOINT_RIGHT_SHOULDER,
    //        JOINT_RIGHT_ELBOW,
    //        JOINT_RIGHT_HAND,
    //
    //        // left leg
    //        JOINT_LEFT_HIP,
    //        JOINT_LEFT_KNEE,
    //        JOINT_LEFT_FOOT,
    //
    //        // right leg
    //        JOINT_RIGHT_HIP,
    //        JOINT_RIGHT_KNEE,
    //        JOINT_RIGHT_FOOT,
    //
    //        JOINT_COUNT,
    //        JOINT_UNKOWN
    //    };

    // mouse interaction
    bool hitTest(int tx, int ty) const { // redefines hitTest function of ofxMSAInteractiveObject
        float imgScale = myImgsPathScaleMap.at(it->second[currentImgIndex]);
        int imgWidth   = myImgs[currentImgIndex]->getWidth();
        int imgHeight  = myImgs[currentImgIndex]->getHeight();
        tx += myAnchorInPercentages.x * imgWidth * imgScale;
        ty += myAnchorInPercentages.y * imgHeight * imgScale;
        return ((tx > x) && (tx < x + imgWidth * imgScale) && (ty > y) && (ty < y + imgHeight * imgScale));
    }
    
    void startDragging() {
        dragging = true;
    }
    
    void stopDragging() {
        if (dragging) dragging = false;
    }
    
    bool getDragging() {
        return dragging;
    }
    
    void updatePosition(int x, int y) {
        float imgWidth = myImgs[currentImgIndex]->width;
        float imgHeight = myImgs[currentImgIndex]->height;
        float dx = (x - ofGetPreviousMouseX()) / imgWidth;
        float dy = (y - ofGetPreviousMouseY()) / imgHeight;
        this->myAnchorInPercentages.x -= dx;
        this->myAnchorInPercentages.y -= dy;
        for(int imgIndx = 0; imgIndx < myImgs.size(); imgIndx++) {
            myImgs[imgIndx]->setAnchorPercent(myAnchorInPercentages.x, myAnchorInPercentages.y);
        }
        
        // TODO: make sure images cannnot get dragged off the edge of the screen
        //        this->x -= (float)((int)this->x % ofGetWidth());
        //        this->y -= (float)((int)this->y % ofGetHeight());
        //        this->x = fmod(this->x, ofGetWidth());
        //        this->y = fmod(this->y, ofGetHeight());
        //
        //        if this-> >= ofGetWidth();
        
        
    }
    
    void mouseDragged(int x, int y, int button) {
        //        printf("MSAjoint::mouseDragged(x: %i, y: %i, button: %i)\n", x, y, button);
        if (dragging) updatePosition(x, y);
    }
    
    virtual void onPress(int x, int y, int button) {
        //		printf("MSAjoint::onPress(x: %i, y: %i, button: %i)\n", x, y, button);
        startDragging();
    }
    
    virtual void onRelease(int x, int y, int button) {
        //		printf("MSAjoint::onRelease(x: %i, y: %i, button: %i)\n", x, y, button);
        stopDragging();
    }
    
    virtual void onReleaseOutside(int x, int y, int button) {
        //		printf("MSAjoint::onReleaseOutside(x: %i, y: %i, button: %i)\n", x, y, button);
        stopDragging();
    }

    
    
private:

    // internal data

};

//#endif
