BB.DepthClient = function(config, callback) {

    var self = this;

    this.host = config.host || '127.0.0.1';
    this.port = config.port || 8197;
    this.device = (typeof config.device === "Number") ? config.device : 0; 
    this.modules = (config.modules && config.modules instanceof Array) ? config.modules : [];

    this.supportedEvents = [
        'connect',
        'disconnect',
        'error',
        'websocketError',
        'websocketOpen',
        'websocketClose',
        'websocketMessage',
        'motion',
        'contour',
        'nearestPixel',
        'colorFrame',
        'infraredFrame',
        'depthFrame',
        'trackingFrame',
        'userNew',
        'userLost',
        'userCalibrating',
        'userTracked',
        'userCalibrationError',
        'userVisible',
        'userNotVisible',
        'jointTorso',
        'jointNeck',
        'jointHead',
        'jointLeftShoulder',
        'jointLeftElbow',
        'jointLeftHand',
        'jointRightShoulder',
        'jointRightElbow',
        'jointRightHand',
        'jointLeftHip',
        'jointLeftKnee',
        'jointLeftFoot',
        'jointRightHip',
        'jointRightKnee',
        'jointRightFoot'
    ];

    this._users = {};
    this._numUsers = 0;
    this._eventCallbacks = {};

    for (var i = 0; i < this.supportedEvents.length; i++) {
        this._eventCallbacks["_" + this.supportedEvents[i] + "_" ] = [];
    }

    this._jsonRPCClient = new $.JsonRpcClient({
        ajaxUrl: getDefaultPostURL(),
        socketUrl: 'ws://' + this.host + ':' + this.port, // get a websocket for the localhost
        onmessage: self._onWebSocketMessage(this),
        onopen: self._onWebSocketOpen(this),
        onclose: self._onWebSocketClose(this),
        onerror: self._onWebSocketError(this)
    });

    try {
        this._jsonRPCClient.notify('hello');
    } catch(err) {
        // console.log(err);
    }

}

BB.DepthClient.prototype.on = function(name, callback) {
    
    if (this._eventCallbacks.hasOwnProperty("_" + name + "_") && typeof callback === 'function') {
        this._eventCallbacks["_" + name + "_"].push(callback); 
    }
}

BB.DepthClient.prototype.getDevices = function(callback) {
     this._jsonRPCClient.call("getDevices", 
                              "", 
                              function(devices) {
                                  callback(null, devices);
                              },
                              function(err) {
                                  callback(err, null)
                              });
}

BB.DepthClient.prototype.setSkeletonSmoothing = function(value, callback) {
    this._jsonRPCClient.call("setSkeletonSmoothingFactor", 
                              value, 
                              function() {
                                  callback(null);
                              },
                              function(err) {
                                  callback(err)
                              });
}

BB.DepthClient.prototype.getSkeletonSmoothing = function(callback) {
    this._jsonRPCClient.call("getSkeletonSmoothingFactor", 
                              "", 
                              function(smoothing) {
                                  callback(null, smoothing);
                              },
                              function(err) {
                                  callback(err, null)
                              });
}

BB.DepthClient.prototype._onWebSocketOpen = function(self) {
    
    return function(evt) {
        self._fireEvents('websocketOpen', evt)
    }
}
var count = 0;
setInterval(function(){
                console.log(count + " FPS");
                count = 0;
            }, 1000);
BB.DepthClient.prototype._onWebSocketMessage = function(self) {
    
    return function(evt) {
       
        try {
            var data = JSON.parse(evt.data)
        } catch (err) { /*probably binary*/ }
        
        if (typeof data !== 'undefined') {

            if (data.module === "NIStreamer") {
                if (data.method === 'trackingFrame') {
                    self._processTrackingFrame(data.params);
                    self._fireEvents('trackingFrame', data.params)
                } else if (data.method === 'userNew') {
                    self._users[data.params.id] = data.params;
                    self._numUsers++;
                    // set not visible to allow "userVisible" event to be
                    // triggered from inside _processTrackingFrame()
                    self._users[data.params.id].state.visible = false;
                    self._fireEvents('userNew', data.params);
                } else if (data.method === 'userLost') {
                    delete self._users[data.params.id];
                    self._numUsers--;
                    self._fireEvents('userLost', data.params);
                }
            }     
        } else { // data failed json parse, probably binary
            
            var arrayBuffer;
            
            var fileReader = new FileReader();
            fileReader.onload = function() {
                arrayBuffer = new Uint16Array(this.result);
                // var max = 0;
                // for (var i = 0; i < arrayBuffer.length; i++) {
                //     max = Math.max(max, arrayBuffer[i]);
                // }
                // console.log(max);
                count++;
            };


            fileReader.readAsArrayBuffer(evt.data);
        }
        
    }
}

