//
//  GRTEditor.cpp
//  harlequin
//
//  Created by david allen on 4/21/15.
//
//

#include "GRTEditor.h"

bool GRTEditor::loadData() {
    //    /////////////////////////////////////////
    //    // load training data and setup models //
    //    /////////////////////////////////////////
    //    trainingDataJointsPosABSfileName    = trainedImagesDirectory + "JointsPosABSdata.txt"; // TODO: make these relative to selected directories
    //    trainingModelJointsPosABSfileName   = trainedImagesDirectory + "JointsPosABSmodel.txt";
    //    trainingDataJointsPosRelfileName    = trainedImagesDirectory + "JointsPosReldata.txt";
    //    trainingModelJointsPosRelfileName   = trainedImagesDirectory + "JointsPosRelmodel.txt";
    //    trainingDataJointsRotAxisAfileName  = trainedImagesDirectory + "JointsRotAxisAdata.txt";
    //    trainingModelJointsRotAxisAfileName = trainedImagesDirectory + "JointsRotAxisAmodel.txt";
    //    //
    //    trainingDataJointsPosABS.setDatasetName("harlequinPosABS");
    //    trainingDataJointsPosABS.setNumDimensions(45);
    //    trainingDataJointsPosRel.setDatasetName("harlequinPosRel");
    //    trainingDataJointsPosRel.setNumDimensions(45);
    //    trainingDataJointsRotAxisA.setDatasetName("harlequinRotAxisA");
    //    trainingDataJointsRotAxisA.setNumDimensions(45);
    //    //
    //    //
    //    trainingDataJointsPosABS.loadDatasetFromFile(ofToDataPath(trainingDataJointsPosABSfileName));
    //    trainingDataJointsPosRel.loadDatasetFromFile(ofToDataPath(trainingDataJointsPosRelfileName));
    //    trainingDataJointsRotAxisA.loadDatasetFromFile(ofToDataPath(trainingDataJointsRotAxisAfileName));
    //
    //    trainModelsNow = true;
    //
    //    // TODO: implement the suggested pipeline setup
    //    GRT::SVM trainingModelJointsPosABS(GRT::SVM::LINEAR_KERNEL);
    //    GRT::SVM trainingModelJointsPosRel(GRT::SVM::LINEAR_KERNEL);
    //    GRT::SVM trainingModelJointsRotAxisA(GRT::SVM::LINEAR_KERNEL);
    
    cout << "GRTEditor::loadData() -- loaded training data" << endl;
    return true;
}

//--------------------------------------------------------------
bool GRTEditor::saveData(){
    //    trainingDataJointsPosABS.saveDatasetToFile(ofToDataPath(trainingDataJointsPosABSfileName));
    //    trainingDataJointsPosRel.saveDatasetToFile(ofToDataPath(trainingDataJointsPosRelfileName));
    //    trainingDataJointsRotAxisA.saveDatasetToFile(ofToDataPath(trainingDataJointsRotAxisAfileName));
    
    cout << "GRTEditor::saveData() -- saved training data" << endl;
    return true;
}


//--------------------------------------------------------------
bool GRTEditor::loadModel() {
    
//    // TODO: loading models only works when called outside startup()——could be fixed by implementing the suggested pipeline setup?
//    trainingModelJointsPosABS.loadModelFromFile(ofToDataPath(trainingModelJointsPosABSfileName));
//    trainingModelJointsPosRel.loadModelFromFile(ofToDataPath(trainingModelJointsPosRelfileName));
//    trainingModelJointsRotAxisA.loadModelFromFile(ofToDataPath(trainingModelJointsRotAxisAfileName));
//
//    trainModelsNow = false;
    
    cout << "GRTEditor::loadModel() -- loaded training model" << endl;
    return true;
}

//--------------------------------------------------------------
bool GRTEditor::trainModel() {
    
    // TODO: training model data only works when called outside startup()——could this also be fixed by implementing the suggested pipeline setup?
    //    trainingModelJointsPosABS.train(trainingDataJointsPosABS);
    //    trainingModelJointsPosRel.train(trainingDataJointsPosRel);
    //    trainingModelJointsRotAxisA.train(trainingDataJointsRotAxisA);
    //
    //    trainModelsNow = false;

    cout << "GRTEditor::trainModel() -- trained model" << endl;
    return true;
}

//--------------------------------------------------------------
bool GRTEditor::saveModel() {
    //    trainingModelJointsPosABS.train(trainingDataJointsPosABS);
    //    trainingModelJointsPosABS.saveModelToFile(ofToDataPath(trainingModelJointsPosABSfileName));
    //
    //    trainingModelJointsPosRel.train(trainingDataJointsPosRel);
    //    trainingModelJointsPosRel.saveModelToFile(ofToDataPath(trainingModelJointsPosRelfileName));
    //
    //    trainingModelJointsRotAxisA.train(trainingDataJointsRotAxisA);
    //    trainingModelJointsRotAxisA.saveModelToFile(ofToDataPath(trainingModelJointsRotAxisAfileName));
    cout << "GRTEditor::saveModel() -- saved training model" << endl;
    return true;
}