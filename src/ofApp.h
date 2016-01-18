#pragma once

#include "ofMain.h"
#include "ofxNI2.h"
#include "ofxNiTE2.h"
#include "NIStreamer.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void onTrackerNewUser(ofxNiTE2::User::Ref & user);
        void onTrackerLostUser(ofxNiTE2::User::Ref & user);
        void onSetSkeletonSmoothing(float& smoothing);
        void onGetSkeletonSmoothing(ofx::JSONRPC::MethodArgs& args);
        void onGetDevices(ofx::JSONRPC::MethodArgs& args);
    
        void initDevices();
    
        bool bDevicesInited;
        bool bTrackingEventsRegistered;
        bool bSendDepth;
    
        NIStreamer niStreamer;
        ofxNI2::Device device;
        ofxNiTE2::UserTracker tracker;
        ofImage depthImage;
    
        int lastSecond;
};
