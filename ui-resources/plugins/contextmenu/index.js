/*
 * Copyright (C) Research In Motion Limited 2012. All rights reserved.
 */

var contextmenu,
    menuVisible,
    menuPeeked;

function init() {
    var menu = document.getElementById('contextMenu');
    menu.addEventListener('webkitTransitionEnd', contextmenu.transitionEnd.bind(contextmenu));
}

contextmenu = {
    init: init,
    setMenuOptions: function (options) {
        var menu = document.getElementById("contextMenuContent"),
            i,
            header,
            menuItem,
            callback,
            menuImage;

        while (menu.childNodes.length >= 1) {
            menu.removeChild(menu.firstChild);
        }
        contextmenu.setHeadText('');
        contextmenu.setSubheadText('');

        for (i = 0; i < options.length; i++) {
            if (options[i].headText || options[i].subheadText) {
                header = document.getElementById('contextMenuHeader');
                header.className = 'contextMenuHeader';
                if (options[i].headText) {
                    contextmenu.setHeadText(options[i].headText);
                }
                if (options[i].subheadText) {
                    contextmenu.setSubheadText(options[i].subheadText);
                }
                continue;
            }
            menuItem = document.createElement('div');
            callback = options[i].function;
            menuImage = document.createElement('img');
            menuImage.src = options[i].imageUrl ? options[i].imageUrl : 'assets/generic_81_81_placeholder.png';
            menuItem.appendChild(menuImage);
            menuItem.appendChild(document.createTextNode(options[i].name));
            menuItem.setAttribute("class", "menuItem");
            menuItem.ontouchend = callback.bind(this, menuItem);
            menuItem.addEventListener('mousedown', contextmenu.handleMouseDown, false);
            // FIXME: need to make this work
            //menuItem.onmousedown = function () { menuItem.attr("class", "menuItem click"); };
            menu.appendChild(menuItem);
        }
    },

    handleMouseDown: function (evt) {
        evt.preventDefault();
    },

    setHeadText: function (text) {
        var headText = document.getElementById('contextMenuHeadText');
        headText.innerText = text;
    },

    setSubheadText: function (text) {
        var subheadText = document.getElementById('contextMenuSubheadText');
        subheadText.innerText = text;
    },
    
    showContextMenu: function (evt) {
        if (menuVisible) {
            return;
        }
        var menu = document.getElementById('contextMenu');
        menu.className = 'showMenu';
        menuVisible = true;
        if (menuPeeked) {
            evt.cancelBubble = true;
            menuPeeked = false;
        }
    },

    isMenuVisible: function () {
        return menuVisible || menuPeeked;
    },

    hideContextMenu: function () {
        if (!menuVisible && !menuPeeked) {
            return;
        }
        var menu = document.getElementById('contextMenu'),
            handle = document.getElementById('contextMenuHandle');
        menu.removeEventListener('touchend', contextmenu.hideContextMenu, false);
        handle.removeEventListener('touchend', contextmenu.showContextMenu, false);
        menuVisible = false;
        menuPeeked = false;
        menu.className = 'hideMenu';
        // TODO: 2 for client webview
        qnx.callExtensionMethod("webview.notifyContextMenuCancelled", 2);
        
        /*tabs.getSelected(0, function (tab) {
            if (tab) {
                qnx.callExtensionMethod("webview.notifyContextMenuCancelled", tab.id);
            }
        });*/
        // Reset sensitivity
        // TODO: 3 for ui webview
        qnx.callExtensionMethod("webview.setSensitivity", 3, "SensitivityTest");
    },

    peekContextMenu: function (show, zIndex) {
        if (menuVisible || menuPeeked) {
            return;
        }
        //TODO: 3 for ui webview
        qnx.callExtensionMethod("webview.setSensitivity", 3, "SensitivityNoFocus");
        var menu = document.getElementById('contextMenu'),
            handle = document.getElementById('contextMenuHandle');
        handle.className = 'showContextMenuHandle';
        menuVisible = false;
        menuPeeked = true;
        menu.className = 'peekContextMenu';
    },

    transitionEnd: function () {
        var menu = document.getElementById('contextMenu'),
            handle = document.getElementById('contextMenuHandle'),
            header;
        if (menuVisible) {
            menu.addEventListener('touchend', contextmenu.hideContextMenu, false);
            handle.removeEventListener('touchend', contextmenu.showContextMenu, false);
        } else if (menuPeeked) {
            handle.addEventListener('touchend', contextmenu.showContextMenu, false);
            menu.addEventListener('touchend', contextmenu.hideContextMenu, false);
        } else {
            header = document.getElementById('contextMenuHeader');
            header.className = '';
            contextmenu.setHeadText('');
            contextmenu.setSubheadText('');
        }
    },

      // TODO: i18n
    generateContextMenuItems: function (value) {
        var items = [], 
            i,
            // TODO: just for demo, should be replaced by a real function
            foo = function () {
                alert("Menu");
            };

        for (i = 0; i < value.length; i++) {
            switch (value[i]) {
            case 'ClearField':
            case 'SendLink':
            case 'SendImageLink':
            case 'FullMenu':
            case 'Delete':
                break;
            case 'Cancel':
                items.push({'name': 'Cancel', 'function': foo, 'imageUrl': 'assets/Browser_Cancel_Selection.png'});
                break;
            case 'Cut':
                items.push({'name': 'Cut', 'function': foo, 'imageUrl': 'assets/Browser_Cut.png'});
                break;
            case 'Copy':
                items.push({'name': 'Copy', 'function': foo, 'imageUrl': 'assets/Browser_Copy.png'});
                break;
            case 'Paste':
                items.push({'name': 'Paste', 'function': foo, 'imageUrl': 'assets/crosscutmenu_paste.png'});
                break;
            case 'Select':
                items.push({'name': 'Select', 'function': foo, 'imageUrl': 'assets/crosscutmenu_paste.png'});
                break;
            case 'OpenLinkInNewTab':
                items.push({'name': 'Open in New Tab', 'function': foo, 'imageUrl': 'assets/Browser_OpenLinkInNewTab.png'});
                break;
            case 'AddLinkToBookmarks':
                items.push({'name': 'Bookmark Link', 'function': foo, 'imageUrl': 'assets/Browser_BookmarkLink.png'});
                break;
            case 'CopyLink':
                items.push({'name': 'Copy Link', 'function': foo, 'imageUrl': 'assets/Browser_CopyLink.png'});
                break;
            case 'OpenLink':
                items.push({'name': 'Open', 'function': foo, 'imageUrl': 'assets/Browser_OpenLink.png'});
                break;
            case 'SaveLinkAs':
                items.push({'name': 'Save Link as', 'function': foo, 'imageUrl': 'assets/Browser_SaveLink.png'});
                break;
            case 'SaveImage':
                items.push({'name': 'Save Image', 'function': foo, 'imageUrl': 'assets/Browser_SaveImage.png'});
                break;
            case 'CopyImageLink':
                items.push({'name': 'Copy Image Link', 'function': foo, 'imageUrl': 'assets/Browser_CopyImageLink.png'});
                break;
            case 'ViewImage':
                items.push({'name': 'View Image', 'function': foo, 'imageUrl': 'assets/Browser_ViewImage.png'});
                break;
            case 'Search':
                items.push({'name': 'Search', 'function': foo, 'imageUrl': 'assets/Browser_Search.png'});
                break;
            case 'ShareLink':
                items.push({'name': 'Share Link', 'function': foo, 'imageUrl': 'assets/Browser_ShareLink.png'});
                break;
            case 'ShareImage':
                items.push({'name': 'Share Image', 'function': contextmenu.shareImage, 'imageUrl': 'assets/Browser_ShareImage.png'});
                break;
            case 'InspectElement':
                items.push({'name': 'Inspect Element', 'function': foo, 'imageUrl': 'assets/generic_81_81_placeholder.png'});
                break;
            }
        }

        /*
        if (currentContext && currentContext.url && currentContext.text) {
            items.push({'headText': currentContext.text, 'subheadText': currentContext.url});
        }
        */
        return items;
    },
    
    share : function (type, errorMessage, dataCallback) {
        var invocation = window.qnx.webplatform.getApplication().invocation,
            request = {
                action: 'bb.action.SHARE',
                type: type,
                target_type: invocation.TARGET_TYPE_ALL,
                action_type: invocation.ACTION_TYPE_MENU
            };

        invocation.queryTargets(request, function (error, results) {
            if (error || results.length === 0) {
                //TODO: nice dialog
                alert(errorMessage);
            } else {
                console.log(results);
               // TODO: get an invocation list 
                
                /*var invocationList = screenManager.loadScreen('invocationlist');
                dataCallback(request);
                invocationList.setContext({request: request, results: results[0]});
                screenManager.pushScreen(invocationList); */
            
            }
        });
    },

    shareImage : function () {
        contextmenu.share('image/*', 'No image sharing applications installed', function (request) {
            //TODO: implement PropertyCurrentContextEvent
            //request.uri = currentContext.src;
        });
    }

};

//event.on('browser.plugins.init', init);

module.exports = contextmenu;