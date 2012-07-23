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

#ifndef BBM_BPS_H_
#define BBM_BPS_H_

#include <string>

class BBM;

namespace webworks {

struct BBMContact {
    std::string displayName;
    std::string status;
    std::string statusMessage;
    std::string personalMessage;
    std::string ppid;
    std::string handle;
    std::string appVersion;
    std::string bbmsdkVersion;
    std::string countryCode;
    std::string countryFlag;
    std::string timezone;
};

class BBMBPS {
public:
    explicit BBMBPS(BBM *parent = NULL);
    ~BBMBPS();
    void SetActiveChannel(int channel);
    int InitializeEvents();
    int WaitForEvents();
    static void SendEndEvent();
    static int GetActiveChannel();
    // BBM related functions
    void Register(const std::string& uuid);
    void GetProfile(BBMContact *bbmContact);
    void SetStatus(int status, const std::string& personalMessage);
    void SetPersonalMessage(const std::string& personalMessage);
private:
    BBM *m_pParent;
    void processAccessCode(int code);
    void processPresenceUpdate(int code);
    static int m_eventChannel;
    static int m_endEventDomain;
};

} // namespace webworks

#endif // BBM_BPS_H_
