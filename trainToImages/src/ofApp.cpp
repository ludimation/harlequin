#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //------load images from folder
    ofDirectory dir;
    nFiles = dir.listDir("images");
    if(nFiles) {
        for(int i=0; i<dir.numFiles(); i++) {
            cout << "loading image: " << i << " out of: " << nFiles << endl;
            // add the image to the vector
            string filePath = dir.getPath(i);
            images.push_back(ofImage());
            images.back().loadImage(filePath);
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if(images[cImage].width > images[cImage].height) {
        iRatio = 400 / (float)images[cImage].width;
    } else {
        iRatio = 400 / (float)images[cImage].height;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    images[cImage].draw(10, 10, images[cImage].width * iRatio, images[cImage].height * iRatio);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case 'a' :
            cImage ++ ;
            break;
        case 'z':
            cImage -- ;
            break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
