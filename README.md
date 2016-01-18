# OpenNIWebSocketDaemon

Authored by @brannondorsey and others w/ support from [Branger_Briz](http://brangerbriz.com/).

__NOTE: The code in this repository should be considered experimental and unstable. It may not yet work how you expect, or at all__.

The purpose of this daemon is to provide high-level depth sensor and skeleton tracking data via WebSockets so that it may be available for use in the browser or similar style environment. 

The daemon app handles:

- Low-level USB communication to a variety of depth sensors (tested only with the Kinect v1 as of this writing) via custom drivers (libfreenect) and OpenNI2
- Skeleton tracking via NiTE2
- Custom CV-related algorithms, most notably blob-tracking and contour finding (not yet implemented) via ofxCV
- WebSocket communication via ofxHTTP

## Dependencies and Frameworks

Pretty dependency heavy, but everything should work correctly once you've got things installed. Tested to work on OSX 10.10 w/ OpenFrameworks v0.9.0. Should work well with Linux as well, however you will need to build your own linux versions of OpenNI2 and NiTE2 for ofxNI2 (I've also seen a linux-compatible fork of ofxNI2 floating around but can't find it at the moment. This approach is untested).

To get up and running first download and install the appropriate [OpenFrameworks v0.9.0](http://openframeworks.cc/versions/v0.9.0/) for your OS. 

If you aren't familiar with OF, it uses [Addons](ofxaddons.com) to package custom libraries and wrappers in a way that plays nice with the OF build process. Any addon (almost always prefixed with "fox") must live in the `addons/` folder in the extracted OpenFrameworks download.

The complete list of addons OpenNIWebSocketDaemon is as follows:

- [ofxNI2](https://github.com/brannondorsey/ofxNI2) (@brannondorsey custom fork)
- [ofxJSONRPC](https://github.com/bakercp/ofxJSONRPC) (Note: the rest of the addon dependencies are ofxJSONRPC's dependencies)
	- [ofxSSLManager](https://github.com/bakercp/ofxSSLManager)
	- [ofxTaskQueue](https://github.com/bakercp/ofxTaskQueue)
	- [ofxNetworkUtils](https://github.com/bakercp/ofxNetworkUtils)
	- [ofxIO](https://github.com/bakercp/ofxIO)
	- [ofxJSON](https://github.com/bakercp/ofxJSON)
	- [ofxMediaType](https://github.com/bakercp/ofxMediaType)
	
In the past, I've also needed to add the following "run script" to copy over the necessary compiled NiTE2 and OpenNI2 static library files to a place where the OSX `OpenNIWebSocketDaemon.app` can find them using:

```
cp -R ../../../addons/ofxNI2/libs/OpenNI2/lib/osx/ "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/";
cp -R ../../../addons/ofxNI2/libs/NiTE2/lib/osx/ "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/";

```

If you are using Xcode, and experience linking problem, try adding the above to end of __Build Phases > Run Script__.

## Daemon Usage

Once OpenNIWebSocketDaemon is compiling and linking correctly, and you have a Kinect v1 plugged into your machine via USB, you should see the following console output from running the daemon:

```
COME BACK
```

If everything worked correctly a depth sensor and skeleton tracker should have both been initialized and the daemons data should be accessible via WebSockets at `ws://localhost:8197`. You can test this by visiting `http://localhost:8197` in your browser and opening the JavaScript console (Note: this page is void of content, however should be logging helpful information to the console). This webpage is being served by the daemon itself and is located in `bin/data/DocumentRoot`.

Read ahead to learn more about how to access the data provided from the OpenNIWebSocketDaemon via WebSockets as well as the current limitations in doing so.

## Client Side Usage

### Files

A few JavaScript libraries are required in order to facilitate communication w/ the daemon and are already included in `bin/data/DocumentRoot`. They are:

- `BB.DepthClient.js`: Contains the client-side object that abstracts the communication w/ the daemon and provides high-level access to its skeleton and raw depth data.
- `BB.js`: Needed by `BB.DepthClient.js`.
- `ofxHTTP.js`: Contains `ofxHTTPBasicWebSocketClient`, which is used by `ofJSONRPC` and `ofxHTTP` and instantiated by `BB.DepthClient.js`.
- `jquery.jsonrpcclient.js` (needed by `ofxHTTP.js`)
- `jquery.min.js`: Needed by `jquery.jsonrpcclient.js`
- `jquery.json.js`: Needed by `jquery.jsonrpcclient.js`

### Usage

Inspect `index.html` for an example usage of the `BB.DepthClient` "class". The basic premise is that it is instantiated like so:

```
var depthClient = new BB.DepthClient({
 	host: '127.0.0.1',
 	port: 8197
 });
```

And then most all of it's functionality is accessible by subscribing/registering to events using the `BB.DepthClient.prototype.on(...)` method:

```
depthClient.on('userNew', function(user) {
 	console.log('New user #' + user.id)
 });
```

Here is a list of events that the `BB.DepthClient` currently supports, or intends to support:

- connect
- disconnect
- error
- websocketError
- websocketOpen
- websocketClose
- websocketMessage
- motion (Not implemented)
- contour (Not implemented)
- nearestPixel (Not implemented)
- colorFrame (Not implemented)
- infraredFrame (Not implemented)
- depthFrame
- trackingFrame
- userNew
- userLost
- userCalibrating
- userTracked
- userCalibrationError
- userVisible
- userNotVisible
- jointTorso
- jointNeck
- jointHead
- jointLeftShoulder
- jointLeftElbow
- jointLeftHand
- jointRightShoulder
- jointRightElbow
- jointRightHand
- jointLeftHip
- jointLeftKnee
- jointLeftFoot
- jointRightHip
- jointRightKnee
- jointRightFoot
 
Perhaps the most useful events are "trackingFrame" and "depthFrame". "trackingFrame" provides access to all skeleton data and fires at ~30 fps (the framerate specified to OpenNI by the daemon). Registering to it looks like:

```
depthClient.on('trackingFrame', function(trackingData){
	// use trackingData here
});
 ```

### Skeleton Tracking Data

The structure of `trackingData` follows this scheme:

```

{
    "id": 1,
    "centerOfMass": { "x": 1, "y": 1, "z": 1 },
    "centerOfBone": { "x": 1, "y": 1, "z": 1 },
    "boundingBox": {
        "min": { "x": 1, "y": 1, "z": 1 },
        "max": { "x": 1, "y": 1, "z": 1 }
    },
    "pose": {
        "CROSSED_HANDS": {
            "held": true,
            "entered": false,
            "exited": false
        },
        "PSI": {}
    },
    "state": {
        "new": false,
        "visible": true,
        "lost": false
    },
    "skeleton": {
        "state": "TRACKED", // NONE, CALIBRATING, TRACKED, ERROR_NOT_IN_POSE, ERROR_HANDS, ERROR_HEAD, ERROR_LEGS, ERROR_TORSO
        "joints": { // can be null if state is NONE, CALIBRATING, OR ERROR_*
            "head": {
                "position": {
                	"real": { "x": 1, "y": 1, "z": 1}, // REAL IS USED FOR 3D SCENES
                	"projective": { "x": 1, "y": 1, "z": 1}, // PROJECTIVE IS USED FOR 2D SCENES
                },
                "positionConfidence": 0.7,
                "orientation": { "x": 1, "y": 1, "z": 1, "w": 1},
                "orientationConfidence": 0.5
            },
            "neck": {}, // THE REST OF THE JOINT OBJECTS FOLLOW THE HEAD'S SCHEME
            "leftShoulder": {},
            "rightShoulder": {},
            "leftElbow": {},
            "rightElbow": {},
            "leftHand": {},
            "rightHand": {},
            "torso": {},
            "leftHip": {},
            "rightHip": {},
            "leftKnee": {},
            "rightKnee": {},
            "leftFoot": {},
            "rightFoot": {}
        }
    }
}
```

### Raw Depth Data

The real shortcomings of the OpenNIWebSocketDaemon become obvious when accessing the raw depth data via:

```
depthClient.on('trackingFrame', function(depthData){
	// use depthData here
});
```

__Note__: Currently this callback is __increadibly laggy__, firing between ~4-9 FPS in my tests. This renders this feature almost entirely unusable for the time being. Read the [Improving Depth Frame FPS](#improving-depth-frame-fps) section for more info on this subject and attempts that have already been made to improve it.

`depthData` is a `UInt16Array` holding 307,200 (640x480) elements if using the Kinect v1.
The value of each element in the array represents distance from the camera expressed in millimeters. 

__Note__: Only the frequency that this event fires has been tested at the time of this writing, not the actual `depthData` value itself. Do not yet rely on the above description as an accurate representation of `depthData`. 

## Improving Depth Frame FPS

The absolutely abysmal frame rate of the `depthData` event is a serious problem that needs to be fixed. The nature of this problem comes from trying to send over 18MB per second or 147Mbps (640 width * 480 height * 2 bytes per "pixel" * 30 fps) via a WebSocket connection. Initially ofxJSONRPC/ofxHTTP did not support `permessage-deflate` WebSocket compression, however I reached out to @bakercp and he has since added support for it. While this solution does successfully compress the data ~85-97%, it takes between 83-110 milliseconds for the daemon to compress each frame, causing a maximum of ~10-12 FPS without figuring in transfer time. The perfect solution to this problem alludes me. Note that whatever solution is devised needs to work only on a loopback (localhost) connection, or at most a wired/wireless LAN network and is not expected to be pushed through the bandwidth limits imposed by the internet.

Possible Solutions:
	
- Resize the depth data "images" to half size or smaller before sending to the client. This is a bummer solution because you can't do much with a 360x240 depth image.
- Investigate a faster compression method.
- Implement a solution similar to @arturoc's [ofxDepthStreamCompression](https://github.com/arturoc/ofxDepthStreamCompression) which sends binary diffs rather than raw depth data. The algorithm to "reconstruct" the depth data on the client side would have to be implemented in JavaScript and may be non-trivial.
- Become a better programmer.

