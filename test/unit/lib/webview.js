describe("webview", function () {
    var webview,
        libPath = "./../../../", 
        mockedController,
        mockedWebview,
        mockedApplicationWindow,
        request = require(libPath + "lib/request");

    beforeEach(function () {
        webview = require(libPath + "lib/webview");
        mockedController = {
            enableWebInspector: undefined,
            enableCrossSiteXHR: undefined,
            visible: undefined,
            active: undefined,
            setGeometry: jasmine.createSpy()            
        };
        mockedWebview = {
            enableCrossSiteXHR: undefined,
            visible: undefined,
            active: undefined,
            zOrder: undefined,
            setGeometry: jasmine.createSpy(),
            onNetworkResourceRequested: undefined
        };
        mockedApplicationWindow = {
            visible: undefined
        };
        GLOBAL.window = {
            qnx: {
                webplatform: {
                    getController: function () {
                        return mockedController;
                    },
                    createWebView: function (createFunction) {
                        //process.nextTick(createFunction);
                        //setTimeout(createFunction,0);
                        runs(createFunction);
                        return mockedWebview;
                    },
                    getApplicationWindow: function () {
                        return mockedApplicationWindow;
                    }
                }
            }
        };
        GLOBAL.screen = {
            width : 1024,
            height: 768
        };
    });

    describe("create", function () {
        it("sets up the controller", function () {
            webview.create();
            expect(mockedController.enableWebInspector).toEqual(false);
            expect(mockedController.enableCrossSiteXHR).toEqual(true);
            expect(mockedController.visible).toEqual(true);
            expect(mockedController.active).toEqual(false);
            expect(mockedController.setGeometry).toHaveBeenCalledWith(0, 0, screen.width, 0);
        }); 
        
        it("passes in config settings", function () {
            webview.create(null, {debugEnabled: true});
            expect(mockedController.enableWebInspector).toEqual(true);
        });
        
        it("sets up the visible webview", function () {
            spyOn(request, "init").andCallThrough();
            webview.create();
            waits(1);
            runs(function () {
                expect(mockedWebview.enableCrossSiteXHR).toEqual(true);
                expect(mockedWebview.visible).toEqual(true);
                expect(mockedWebview.active).toEqual(true);
                expect(mockedWebview.zOrder).toEqual(0);
                expect(mockedWebview.setGeometry).toHaveBeenCalledWith(0, 0, screen.width, screen.height);
                
                expect(mockedApplicationWindow.visible).toEqual(true);

                expect(request.init).toHaveBeenCalledWith({webview: mockedWebview});
                expect(mockedWebview.onNetworkResourceRequested).toEqual(request.init(mockedWebview).networkResourceRequestedHandler);
            });
        });

        it("calls the ready function", function () {
            var chuck = jasmine.createSpy();
            webview.create(chuck);
            waits(1);
            runs(function () {
                expect(chuck).toHaveBeenCalled();
            });
        });

    });
});
