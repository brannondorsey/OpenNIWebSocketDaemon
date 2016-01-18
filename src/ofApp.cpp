#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetBackgroundColor(0);
    ofSetFrameRate(30);
//    ofSetLogLevel(OF_LOG_VERBOSE);
    lastSecond = ofGetElapsedTimeMillis();
    
    bDevicesInited = false;
    bTrackingEventsRegistered = false;
    bSendDepth = true;
    
    if (device.setup()) {
        ofLogNotice() << "device inited";
        bDevicesInited = true;
        
    } else {
        ofLogError() << "device NOT inited";
    }
    
    try {
        if (bDevicesInited && tracker.setup(device))
        {
            ofLogNotice() << "tracker inited";
            ofAddListener(tracker.newUser, this, &ofApp::onTrackerNewUser);
            ofAddListener(tracker.lostUser, this, &ofApp::onTrackerLostUser);
            bTrackingEventsRegistered = true;
        }
        else
        {
            ofLogError() << "tracker NOT inited";
        }
    }
    catch(std::runtime_error) {
        ofLogError() << "tracker NOT inited";
    }
    
//    try {
//        if(depth.setup(*device)){
//            depth.setSize(320, 240);
//            depth.setFps(30);
//            depth.start();
//        }
//    } catch (std::runtime_error err) {
//        ofLogError() << "depth NOT inited";
//    }
    
    niStreamer.registerEvents(this);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    device.update();
    depthImage.setFromPixels(tracker.getPixelsRef(1000, 9000));
    
    if (bSendDepth) {
        niStreamer.sendDepthData(tracker.getPixelsRef());
    }
    
    if (ofGetElapsedTimeMillis() - lastSecond > 1000) {
        ofLogNotice(__func__) << niStreamer.count << " websocket frames sent this second";
        niStreamer.count = 0;
        lastSecond = ofGetElapsedTimeMillis();
    }
    
//    niStreamer.sendTrackingData(tracker);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    depthImage.draw(0, 0);
    
    ofPushView();
	tracker.getOverlayCamera().begin(ofRectangle(0, 0, 640, 480));
	tracker.draw();
	tracker.getOverlayCamera().end();
	ofPopView();
    
    ofDrawBitmapStringHighlight(ofToString((int) ofGetFrameRate()) + " FPS", 20, 30);

}

void ofApp::initDevices() {
    
    if (!bDevicesInited) {
        
        if (device.setup()) {
            bDevicesInited = true;
        }
        
    } else {
        ofLogWarning("ofApp::initDevices") << "Devices already inited";
    }
}

void ofApp::exit() {
    
    if (bTrackingEventsRegistered) {
        ofRemoveListener(tracker.newUser, this, &ofApp::onTrackerNewUser);
        ofRemoveListener(tracker.lostUser, this, &ofApp::onTrackerLostUser);
    }
    
    niStreamer.unregisterEvents(this);
    
    tracker.exit();
    device.exit();
}

void ofApp::onTrackerNewUser(ofxNiTE2::User::Ref & user) {
    ofLogNotice() << "Tracking new user: #" << user->getId();
    niStreamer.sendNewUser(user, tracker);
}

void ofApp::onTrackerLostUser(ofxNiTE2::User::Ref & user) {
    ofLogNotice() << "Tracking lost user: #" << user->getId();
    niStreamer.sendLostUser(user, tracker);
}

void ofApp::onGetDevices(ofx::JSONRPC::MethodArgs& args) {
    
    ofxNI2::init();
	
	openni::Array<openni::DeviceInfo> deviceList;
	openni::OpenNI::enumerateDevices(&deviceList);
	
    ofxJSONElement json(Json::arrayValue);
    
	for (int i = 0; i < deviceList.getSize(); ++i)
	{
        ofxJSONElement device;
        device["name"] = deviceList[i].getName();
        device["vendor"] = deviceList[i].getVendor();
        device["uri"] = deviceList[i].getUri();
        json.append(device);
	}
	
	args.result = json;

}

void ofApp::onSetSkeletonSmoothing(float& smoothing) {
    tracker.setSkeletonSmoothingFactor(smoothing);
}

void ofApp::onGetSkeletonSmoothing(ofx::JSONRPC::MethodArgs& args) {
    args.result = tracker.getSkeletonSmoothingFactor();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        niStreamer.saveDepthData(tracker.getPixelsRef());
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
