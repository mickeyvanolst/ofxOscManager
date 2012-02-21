//
//  network.cpp
//  emptyExample
//
//  Created by Mick van Olst on 11-11-11.
//  Copyright (c) 2011 POPSURE. All rights reserved.
//
#include "ofxOscManager.h"

//--------------------------------------------------------------
oscMangager::oscMangager()
{
    serverExists        = false;
    initOnce            = false;
    checkHelloMsg       = false;
    connectedToServer   = false;
    newClientConnected  = false;
    newControllerConnected = false;
    newConnection       = false;
    newConCreated       = false;
    iamserver           = false;
    conExists           = false;
    hasMessages         = false;
    reconnectSenderIndex = 0;
    lastTriedConnecting = 0;
}

//--------------------------------------------------------------
void oscMangager::update()
{
    hasMessages = false;
    
    // listen for general port connection
    if (iamserver)
    {
        while( receivers[0].hasWaitingMessages() )
        {   // receivers[0] is the first and main connection of the server
            // every client and controller has this port programmed as starting point
            
            
            // get the next message
            ofxOscMessage m;
            receivers[0].getNextMessage( &m );
            
            // check if its a hello message from a client or controller
            if( m.getAddress() == "/client/hello" || m.getAddress() == "/controller/hello" )
            {   
                string remoteMode;
                
                if(m.getAddress() == "/client/hello")
                {
                    remoteMode = "client";
                    newClientConnected = true;
                } 
                else if(m.getAddress() == "/controller/hello")
                {
                    remoteMode = "controller";
                    newControllerConnected = true;
                }
                
                newConnection = true;
                
                
                for(int i=0;i<connections.size();i++)
                {
                    if(connections[i].ipAddr == ofToString(m.getRemoteIp()))
                    {
                        if(connections[i].type == remoteMode)
                        {
                            conExists = true;
                            printf("memory ip:%s\n",connections[i].ipAddr.c_str());
                            printf("remote ip:%s\n",m.getRemoteIp().c_str());
                            printf("is al verbonden\n");
                            
                            reconnectSender[reconnectSenderIndex].setup(m.getRemoteIp(), serverPortOut);
                            ofxOscMessage m;
                            m.setAddress("/server/hello");
                            m.addIntArg(serverPortOut + connections.size()-1);
                            m.addIntArg(serverPortIn + connections.size()-1);
                            reconnectSender[reconnectSenderIndex].sendMessage(m);
                            reconnectSenderIndex++;
                        }
                    } 
                    else
                    {
                        conExists = false;
                    }
                }
                
                if(!conExists)
                {   // if the connection is not in the servers database add it and send something back
                    
                    addConnection(m.getRemoteIp(), m.getAddress());
                    printf("remote ip: %s\n",m.getRemoteIp().c_str());
                    
                    // send back what port is gonna be used for talking
                    ofxOscMessage m;
                    m.setAddress("/server/hello");
                    m.addIntArg(serverPortOut + connections.size()-1);
                    m.addIntArg(serverPortIn + connections.size()-1);
                    senders[connections.size()-1].sendMessage(m);
                    
                    // set the connection to the new port after telling the client through the old port
                    senders[connections.size()-1].setup(connections[connections.size()-1].ipAddr, serverPortOut+(connections.size()-1));
                    
                    printf("senders[%lu].setup ip: %s port:%lu)\n",connections.size()-1,connections[connections.size()-1].ipAddr.c_str(),serverPortOut+(connections.size()-1));
                    
                    // add a receiver to listen to messages from the client
                    receivers[connections.size()-1].setup(serverPortIn+(connections.size()-1));
                }
            }
            else 
            {
                
                printDebug(m);
                msg = m;
                hasMessages = true;
            }
        }
        
        // check for messages from all other connections 
        // that is not the servers main entry connection
        for(int i = 1;i<connections.size();i++)
        {   
            while( receivers[i].hasWaitingMessages() )
            {
                ofxOscMessage m;
                receivers[i].getNextMessage( &m );
                
                if(connections[i].type != "controller")
                {
                    printDebug(m);
                    msg = m;
                    hasMessages = true;
                }
                
                if(connections[i].type == "controller")
                {
                    for(int i=0;i<connections.size();i++)
                    {
                        if(connections[i].type != "controller")
                        {
                            senders[i].sendMessage(m);
                        } 
                    }
                }
            }
        }
    }
    
    // if the client is not connected, say hello to server and check for response
    if (!connectedToServer && ofGetElapsedTimef()-lastTriedConnecting > 1 && !iamserver)
    {
        lastTriedConnecting = ofGetElapsedTimef();
        
        // say hello to server if your not connected yet
        if(checkHelloMsg)
        {
            if(!connectedToServer)
            {
                printf("hallo zeggen tegen server\n");
                ofxOscMessage m;
                m.setAddress("/"+ currentMode +"/hello");
                m.addStringArg("hi there");
                helloSender.sendMessage(m);
            }
            
        }
        
        // check if the helloReceiver has any messages back from server
        while( helloReceiver.hasWaitingMessages() )
        {
            // get the next message
            ofxOscMessage m;
            helloReceiver.getNextMessage( &m );
            
            if ( m.getAddress() == "/server/hello" )
            {
                printf("server connected\n");
                connectedToServer   = true;
                checkHelloMsg       = false;
                steadyPortIn        = m.getArgAsInt32(0);
                steadyPortOut       = m.getArgAsInt32(1);
                serverIp            = m.getRemoteIp();
            }
            
            printDebug(m);
        }
    }
    
    // client regular/steady message checking and adding steady receiver+sender
    if(connectedToServer && !iamserver && !newConCreated)
    {
        steadyReceiver.setup(steadyPortIn);
        steadySender.setup(serverIp,steadyPortOut);
        newConCreated = true;
        printf("connectedToServer received from server, now using port %i\n",steadyPortIn);
    }
    else if(connectedToServer && !iamserver && newConCreated) 
    {   // client regular/steady message checking
        while( steadyReceiver.hasWaitingMessages() )
        {
            // get the next message
            ofxOscMessage m;
            steadyReceiver.getNextMessage( &m );
            
            printDebug(m);
            
            msg = m;
            hasMessages = true;
        }
    }
}

