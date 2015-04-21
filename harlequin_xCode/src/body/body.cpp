//
//  body.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "body.h"

//--------------------------------------------------------------
int body::getRandomExcluding(int min, int max, int i) {
    // TODO: Might want to simply hand over a vector of things to select from, and a second vector of filters to exclude?
    
    // exceptions
    if (max - min <= 1 and i <= max and i >= min) return -1;
    
    int value = i;
    
    while (value == i) {
        value = ofRandom(min, max);
    }
    
    return value;
}
