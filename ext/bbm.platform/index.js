/*
 * Copyright 2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
function requireLocal(id) {
    return !!require.resolve ? require("../../" + id) : window.require(id);
}

function validateOptions(args) {
    return 0;
}

var bbm = require("./BBMJNEXT").bbm,
    _event = require("../../lib/event"),
    _utils = require("../../lib/utils"),
    _actionMap = {
        onaccesschanged: {
            context: require("./BBMEvents"),
            event: "onaccesschanged",
            trigger: function (allowed, reason) {
                _event.trigger("onaccesschanged", allowed, reason);
            }
        }
    };

module.exports = {
    registerEvents: function (success, fail, args, env) {
        try {
            var _eventExt = _utils.loadExtensionModule("event", "index");
            _eventExt.registerEvents(_actionMap);
            success();
        } catch (e) {
            fail(-1, e);
        }
    },
    
    register: function (success, fail, args, env) {
        if (args) {
            args.options = JSON.parse(decodeURIComponent(args.options));

            if (args.options.uuid.length < 32) {
                fail(-1, "options are not valid");
                return;
            }
        
            bbm.register(args.options);
            success();
        }
    },

    self : {
        appVersion : function (success, fail, args, env) {
            success(bbm.getProfile().appVersion);
        },

        bbmsdkVersion : function (success, fail, args, env) {
            success(parseInt(bbm.getProfile().bbmsdkVersion, 10));
        },

        displayName : function (success, fail, args, env) {
            success(bbm.getProfile().displayName);
        },

        handle : function (success, fail, args, env) {
            success(bbm.getProfile().handle);
        },

        personalMessage : function (success, fail, args, env) {
            success(bbm.getProfile().personalMessage);
        },

        ppid : function (success, fail, args, env) {
            success(bbm.getProfile().ppid);
        },

        status : function (success, fail, args, env) {
            success(bbm.getProfile().status);
        },

        statusMessage : function (success, fail, args, env) {
            success(bbm.getProfile().statusMessage);
        },

        setStatus : function (success, fail, args, env) {
            if (args) {
                args.status = JSON.parse(decodeURIComponent(args.status));
                args.statusMessage = JSON.parse(decodeURIComponent(args.statusMessage));
            
                if (args.status !== "available" && args.status !== "busy") {
                    fail(-1, "status is not valid");
                    return;
                }
            }

            bbm.setStatus(args);
            success();
        },

        setPersonalMessage : function (success, fail, args, env) {
            if (args) {
                args.personalMessage = JSON.parse(decodeURIComponent(args.personalMessage));

                if (args.personalMessage.length === 0) {
                    fail(-1, "personal message must not be empty");
                    return;
                }
            }

            bbm.setPersonalMessage(args.personalMessage);
            success();
        }
    }
};

