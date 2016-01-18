//
//  NIStreamer.h
//  OpenNIServer
//
//  Created by bdorse on 12/7/15.
//
//

#ifndef __OpenNIServer__NIStreamer__
#define __OpenNIServer__NIStreamer__

#include "ofxNI2.h"
#include "ofxNiTE2.h"
#include "ofxJSON.h"
#include "ofxJSONRPC.h"

class ofApp;

class NIStreamer {
    
public:
    
    static std::string toJSONString(const Json::Value& json);
    static Json::Value toJSONMethod(const std::string& module, const std::string& method, const Json::Value& params);
    static Json::Value toJSON(ofxNiTE2::User::Ref user, const ofxNiTE2::UserTracker& tracker);
    static float roundFloat(float val, float decimalPlace) {
        return roundf(val * decimalPlace) / decimalPlace;
    };
    
    NIStreamer();
    ~NIStreamer();
    
//    void registerEvents(ofApp* listener);
    void sendNewUser(ofxNiTE2::User::Ref& user, const ofxNiTE2::UserTracker& tracker);
    void sendLostUser(ofxNiTE2::User::Ref& user, const ofxNiTE2::UserTracker& tracker);
    
    void sendTrackingData(ofxNiTE2::UserTracker& tracker);
    void sendDepthData(ofShortPixels& pixels);
    void saveDepthData(ofShortPixels& pixels);
    
    bool onWebSocketOpenEvent(ofx::HTTP::WebSocketOpenEventArgs& args);
    bool onWebSocketCloseEvent(ofx::HTTP::WebSocketCloseEventArgs& args);
    bool onWebSocketFrameReceivedEvent(ofx::HTTP::WebSocketFrameEventArgs& args);
    bool onWebSocketFrameSentEvent(ofx::HTTP::WebSocketFrameEventArgs& args);
    bool onWebSocketErrorEvent(ofx::HTTP::WebSocketErrorEventArgs& args);
    
    template<class ListenerClass>
    void registerEvents(ListenerClass* listener)
    {
        ofAddListener(_setSmoothingE, listener, &ListenerClass::onSetSkeletonSmoothing);
        ofAddListener(_getSmoothingE, listener, &ListenerClass::onGetSkeletonSmoothing);
        ofAddListener(_getDevicesE, listener, &ListenerClass::onGetDevices);
    }
    
    template<class ListenerClass>
    void unregisterEvents(ListenerClass* listener)
    {
        ofRemoveListener(_setSmoothingE, listener, &ListenerClass::onSetSkeletonSmoothing);
        ofRemoveListener(_getSmoothingE, listener, &ListenerClass::onGetSkeletonSmoothing);
        ofRemoveListener(_getDevicesE, listener, &ListenerClass::onGetDevices);
    }
    
    int count;
    
protected:
    
    static void _addJoint(Json::Value& json, const std::string& name, const nite::SkeletonJoint& joint, const ofxNiTE2::UserTracker& tracker);
    float _roundFloat(float val, float decimalPlace) const;
    void _onSetSkeletonSmoothingFactor(ofx::JSONRPC::MethodArgs& args);
    void _onGetSkeletonSmoothingFactor(ofx::JSONRPC::MethodArgs& args);
    void _onGetDevices(ofx::JSONRPC::MethodArgs& args);
    
    bool _debug;
    
    ofx::HTTP::JSONRPCServer _server;
    ofEvent<float> _setSmoothingE;
    ofEvent<ofx::JSONRPC::MethodArgs> _getSmoothingE;
    ofEvent<ofx::JSONRPC::MethodArgs> _getDevicesE;
};


#endif /* defined(__OpenNIServer__NIStreamer__) */
