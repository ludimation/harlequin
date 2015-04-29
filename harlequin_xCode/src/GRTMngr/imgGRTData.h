//
//  imgGRTData.h
//  harlequin
//
//  Created by student on 4/28/15.
//
//

#ifndef harlequin_imgGRTData_h
#define harlequin_imgGRTData_h

enum jointDataTypes {
    JOINT_DATA_TYPE_ABS = 0,
    JOINT_DATA_TYPE_REL,
    JOINT_DATA_TYPE_AA,
    JOINT_DATA_TYPE_COUNT
};

class imgGRTData {

public:

    // properties
    string                  baseName;
    string                  GRTFilePath;
    vector<string>          imgPaths;
    map<string, bool>       tags;
    vector<jointDataTypes>  jointDataTypesAssignment;
    
    // functions
    imgGRTData(string baseName_ = "", string GRTFilePath_ = "") {
        baseName = baseName_;
        GRTFilePath = GRTFilePath_;
        open();
    }
    
    ~imgGRTData() {
        
    }
    
    void open() {
        
    }
    
    void save() {
        
    }
    
    void setBaseName (baseName_ = "") {
        baseName = baseName_;
    }
    
    void setTagsList(vector< string > = vector< string >()) {
        
    }
    
    vector< string > getTagsList() {
        
    }
    
    void addTags(vector< string > tags_ = vector< string >()) {
        
    }
    
    void addTag(string tag_ = "") {
        
    }
    
    void removeTag(string tag_ = "") {
        
    }
    
    void setJointDataFormat(vector< jointDataTypes > jointDataTypeSet_) {
        
    }
    
    vector< jointDataTypes > getJointDataTypes() {
        
    }
    
};


#endif