//--------------------------------------------------------------
void oscMangager::setup(string serverIp, int portIn, int portOut, string myMode)
{   // setting up client/controller and connecting
    if(myMode != "server")
    {
        if(!connectedToServer) 
        {   
            helloSender.setup(serverIp, portIn); //serverPortIn refers to the server
            helloReceiver.setup(portOut);
            printf("connecting to server\n");
            printf("helloSender hostname: %s port: %i\n",serverIp.c_str(),portIn);
            printf("helloReceiver portOut: %i\n",portOut);
            
            // check for message from server
            checkHelloMsg = true;
            currentMode = myMode;
        } 
    }
    else if(myMode == "server")
    {
        if(!serverExists)
        {
            serverExists = true;
            iamserver = true;
            
            serverPortIn = portIn;
            serverPortOut = portOut;
            
            Connection connection;
            connection.type = "server";
            
            connections.push_back(connection);
            receivers[connections.size()-1].setup(serverPortIn);
            senders[connections.size()-1].setup( "localhost", serverPortIn);
            
            printf("server setup portIn: %i portOut: %i\n", portIn, portOut);
            printf("receivers[%lu] op poort: %i\n",connections.size()-1,serverPortIn);
            
        } 
        else 
        {
            printf("server already exists\n");
        }
    }
}

//--------------------------------------------------------------
void oscMangager::addConnection(string remoteIp, string OSCaddress)
{   // add new connection to connections list
    Connection connection;
    
    connection.portOut = serverPortOut + connections.size();
    connection.ipAddr = remoteIp;
    if( OSCaddress == "/client/hello")
    {
        connection.type = "client";
    } else if( OSCaddress == "/controller/hello")
    {
        connection.type = "controller";
    }
    
    // send message back with new connection, only still using the old port of course
    senders[connections.size()].setup( remoteIp, serverPortOut);
    printf("senders[%lu] hostIp: %s port: %lu\n",connections.size(),remoteIp.c_str(),serverPortOut+connections.size());
    
    connections.push_back(connection);
    
}

//--------------------------------------------------------------
void oscMangager::sendMsg(ofxOscMessage m)
{
    if(iamserver && connections.size() > 0)
    {
        for(int i=0;i<connections.size();i++)
        {
            //printf("nr con type: %i\n", i);
            if(connections[i].type != "controller")
            {
                senders[i].sendMessage(m);
            } 
            
        }
    } 
    else if(!iamserver && connectedToServer)
    {
        steadySender.sendMessage(m);
    }
    else
    {
        printf("no connections to send message to yet :-(\n");
    }
}

//--------------------------------------------------------------
void oscMangager::sendCtrMsg(ofxOscMessage m)
{
    if(iamserver && connections.size() > 0)
    {
        for(int i=0;i<connections.size();i++)
        {
            //printf("nr con type: %i\n", i);
            if(connections[i].type == "controller")
            {
                senders[i].sendMessage(m);
            } 
            
        }
    } 
    else if(!iamserver && connectedToServer)
    {
        steadySender.sendMessage(m);
    }
    else
    {
        printf("no connections to send message to yet :-(\n");
    }
}

//--------------------------------------------------------------
void oscMangager::printDebug(ofxOscMessage m)
{
    // unrecognized message: display on the bottom of the screen
    string msg_string;
    msg_string = m.getAddress();
    msg_string += ": ";
    for ( int i=0; i<m.getNumArgs(); i++ )
    {
        // get the argument type
        msg_string += m.getArgTypeName( i );
        msg_string += ":";
        // display the argument - make sure we get the right type
        if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
            msg_string += ofToString( m.getArgAsInt32( i ) ) ;
        else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
            msg_string += ofToString( m.getArgAsFloat( i ) );
        else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
            msg_string += m.getArgAsString( i );
        else
            msg_string += "unknown";
    }
    // add to the output window
    printf("OSC MESSAGE: %s\n",msg_string.c_str());
}