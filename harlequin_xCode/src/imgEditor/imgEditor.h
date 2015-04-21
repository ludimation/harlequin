//
//  imgEditor.h
//  harlequin
//
//  Created by david allen on 4/15/15.
//
//

#pragma once

//#ifndef harlequin_imgEditor_h
//#define harlequin_imgEditor_h

#include "MSAjoint.h"

class imgEditor {
    
public:
    
    //
    ofxUISuperCanvas                *gui; // gui = new ofxUISuperCanvas("harlequin");
    vector< vector <MSAjoint*> >    joints;
    
    void setup() {
        //////////////////////
        // MSA joints setup //
        //////////////////////
        int joints_count = 15;
        joints.resize(1);
        //
        for( int i = 0; i < joints_count; i++)
        {
            MSAjoint *obj = new MSAjoint();
            obj->set(300 + (i*15), 400, 10, 10);
            obj->enabled = true;
            obj->enableMouseEvents();
            joints[0].push_back(obj);
        }
    }
    
private:
    
    //
    
};

//#endif
