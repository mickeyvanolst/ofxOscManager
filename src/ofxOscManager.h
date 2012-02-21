//
//  ofxOscManager.h
//  emptyExample
//
//  Created by Mick van Olst on 11-11-11.
//  Copyright (c) 2011 POPSURE. All rights reserved.
//

#pragma once

#include "ofxOsc.h"

#define MAX_CONNECTIONS 100 // max aantal senders

class ofxOscListener {
public:
    
    virtual void receivedOsc(ofxOscMessage &m) = 0;
    
};

class ofxOscManager {
    
public: 
    ofxOscManager();
    
    void update();
    void setup(string serverIp, int portIn, int portOut, string myMode);
    
    void sendMsg(ofxOscMessage m);
    void sendCtrMsg(ofxOscMessage m);
    
    bool                    connectedToServer;
    bool                    newClientConnected;
    bool                    newControllerConnected;
    bool                    newConnection;
    bool                    hasMessages;
    int                     serverPortIn;
    int                     serverPortOut;
    
    ofxOscMessage           msg;

    static void printDebug(ofxOscMessage &m);
    
private:
    void addConnection(string remoteIp, string OSCaddress);
    
    bool                    iamserver;
    bool                    serverExists;
    bool                    initOnce;
    bool                    checkHelloMsg;
    bool                    newConCreated;
    bool                    conExists;
    
    string                  serverIp;
    string                  currentMode;
    
    string                  oscAddress;
    
    ofxOscSender            helloSender;
    ofxOscReceiver          *helloReceiver;
    
    ofxOscReceiver          steadyReceiver;
    ofxOscSender            steadySender;
    
    int tmpServerPortAccepting;
    int                     steadyPortIn;
    int                     steadyPortOut;
    
    int                     lastTriedConnecting;
    
    struct Connection {
        int                 portIn;
        int                 portOut;
        string              ipAddr;
        string              type; // server, client, controller
    };
    vector <Connection>     connections;
    
    ofxOscReceiver          receivers[MAX_CONNECTIONS];
    ofxOscSender            senders[MAX_CONNECTIONS];
    
    ofxOscSender            reconnectSender[MAX_CONNECTIONS];
    int                     reconnectSenderIndex;

protected:
    ofxOscListener *listener; //in manager
};

class ofxOscServer : public ofxOscManager {
public:
    void setup(ofxOscListener *listener, int serverPort=7000, int clientPort=9000) {
        ofxOscManager::setup("localhost",serverPort,clientPort,"server");
        this->listener = listener;
    }
};

class ofxOscClient : public ofxOscManager {
public:
    void setup(ofxOscListener *listener, string serverIp="localhost", int serverPort=7000, int clientPort=9000) {
        ofxOscManager::setup(serverIp,serverPort,clientPort,"client");
        this->listener = listener;
    }
};


