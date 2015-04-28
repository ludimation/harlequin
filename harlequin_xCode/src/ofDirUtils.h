//
//  ofDirUtils.h
//  harlequin
//
//  Created by student on 4/28/15.
//
//

//#ifndef harlequin_ofDirUtils_h
//#define harlequin_ofDirUtils_h

#pragma once

// TODO: make this accessible from all files in some way (use namespace, class or some other object?)

class ofDirUtils {
    
public:
    
    ofDirUtils() {
    }
    
    ~ofDirUtils {
    }
    
    ////--------------------------------------------------------------
    //vector<string> listFilesOfType(ofDirectory dir_, vector<string> extensions_ = vector<string>(), vector<string> fileList_ = vector<string>()) {
    //}
    
    //--------------------------------------------------------------
    vector<string> listFilesOfType(ofDirectory dir_, string ext_ = "", vector<string> fileList_ = vector<string>()) {
        int i, size;
        size = dir_.listDir();
        dir_.sort();
        vector<string> fileList;
        
        for (int i = 0; i < size; i++) {
            ofFile  fileObj         = dir_.getFile(i);
            bool    fileIsDir       = fileObj.isDirectory();
            string  filePath        = fileObj.path();
            string  fileExt         = fileObj.getExtension();
            if (fileIsDir) {
                ofDirectory newDir(filePath);
                fileList = listFilesOfType(newDir, ext_);
                for (int j = 0; j < fileList.size(); ++j) {
                    fileList_.push_back(fileList[j]);
                }
            }else if (fileExt == ext_ || ext_ == "") {
                fileList_.push_back(filePath);
                // debug
                cout << "dirUtils » listFilesOfType() -- filePath = " << filePath << endl;
            }
        }
        
        return fileList_;
    }
};


//#endif
