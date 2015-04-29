//
//  imgGRTData.h
//  harlequin
//
//  Created by student on 4/28/15.
//
//

#ifndef harlequin_imgGRTData_h
#define harlequin_imgGRTData_h

#include "GRT.h"
using namespace GRT;

enum jointDataTypes {
    JOINT_DATA_TYPE_ABS = 0,
    JOINT_DATA_TYPE_REL,
    JOINT_DATA_TYPE_AA,
    JOINT_DATA_TYPE_COUNT
};

class imgGRTData {

public:

    // properties
    string                      baseName;
    string                      GRTFileSavePath;
    string                      XMLFileSavePath;
    vector< string >            tags;
    map< vector < vector < double > >, string > imgGRTDataPathMap; // index is GRT label number
    vector< jointDataTypes >    jntDataformat;
    
    // functions
    imgGRTData(string baseName_ = "", string savePath_ = "") {
        baseName = baseName_;
        GRTFileSavePath = savePath_ + baseName + ".txt";
        XMLFileSavePath = savePath_ + baseName + ".xml";
        open();
    }
    
    ~imgGRTData() {
        
    }
    
    void open() {
        // try to load XML settings if they exist
        // otherwise, use default values
    }
    
    void save() {
        ///////////////////////
        // save XML settings
        ///////////////////////
        //     - GRTFilename
        //     - tags
        //     - label-indexed image path list
        //     - joint data format
        
        
        ///////////////////////
        // build and save GRT txt data
        ///////////////////////
        //
        // Create a new instance of the ClassificationData
        ClassificationData trainingData;
        //
        // Set training data object properties
        int jointSets = 2;
        int jointsPerSet = 15;
        int dimensionsPerJoint = 3;
        int numDimensions = jointSets * jointsPerSet * dimensionsPerJoint;
        trainingData.setNumDimensions(numDimensions);
        trainingData.setDatasetName(baseName);
        trainingData.setInfoText("This GRT classification data file contains joint data for images with tags:" + ofJoinString(tags, ", ") + ".");
        //
        // TODO: add joint data for each entry in imgGRTDataPathMap
        for(map< vector < vector < double > >, string >::iterator it=imgGRTDataPathMap.begin(); it != imgGRTDataPathMap.end(); ++it) {
            UINT gestureLabel = distance(imgGRTDataPathMap.begin(), it);
            vector<double> sample(numDimensions);
            // For now we will just add some random data
            Random random;
            for(UINT i=0; i<100; i++){
                for(int j=0; j<numDimensions; ++j) {
                    sample[j] = random.getRandomNumberUniform(-1.0,1.0);
                }
                
                //Add the sample to the training data
                trainingData.addSample( gestureLabel, sample );
            }
        }
        
        //After recording your training data you can then save it to a file
        if( !trainingData.saveDatasetToFile( GRTFileSavePath ) ){
            cout << "ERROR: Failed to save dataset to file!\n";
            return EXIT_FAILURE;
        }
        
        //This can then be loaded later
        if( !trainingData.loadDatasetFromFile( GRTFileSavePath ) ){
            cout << "ERROR: Failed to load dataset from file!\n";
            return EXIT_FAILURE;
        }
    }
    
    string getBaseName () {
        return baseName;
    }
    
    void setBaseName (string baseName_ = "") {
        baseName = baseName_;
    }
    
    void setTagsList(vector< string > tags_ = vector< string >()) {
        tags = tags_;
    }
    
    vector< string > getTagsList() {
        return tags;
    }
    
    void addTags(vector< string > tags_ = vector< string >()) {
        
    }
    
    void addTag(string tag_ = "") {
        
    }
    
    void removeTag(string tag_ = "") {
        
    }
    
    void setJntDataFormat(vector< jointDataTypes > jntDataFormat_) {
        jntDataformat = jntDataFormat_;
    }
    
    vector< jointDataTypes > getJointDataFormat() {
        return jntDataformat;
    }
    
};


#endif