BB.DepthClient.prototype._onWebSocketClose = function(self) {
    
    return function(evt) {
        self._fireEvents('websocketClose', evt)
    }
}

BB.DepthClient.prototype._onWebSocketError = function(self) {
    
    return function(evt) {
        // If there is no registered websocketError function throw one
        if (!self._fireEvents('websocketError', evt)) {
            throw new Error("BB._onWebSocketError: Websocket connection error."
                + " Make sure the DepthServer is running and has websockets listening at ws://" 
                + self.host + ":" + self.port);
        }
    }
    
}

// returns true if at least one callback had been registered and was fired
BB.DepthClient.prototype._fireEvents = function(name) {

    var oneOrMoreEventsFired = false;

    if (this._eventCallbacks["_" + name + "_"].length > 0) {
        for (var i = 0; i < this._eventCallbacks["_" + name + "_"].length; i++) {
            if (typeof this._eventCallbacks["_" + name + "_"][i] === 'function') {
                var args = Array.prototype.slice.call(arguments);
                args.shift(); // remove the name parameter
                (this._eventCallbacks["_" + name + "_"][i]).apply(this, args);
                oneOrMoreEventsFired = true;
            }
        }
    }

    return oneOrMoreEventsFired;

}

BB.DepthClient.prototype._processTrackingFrame = function(trackingFrame) {
    
    if (typeof trackingFrame.users === 'object') {
        
        for (var i = 0; i < trackingFrame.users.length; i++) {
            
            var user = trackingFrame.users[i];

            if (typeof this._users[user.id] !== 'undefined') {

                 // visible state
                if (user.state.visible !== this._users[user.id].state.visible) {
                    if (user.state.visible) this._fireEvents('userVisible', user);
                    else this._fireEvents('userNotVisible', user);
                }

                // skeleton state
                if (user.skeleton.state !== this._users[user.id].skeleton.state) {
                     switch (user.skeleton.state) {
                        case 'CALIBRATING': this._fireEvents('userCalibrating', user); break;
                        case 'TRACKED': this._fireEvents('userTracked', user); break;
                        case 'ERROR_NOT_IN_POSE':
                        case 'ERROR_HANDS':
                        case 'ERROR_HEAD':
                        case 'ERROR_LEGS':
                        case 'ERROR_TORSO':
                            this._fireEvents('userCalibrationError', user); break;
                    }
                }

                if (user.skeleton.joints) {
                    this._fireEvents('jointHead', user.skeleton.joints.head, user);
                    this._fireEvents('jointNeck', user.skeleton.joints.neck, user);
                    this._fireEvents('jointLeftShoulder', user.skeleton.joints.leftShoulder, user);
                    this._fireEvents('jointRightShoulder', user.skeleton.joints.rightShoulder, user);
                    this._fireEvents('jointLeftElbow', user.skeleton.joints.leftElbow, user);
                    this._fireEvents('jointRightElbow', user.skeleton.joints.rightElbow, user);
                    this._fireEvents('jointLeftHand', user.skeleton.joints.leftHand, user);
                    this._fireEvents('jointRightHand', user.skeleton.joints.leftHand, user);
                    this._fireEvents('jointTorso', user.skeleton.joints.torso, user);
                    this._fireEvents('jointLeftHip', user.skeleton.joints.leftHip, user);
                    this._fireEvents('jointRightHip', user.skeleton.joints.rightHip, user);
                    this._fireEvents('jointLeftKnee', user.skeleton.joints.leftKnee, user);
                    this._fireEvents('jointRightKnee', user.skeleton.joints.rightKnee, user);
                    this._fireEvents('jointLeftFoot', user.skeleton.joints.leftFoot, user);
                    this._fireEvents('jointRightFoot', user.skeleton.joints.rightFoot, user);
                }
  
            } else {
                console.warn('BB._processTrackingFrame: User #' + user.id + ' was not found in the _users object');
            }

            this._users[user.id] = user;
        }
    }
}

//--------------------------------------------------------------------------
// make sure that the appropriate websocket scheme is used
// i.e. ws:// or wss:// for secure connections
// function getDefaultWebSocketURL() 
// {
//     var scheme;
//     var url = document.URL;
//     if(url.substring(0, 5) == "https")
//     {
//         scheme = "wss://";
//         url = url.substr(8);
//     }
//     else
//     {
//         scheme = "ws://";
        
//         if (url.substring(0, 4) == "http")
//         {
//             url = url.substr(7);
//         }
//     }

//     url = url.split('/');

//     return scheme + url[0];
// }

//--------------------------------------------------------------------------
function getDefaultPostURL() 
{
    return document.URL + "post";
}

