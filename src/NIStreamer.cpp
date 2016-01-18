//
//  NIStreamer.cpp
//  OpenNIServer
//
//  Created by bdorse on 12/7/15.
//
//

#include "ofApp.h"
#include "NIStreamer.h"

std::string NIStreamer::toJSONString(const Json::Value& json) {
    Json::FastWriter writer;
    return writer.write(json);
}

Json::Value NIStreamer::toJSONMethod(const std::string& module, const std::string& method, const Json::Value& params) {
    Json::Value json;
    json["OpenNIServer"] = "1.0";
    json["module"] = module;
    json["method"] = method;
    json["params"] = params;
    return json;
}

Json::Value NIStreamer::toJSON(ofxNiTE2::User::Ref user, const ofxNiTE2::UserTracker& tracker) {
    
    int floatPrecision = 1000;
    ofxJSONElement userJson;
    
    userJson["id"] = user->getId();
    userJson["centerOfMass"]["x"] = roundFloat(user->getCenterOfMass().x, floatPrecision);
    userJson["centerOfMass"]["y"] = roundFloat(user->getCenterOfMass().y, floatPrecision);
    userJson["centerOfMass"]["z"] = roundFloat(user->getCenterOfMass().z, floatPrecision);

    userJson["centerOfBone"]["x"] = roundFloat(user->getCenterOfBone().x, floatPrecision);
    userJson["centerOfBone"]["y"] = roundFloat(user->getCenterOfBone().y, floatPrecision);
    userJson["centerOfBone"]["z"] = roundFloat(user->getCenterOfBone().z, floatPrecision);

    const nite::BoundingBox& bBox = user->get().getBoundingBox();
    userJson["boundingBox"]["min"]["x"] = roundFloat(bBox.min.x, floatPrecision);
    userJson["boundingBox"]["min"]["y"] = roundFloat(bBox.min.y, floatPrecision);
    userJson["boundingBox"]["min"]["z"] = roundFloat(bBox.min.z, floatPrecision);

    userJson["boundingBox"]["max"]["x"] = roundFloat(bBox.max.x, floatPrecision);
    userJson["boundingBox"]["max"]["y"] = roundFloat(bBox.max.y, floatPrecision);
    userJson["boundingBox"]["max"]["z"] = roundFloat(bBox.max.z, floatPrecision);

    userJson["state"]["new"] = user->isNew();
    userJson["state"]["visible"] = user->isVisible();
    userJson["state"]["lost"] = user->isLost();

    const nite::PoseData& crossedHandsPose = user->get().getPose(nite::POSE_CROSSED_HANDS);
    userJson["pose"]["CROSSED_HANDS"]["held"] = crossedHandsPose.isHeld();
    userJson["pose"]["CROSSED_HANDS"]["entered"] = crossedHandsPose.isEntered();
    userJson["pose"]["CROSSED_HANDS"]["exited"] = crossedHandsPose.isExited();

    const nite::PoseData& psiPose = user->get().getPose(nite::POSE_PSI);
    userJson["pose"]["PSI"]["held"] = psiPose.isHeld();
    userJson["pose"]["PSI"]["entered"] = psiPose.isEntered();
    userJson["pose"]["PSI"]["exited"] = psiPose.isExited();

    switch(user->get().getSkeleton().getState()) {
       case nite::SKELETON_NONE:
           userJson["skeleton"]["state"] = "NONE";
           break;
           
       case nite::SKELETON_CALIBRATING:
           userJson["skeleton"]["state"] = "CALIBRATING";
           break;
           
       case nite::SKELETON_TRACKED:
           userJson["skeleton"]["state"] = "TRACKED";
           break;
           
       case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
           userJson["skeleton"]["state"] = "ERROR_NOT_IN_POSE";
           break;
           
       case nite::SKELETON_CALIBRATION_ERROR_HANDS:
           userJson["skeleton"]["state"] = "ERROR_HANDS";
           break;
           
       case nite::SKELETON_CALIBRATION_ERROR_HEAD:
           userJson["skeleton"]["state"] = "ERROR_HEAD";
           break;
           
       case nite::SKELETON_CALIBRATION_ERROR_LEGS:
           userJson["skeleton"]["state"] = "ERROR_LEGS";
           break;
           
       case nite::SKELETON_CALIBRATION_ERROR_TORSO:
           userJson["skeleton"]["state"] = "ERROR_TORSO";
           break;
           
    };

    float projectiveX, projectiveY;
    
    _addJoint(userJson["skeleton"]["joints"], "head", user->getJoint(nite::JOINT_HEAD).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "neck", user->getJoint(nite::JOINT_NECK).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "leftShoulder", user->getJoint(nite::JOINT_LEFT_SHOULDER).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "rightShoulder", user->getJoint(nite::JOINT_RIGHT_SHOULDER).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "leftElbow", user->getJoint(nite::JOINT_LEFT_ELBOW).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "rightElbow", user->getJoint(nite::JOINT_RIGHT_ELBOW).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "leftHand", user->getJoint(nite::JOINT_LEFT_HAND).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "rightHand", user->getJoint(nite::JOINT_RIGHT_HAND).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "torso", user->getJoint(nite::JOINT_TORSO).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "leftHip", user->getJoint(nite::JOINT_LEFT_HIP).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "rightHip", user->getJoint(nite::JOINT_RIGHT_HIP).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "leftKnee", user->getJoint(nite::JOINT_LEFT_KNEE).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "rightKnee", user->getJoint(nite::JOINT_RIGHT_KNEE).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "leftFoot", user->getJoint(nite::JOINT_LEFT_FOOT).get(), tracker);
    _addJoint(userJson["skeleton"]["joints"], "rightFoot", user->getJoint(nite::JOINT_RIGHT_FOOT).get(), tracker);
    
    return userJson;
}

