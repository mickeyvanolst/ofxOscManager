#pragma once

#include "ofMain.h"
#include "ofxOscManager.h"

class testApp : public ofBaseApp, public ofxOscListener {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    void receivedOsc(ofxOscMessage &m);
    
    ofxOscServer server;
    ofxOscClient a,b;
};
