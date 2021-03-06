/*
 * Copyright 2010-2012 Research In Motion Limited.
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

describe("controllerWebView", function () {
    var controllerWebView = require('./../../../lib/controllerWebView'),
        mockedController,
        mockedInvocation,
        mockedApplication;

    beforeEach(function () {
        mockedController = {
            enableWebInspector: null,
            enableCrossSiteXHR: null,
            visible: null,
            active: null,
            setGeometry: jasmine.createSpy(),
            publishRemoteFunction: jasmine.createSpy()
        };
        mockedInvocation = {
            queryTargets: function (request, callback) {
                callback("error", "results");
            }
        };
        mockedApplication = {
            invocation: mockedInvocation
        };
        GLOBAL.window = {
            qnx: {
                webplatform: {
                    getController: function () {
                        return mockedController;
                    },
                    getApplication: function () {
                        return mockedApplication;
                    }
                }
            }
        };
        GLOBAL.screen = {
            width : 1024,
            height: 768
        };
    });

    describe("init", function () {
        it("sets up the controllerWebview", function () {
            controllerWebView.init({debugEnabled: true});
            expect(mockedController.enableWebInspector).toEqual(true);
            expect(mockedController.enableCrossSiteXHR).toEqual(true);
            expect(mockedController.visible).toEqual(false);
            expect(mockedController.active).toEqual(false);
            expect(mockedController.setGeometry).toHaveBeenCalledWith(0, 0, screen.width, screen.height);
        });
    });
});