void NIStreamer::_addJoint(Json::Value& json, const std::string& name, const nite::SkeletonJoint& joint, const ofxNiTE2::UserTracker& tracker) {
    
    int floatPrecision = 1000;
    float projectiveX, projectiveY;
    tracker.convertJointCoordinatesToDepth(joint.getPosition().x, joint.getPosition().y, joint.getPosition().z, &projectiveX, &projectiveY);
    json[name]["position"]["real"]["x"] = roundFloat(joint.getPosition().x, floatPrecision);
    json[name]["position"]["real"]["y"] = roundFloat(joint.getPosition().y, floatPrecision);
    json[name]["position"]["real"]["z"] = roundFloat(joint.getPosition().z, floatPrecision);
    json[name]["position"]["projective"]["x"] = isnan(projectiveX) ? 0 : roundFloat(projectiveX, floatPrecision);
    json[name]["position"]["projective"]["y"] = isnan(projectiveY) ? 0 : roundFloat(projectiveY, floatPrecision);
    json[name]["position"]["projective"]["z"] = roundFloat(joint.getPosition().z, floatPrecision);
    json[name]["positionConfidence"] = roundFloat(joint.getPositionConfidence(), floatPrecision);
    json[name]["orientation"]["w"] = roundFloat(joint.getOrientation().w, floatPrecision);
    json[name]["orientation"]["x"] = roundFloat(joint.getOrientation().x, floatPrecision);
    json[name]["orientation"]["y"] = roundFloat(joint.getOrientation().y, floatPrecision);
    json[name]["orientation"]["z"] = roundFloat(joint.getOrientation().z, floatPrecision);
    json[name]["orientationConfidence"] = roundFloat(joint.getOrientationConfidence(), floatPrecision);
}

NIStreamer::NIStreamer() :
_debug(true),
count(0){
    
    ofx::HTTP::JSONRPCServerSettings settings;
    settings.setPort(8197);
    
    // Initialize the server.
    _server.setup(settings);
    
    _server.getWebSocketRoute().registerWebSocketEvents(this);
    
    // Register RPC methods.
    _server.registerMethod("setSkeletonSmoothingFactor",
                           "Set the smoothing factor of the user tracker.",
                           this,
                           &NIStreamer::_onSetSkeletonSmoothingFactor);
    
    _server.registerMethod("getSkeletonSmoothingFactor",
                           "Get the smoothing factor of the user tracker.",
                           this,
                           &NIStreamer::_onGetSkeletonSmoothingFactor);
    
    _server.registerMethod("getDevices",
                           "Get all connected depth devices as an array.",
                           this,
                           &NIStreamer::_onGetDevices);

    
    _server.start();

};

NIStreamer::~NIStreamer() {
    _server.getWebSocketRoute().unregisterWebSocketEvents(this);
};

void NIStreamer::sendNewUser(ofxNiTE2::User::Ref& user, const ofxNiTE2::UserTracker& tracker) {
    
    if (_debug) {
        ofLogNotice("NIStreamer::sendNewUser") << "Sending new user #" << user->getId();
    }
   
    ofxJSONElement json;
    ofx::HTTP::WebSocketFrame frame(toJSONString(toJSONMethod("NIStreamer", "userNew", toJSON(user, tracker))));
    _server.getWebSocketRoute().broadcast(frame);
};

void NIStreamer::sendLostUser(ofxNiTE2::User::Ref& user, const ofxNiTE2::UserTracker& tracker) {
   
    if (_debug) {
        ofLogNotice("NIStreamer::sendNewUser") << "Sending lost user #" << user->getId();
    }
    
    ofxJSONElement json;
    ofx::HTTP::WebSocketFrame frame(toJSONString(toJSONMethod("NIStreamer", "userLost", toJSON(user, tracker))));
    _server.getWebSocketRoute().broadcast(frame);
};

