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
    string                          myBaseName;
    vector< string >                myImgPaths;
    string                          mySavePath;
    bool                            readOnly;
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
        jointsScale = 1.0f;
        readOnly = false;
        myJointsCount = 15;
        myJointsEdited = false;
        dragging = false;
        myJointSize = 15;
        myTrainingDataJointSize = 5;
        myAnchorInPercentages.set(0.5f, 0.35f);
        currentImgIndex = 0;
        myTrnJointSetsSize = myTrnJointSets.size();
        imgsMirrored = false;
        
        // populate gui joints vector
        for(int jnt = 0; jnt < myJointsCount; ++jnt) {
            MSAjoint *obj = new MSAjoint();
            obj->set(830 + (jnt * (myJointSize + 5)), myJointSize*3, myJointSize, myJointSize);
            // set user-specified joint color
            if (jnt == 0) { // set special properties for root joint
                obj->setSize(myJointSize*1.5f, myJointSize*1.5f);
                obj->setColors(MY_ROOT_JNT_COLOR, MY_ROOT_JNT_COLOR, MY_ROOT_JNT_COLOR);
                // obj->enableMouseEvents(); NOTE: this is ommitted so that the root joint cannot be moved
            } else {
                obj->setColors(MY_JNT_IDLE_COLOR, MY_JNT_OVER_COLOR, MY_JNT_DOWN_COLOR);
                obj->enableMouseEvents();
                obj->setDraggable(true);
            }
            myJoints.push_back(obj);
        }
        
        // handle interactive properties and functions
        setColors(IMG_IDLE_COLOR, IMG_OVER_COLOR, IMG_DOWN_COLOR);
        dragging = false;
        enableMouseEvents(); // needs to appear AFTER MSAjoint declarations so that mousePressed event catches updates to MSAjoints based on their mouse events
        setPosition(ofGetWidth()*myAnchorInPercentages.x, ofGetHeight()*myAnchorInPercentages.y);
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
        myBaseName = it_->first;
        myImgPaths = it_->second;
        mySavePath = imageJointDataDirectory_ + "/" + myBaseName + ".xml";
        jointsScale = jointsScale_;
        
        open(mySavePath, false);
    };
    
    bool open(string path_, bool readOnly_ = true) {
        mySavePath = path_;
        readOnly = readOnly_;
        
        if (readOnly) {
            disableAllEvents(); // disable all inherited ofxMSAInteractiveObject events
            for (int jnt = 0; jnt < myJoints.size(); ++jnt) {
                myJoints[jnt]->disableAllEvents();
            }
        }
        
        // try to load settings from XML file
        ofxXmlSettings imgDataXML;
        if(imgDataXML.loadFile(mySavePath)){
            // if it exists, set imgData properties to match
            
            // common variables
            int entriesCount;
            
            // core tag
            imgDataXML.pushTag("harlequinImgData_0.0.0");
            //
            // image anchor
            imgDataXML.pushTag("anchorPct");
            myAnchorInPercentages.x = imgDataXML.getValue("x", myAnchorInPercentages.x);
            myAnchorInPercentages.y = imgDataXML.getValue("y", myAnchorInPercentages.y);
            imgDataXML.popTag(); // pop "anchorPct"
            //
            // path scale map values
            imgDataXML.pushTag("pathScales");
            entriesCount = imgDataXML.getNumTags("entry");
            for(int entryIndx = 0; entryIndx < entriesCount; ++entryIndx) {
                // each entry tag represents one path
                imgDataXML.pushTag("entry", entryIndx);
                // get path and its corresponding scale
                // try to load image
                string imgPath = imgDataXML.getValue("path", "");
                float imgScale = imgDataXML.getValue("scale", 0.0f);
                myImgsPathScaleMap[imgPath] = imgScale;
                if (!readOnly) { // don't load images in read-only mode
                    ofImage *img = new ofImage();
                    if(img->loadImage(imgPath)) {
                        img->setAnchorPercent(myAnchorInPercentages.x, myAnchorInPercentages.y);
                        myImgs.push_back(img);
                    } else {
                        cout << "imgData::open() -- failed to load imgPath = " << imgPath << endl;
                    }
                }
                imgDataXML.popTag();//pop "entry"
            }
            currentImgIndex = myImgs.size() -1;
            imgDataXML.popTag(); //pop "pathScales"
            //
            // artist-specified joint positions (relative to the root joint)
            imgDataXML.pushTag("joints");
            entriesCount = imgDataXML.getNumTags("entry");
            if (myJointsCount == entriesCount) {
                myJointsEdited = true;
                for (int jnt = 0; jnt < entriesCount; ++jnt) {
                    // each joint entry contains x, y, and z values for one joint
                    imgDataXML.pushTag("entry", jnt);
                    // set path and its corresponding scale
                    float jntX = imgDataXML.getValue("x", 0.0f) * jointsScale + x   ;
                    float jntY = imgDataXML.getValue("y", 0.0f) * jointsScale + y   ;
                    float jntZ = imgDataXML.getValue("z", 0.0f) * jointsScale       ;
                    myJoints[jnt]->setPosition3D(jntX, jntY, jntZ);
                    imgDataXML.popTag();//pop "entry"
                }
            } else {
                cout << "imgData::open() -- number of joints in XML = " << entriesCount << ", but loader expects myJointsCount = " << myJointsCount << endl;
            }
            imgDataXML.popTag(); //pop "joints"
            //
            // training data joints
            imgDataXML.pushTag("trainingJointSets");
            int setsCount = imgDataXML.getNumTags("set");
            if (setsCount) {
                for(int set = 0; set < setsCount; ++set){
                    //each set contains one complete set of joints
                    imgDataXML.pushTag("set",set);
                    entriesCount = imgDataXML.getNumTags("entry");
                    if (myJointsCount == entriesCount) {
                        vector<ofPoint> jntsOfPnts;
                        for (int jnt = 0; jnt < entriesCount; ++jnt) {
                            // each joint entry contains x, y, and z values for one joint
                            imgDataXML.pushTag("entry",jnt);
                            // set path and its corresponding scale
                            float jntX = imgDataXML.getValue("x", 0.0f) * jointsScale + x   ;
                            float jntY = imgDataXML.getValue("y", 0.0f) * jointsScale + y   ;
                            float jntZ = imgDataXML.getValue("z", 0.0f) * jointsScale       ;
                            ofPoint jntOfPnt(jntX, jntY, jntZ);
                            jntsOfPnts.push_back(jntOfPnt);
                            imgDataXML.popTag();//pop "entry"
                        }
                        pushTrnData(jntsOfPnts);
                    } else {
                        cout << "imgData::open() -- number of joints in XML trainingJointSets[" << set << "] = " << entriesCount << ", but loader expects myJointsCount = " << myJointsCount << endl;
                    }
                    imgDataXML.popTag();//pop "set"
                }
            } else {
                cout << "imgData::open() -- XML file had 'trainingJointSets' tag, but it was empty" << endl;
            }
            imgDataXML.popTag(); // pop "trainingJointSets"
            imgDataXML.popTag(); // pop "harlequinImgData_0.0.0"


        } else {
            // otherwise, load images from "it" and calculate scales, using default falues for everything else
            for (int imgPathIndex = 0; imgPathIndex < myImgPaths.size(); ++imgPathIndex) {
                ofImage *img = new ofImage();
                string path = myImgPaths[imgPathIndex];
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
            //
            // set data loading-dependent properties
            currentImgIndex = myImgs.size() -1;
        }
    };
    
    void draw(bool drawMirrored_) {
        // store image scale for quick reference
        float imgScale = myImgsPathScaleMap[myImgPaths[currentImgIndex]]; // TODO: use ".at()" function instead? i.e.: myImgsPathScaleMap.at(it->second[currentImgIndex]);
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
        if (readOnly) return; // no matter what happens in readOnly mode, don't save any data

        //////////////////
        // create XML object
        //////////////////
        ofxXmlSettings imgDataXML;

        //////////////////
        // populate it with imgData properties
        //////////////////
        // core tag
        imgDataXML.addTag("harlequinImgData_0.0.0");
        imgDataXML.pushTag("harlequinImgData_0.0.0");
        //
        // image anchor
        imgDataXML.addTag("anchorPct");
        imgDataXML.pushTag("anchorPct");
        imgDataXML.addValue("x", myAnchorInPercentages.x);
        imgDataXML.addValue("y", myAnchorInPercentages.y);
        imgDataXML.popTag(); // pop "anchorPct"
        //
        // path scale map values
        imgDataXML.addTag("pathScales");
        imgDataXML.pushTag("pathScales");
        int entryIndx = 0;
        for(map<string, float>::iterator it = myImgsPathScaleMap.begin(); it != myImgsPathScaleMap.end(); ++it){
            // each entry tag represents one path
            imgDataXML.addTag("entry");
            imgDataXML.pushTag("entry", entryIndx);
            // set path and its corresponding scale
            imgDataXML.addValue("path", it->first);
            imgDataXML.addValue("scale", it->second);
            imgDataXML.popTag();//pop "entry"
            ++entryIndx;
        }
        imgDataXML.popTag(); //pop "pathScales"
        //
        // artist-specified joint positions (relative to the root joint)
        if (myJointsEdited) {
            imgDataXML.addTag("joints");
            imgDataXML.pushTag("joints");
            ofPoint baseJoint(myJoints[0]->x, myJoints[0]->y, myJoints[0]->z);
            for (int jnt = 0; jnt < myJoints.size(); ++jnt) {
                // each joint entry contains x, y, and z values for one joint
                imgDataXML.addTag("entry");
                imgDataXML.pushTag("entry",jnt);
                // undo affect of jointsScale by dividing MSAjoint positions / jointsScale;
                imgDataXML.addValue("x", (myJoints[jnt]->x - baseJoint.x) / jointsScale);
                imgDataXML.addValue("y", (myJoints[jnt]->y - baseJoint.y) / jointsScale);
                imgDataXML.addValue("z", (myJoints[jnt]->z - baseJoint.z) / jointsScale);
                imgDataXML.popTag();//pop "entry"
            }
            imgDataXML.popTag(); //pop "joints"
        }
        //
        // training data joints
        imgDataXML.addTag("trainingJointSets");
        imgDataXML.pushTag("trainingJointSets");
        for(int set = 0; set < myTrnJointSets.size(); ++set){
            //each set contains one complete set of joints
            imgDataXML.addTag("set");
            imgDataXML.pushTag("set",set);
            ofPoint baseJoint(myTrnJointSets[set][0]->x, myTrnJointSets[set][0]->y, myTrnJointSets[set][0]->z);
            for (int jnt = 0; jnt < myJoints.size(); ++jnt) {
                // each joint entry contains x, y, and z values for one joint
                imgDataXML.addTag("entry");
                imgDataXML.pushTag("entry",jnt);
                // undo affect of jointsScale by dividing MSAjoint positions / jointsScale;
                imgDataXML.addValue("x", (myTrnJointSets[set][jnt]->x - baseJoint.x) / jointsScale);
                imgDataXML.addValue("y", (myTrnJointSets[set][jnt]->y - baseJoint.y) / jointsScale);
                imgDataXML.addValue("z", (myTrnJointSets[set][jnt]->z - baseJoint.z) / jointsScale);
                imgDataXML.popTag();//pop "entry"
            }
            imgDataXML.popTag();//pop "set"
        }
        imgDataXML.popTag(); // pop "trainingJointSets"
        imgDataXML.popTag(); // pop "harlequinImgData_0.0.0"
        
        //////////////////
        // save it to the save directory with myBaseName + ".xml"
        //////////////////
        imgDataXML.saveFile(mySavePath);
        
    };
    
    // get training data functions
    vector<string> getImgPaths() {
        vector<string> imgPaths;
        for(map<string, float>::iterator imgIT = myImgsPathScaleMap.begin(); imgIT != myImgsPathScaleMap.end(); ++imgIT) {
            imgPaths.push_back(imgIT->first);
        }
        
        return imgPaths;
    }
    
    // training data manipulation functions
    void scaleImages(float pct_) {
        for(map<string, float>::iterator imgIT = myImgsPathScaleMap.begin(); imgIT != myImgsPathScaleMap.end(); ++imgIT) {
            imgIT->second *= pct_;
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
        // TODO: update joints dragging functionality so that only children of the selected joint get dragged with it
        //    - include a list of parent joints and children joints in each MSA joint
        //    - call "onPress" for all children
        //    - affect movement on the z-axis to preserve length of limb relative to parent
        //    - restrict x-y axis movement as well
        //    - switch from fwd to behind with modifier key
        if (jntBeingDragged != -1) {
            for (int jnt = 0; jnt < myJoints.size(); ++jnt) {
                // if (jnt != jntBeingDragged) myJoints[jnt]->onPress(x, y, button);
            }
            myJointsEdited = true;
        }
    }
    
    void pushTrnData(vector< ofPoint > jntsOfPnts_) {
        // if the number of pushed joints matches the expected number of joints
        if (jntsOfPnts_.size() != myJointsCount) {
            cout << "imgData::pushTrnData(vector<ofPoint> jntsOfPnts_) -- jntsOfPnts_.size() != myJointsCount " << endl;
            cout << " -- jntsOfPnts_.size() = " << ofToString(jntsOfPnts_.size()) << ", myJointsCount = " << myJointsCount << endl;
            return;
        }
        
        // create a vector of MSAjoint objects for each joint
        vector<MSAjoint*> tJoints;
        for (int jnt = 0; jnt < myJointsCount; ++jnt) {
            MSAjoint *obj = new MSAjoint();
            float jntX = jntsOfPnts_[jnt].x ;
            float jntY = jntsOfPnts_[jnt].y ;
            float jntZ = jntsOfPnts_[jnt].z ;
            obj->setPosition3D(jntX, jntY, jntZ);
            if (readOnly) obj->disableAllEvents(); // joints should not draw or update in read-only mode
            else {
                obj->setSize(myTrainingDataJointSize, myTrainingDataJointSize);
                obj->setColors(TRNDATA_JNT_IDLE_COLOR, TRNDATA_JNT_OVER_COLOR, TRNDATA_JNT_DOWN_COLOR);
            }
            tJoints.push_back(obj);
        }
        // push the vector to the vector of training joint sets
        myTrnJointSets.push_back(tJoints);
        
        myTrnJointSetsSize = myTrnJointSets.size();
        
        // cout << "imgData::pushTrnData(vector<ofPoint> &jntsOfPnts_) -- executed" << endl;
        // cout << " -- myTrnJointSetsSize = " << ofToString(myTrnJointSetsSize) << endl;
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
    
    vector< vector< double > > getJntDataDoubles() {
        // TODO: write getJntDataDoubles();
        // return a vector of user-trained joints in double format;
        
        vector< vector< double > > jointsDoubles;
        
        return jointsDoubles;
    }
    
    vector< vector< vector< double > > > getTrnDataDoubles() {
        // TODO: write getTrnDataDoubles();
        // return a vector of training data joint vectors in double format;
        vector< vector< vector< double > > > trnDataJointsDoubles;
        
        return trnDataJointsDoubles;
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
        float imgScale = myImgsPathScaleMap.at(myImgPaths[currentImgIndex]);
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
