#include "testApp.h"

void testApp::setup() {
    ofBackground(0);
    ofSetFrameRate(60);
    
    server.setup(this);
    
    a.setup(this,"localhost");
    
    //clients[1].setup(this,"localhost");
    //clients[2].setup(this,"localhost");
    
}

void testApp::update() {

    server.update();
    
    a.update();
    //    b.update();
    
}

void testApp::keyPressed(int key) {
    if (key==' ') b.setup(this,"localhost");
}

void testApp::draw() {
    ofLine(ofGetWidth()/2,0,ofGetWidth()/2,ofGetHeight());
    ofLine(0,ofGetHeight()/2,ofGetWidth(),ofGetHeight()/2);
    
    //server.printDebug();
}

void testApp::receivedOsc(ofxOscMessage &m) {
    cout << this << ": receiveOsc" << endl;
    ofxOscManager::printDebug(m);
}