//
//  manageNetwork.h
//  emptyExample
//
//  Created by Mick van Olst on 11-11-11.
//  Copyright (c) 2011 POPSURE. All rights reserved.
//

#ifndef pyramidPlayer_network_h
#define pyramidPlayer_network_h
#include "ofxOsc.h"

//#ifndef PYRAMIDPLAYER_CONSTANTS
//#define PYRAMIDPLAYER_CONSTANTS
#define MAX_CONNECTIONS 100 // max aantal senders
//#endif

class manageNetwork {
    
public: 
    manageNetwork();
    
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
    
private:
    void addConnection(string remoteIp, string OSCaddress);
    void printDebug(ofxOscMessage m);
    
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
    ofxOscReceiver          helloReceiver;
    
    ofxOscReceiver          steadyReceiver;
    ofxOscSender            steadySender;
    
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
};
#endif
