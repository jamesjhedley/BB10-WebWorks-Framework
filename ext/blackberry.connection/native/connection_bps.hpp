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

#ifndef CONNECTION_BPS_H_
#define CONNECTION_BPS_H_

#include "bps_netstatus.hpp"

class ConnectionInterface;

namespace webworks {

enum ConnectionTypes {
    UNKNOWN = 0,
    ETHERNET = 1,
    WIFI = 2,
    BLUETOOTH_DUN = 3,
    USB = 4,
    VPN = 5,
    BB = 6,
    CELLULAR = 7,
    NONE = 8
};

class ConnectionBPS {
public:
    explicit ConnectionBPS(ConnectionInterface *parent = NULL);
    ~ConnectionBPS();
    ConnectionTypes GetConnectionType();
    int WaitForEvents();
    static void EnableEvents();
    static void DisableEvents();
private:
    ConnectionInterface *m_parent;
    static bool m_eventsEnabled;
    BPSNetstatus *m_bps;
};

} // namespace webworks

#endif /* CONNECTION_BPS_H_ */
