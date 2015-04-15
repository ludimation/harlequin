//
//  harlequinIMGEditor.h
//  harlequin
//
//  Created by david allen on 4/15/15.
//
//

#pragma once

//#ifndef harlequin_harlequinIMGEditor_h
//#define harlequin_harlequinIMGEditor_h

#include "MSAjoint.h"

class harlequinIMGEditor {
    
public:
    
    //
    ofxUISuperCanvas                *gui; // gui = new ofxUISuperCanvas("harlequin");
    vector< vector <MSAjoint*> >    joints;
    
    void setup() {
        /////////////////////
        // MSAjoint.h test //
        // TODO: clean these out once harlequinIMGEditor.h is working
        int objs_count = 20;
        joints.resize(1);
        
        for( int i = 0; i < objs_count; i++)
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