void NIStreamer::sendTrackingData(ofxNiTE2::UserTracker& tracker) {
    
    ofxJSONElement json;
    json["users"] = Json::arrayValue;
    
//    if (tracker.getFrame().isValid()) {
//        nite::Plane floor = tracker.getFrame().getFloor();
//        json["floor"]["point"]["x"] = floor.point.x;
//        json["floor"]["point"]["y"] = floor.point.y;
//        json["floor"]["point"]["z"] = floor.point.z;
//        json["floor"]["normal"]["x"] = floor.normal.x;
//        json["floor"]["normal"]["y"] = floor.normal.y;
//        json["floor"]["normal"]["z"] = floor.normal.z;
//        json["floorConfidence"] = tracker.getFrame().getFloorConfidence();
//
//    }
    
    for (int i = 0; i < tracker.getNumUser(); i++) {
        ofxJSONElement userJson = toJSON(tracker.getUser(i), tracker);
        json["users"].append(userJson);
    }
    
    ofx::HTTP::WebSocketFrame frame(toJSONString(toJSONMethod("NIStreamer", "trackingFrame", json)));
    _server.getWebSocketRoute().broadcast(frame);
};

void NIStreamer::sendDepthData(ofShortPixels& pixels) {
    
//    ofBuffer buff((char*) pixels.getData(), (std::size_t) pixels.size());
//    ofx::HTTP::WebSocketFrame frame(toJSONString(toJSONMethod("NIStreamer", "trackingFrame", json)));
//    _server.getWebSocketRoute().broadcast(frame);
    
    ofShortPixels p = pixels; // copy
    p.resize(pixels.getWidth() * 0.5, pixels.getHeight() * 0.5);

//    unsigned char buf[] = {'f', 'f', 'f' , 'f', 'f', 'f'};
//    ofx::HTTP::WebSocketFrame frame(buf, (std::size_t) sizeof(buf), Poco::Net::WebSocket::FRAME_BINARY);
    ofx::HTTP::WebSocketFrame frame((unsigned char *) p.getData(), (std::size_t) p.getTotalBytes(), Poco::Net::WebSocket::FRAME_BINARY);
    _server.getWebSocketRoute().broadcast(frame);
};

void NIStreamer::saveDepthData(ofShortPixels &pixels) {
    std::string filename = ofToDataPath(ofGetTimestampString() + ".bin");
    ofLogNotice() << "Saving depth data to " << filename << endl;
    ofBuffer buff((char *) pixels.getData(), (std::size_t) pixels.getTotalBytes());
    ofFile file(filename, ofFile::ReadWrite);
    file.create();
    file.writeFromBuffer(buff);
    file.close();
};

//------------------------------WEBSOCKET EVENTS--------------------------------//

bool NIStreamer::onWebSocketOpenEvent(ofx::HTTP::WebSocketOpenEventArgs& args) {
    ofLogNotice("NIStreamer::onWebSocketOpenEvent") << "websocket opened";
};

bool NIStreamer::onWebSocketCloseEvent(ofx::HTTP::WebSocketCloseEventArgs& args) {
    ofLogNotice("NIStreamer::onWebSocketCloseEvent") << "websocket closed";
};

bool NIStreamer::onWebSocketFrameReceivedEvent(ofx::HTTP::WebSocketFrameEventArgs& args) {
    ofLogVerbose("NIStreamer::onWebSocketFrameReceivedEvent") << "websocket frame received";
};

bool NIStreamer::onWebSocketFrameSentEvent(ofx::HTTP::WebSocketFrameEventArgs& args) {
    ofLogVerbose("NIStreamer::onWebSocketFrameSentEvent") << "websocket frame sent";
    ofLogNotice() << args.getFrame().size() << " bytes sent"; 
    count++;
};

bool NIStreamer::onWebSocketErrorEvent(ofx::HTTP::WebSocketErrorEventArgs& args) {
    ofLogVerbose("NIStreamer::onWebSocketErrorEvent") << "websocket error event";
};

//------------------------------JSONRPC EVENTS--------------------------------//

void NIStreamer::_onSetSkeletonSmoothingFactor(ofx::JSONRPC::MethodArgs& args) {
    float smoothing = args.params.asFloat();
    ofNotifyEvent(_setSmoothingE, smoothing, this);
}

void NIStreamer::_onGetSkeletonSmoothingFactor(ofx::JSONRPC::MethodArgs& args) {
    ofNotifyEvent(_getSmoothingE, args, this);
}

void NIStreamer::_onGetDevices(ofx::JSONRPC::MethodArgs& args) {
    ofNotifyEvent(_getDevicesE, args, this);
}