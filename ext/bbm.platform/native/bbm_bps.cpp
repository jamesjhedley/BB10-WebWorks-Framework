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

#include "bbm_bps.hpp"
#include "bbm_js.hpp"
#include <bbmsp/bbmsp.h>
#include <bbmsp/bbmsp_events.h>
#include <bbmsp/bbmsp_context.h>
#include <bbmsp/bbmsp_userprofile.h>
#include <bbmsp/bbmsp_util.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>

namespace webworks {

int BBMBPS::m_eventChannel = -1;
int BBMBPS::m_endEventDomain = -1;

BBMBPS::BBMBPS(BBM *parent) : m_pParent(parent)
{
    bps_initialize();
}

BBMBPS::~BBMBPS()
{
    bps_shutdown();
}

void BBMBPS::SetActiveChannel(int channel)
{
    bps_channel_set_active(channel);
}

int BBMBPS::InitializeEvents()
{
    m_eventChannel = bps_channel_get_active();
    m_endEventDomain = bps_register_domain();

    return (m_endEventDomain >= 0) ? 0 : 1;
}

void BBMBPS::processAccessCode(int code)
{
    std::string accessString = "onaccesschanged ";

    switch (code) {
        case BBMSP_ACCESS_ALLOWED:
            accessString.append("allowed");
            break;
        case BBMSP_ACCESS_UNKNOWN:
            accessString.append("unknown");
            break;
        case BBMSP_ACCESS_UNREGISTERED:
            accessString.append("unregistered");
            break;
        case BBMSP_ACCESS_PENDING:
            accessString.append("pending");
            break;
        case BBMSP_ACCESS_BLOCKED_BY_USER:
            accessString.append("user");
            break;
        case BBMSP_ACCESS_BLOCKED_BY_RIM:
            accessString.append("rim");
            break;
        case BBMSP_ACCESS_NO_DATA_COVERAGE:
            accessString.append("nodata");
            break;
        case BBMSP_ACCESS_UNEXPECTED_ERROR:
            accessString.append("unexpectederror");
            break;
        case BBMSP_ACCESS_INVALID_UUID:
            accessString.append("invaliduuid");
            break;
        case BBMSP_ACCESS_TEMPORARY_ERROR:
            accessString.append("temperror");
            break;
        case BBMSP_ACCESS_MAX_DOWNLOADS_REACHED:
            accessString.append("limitreached");
            break;
        case BBMSP_ACCESS_EXPIRED:
            accessString.append("expired");
            break;
        case BBMSP_ACCESS_CANCELLED_BY_USER:
            accessString.append("cancelled");
            break;
        default:
            accessString.append("unknown");
            break;
    }
        fprintf(stderr, "code received: %d\n", code);
        m_pParent->NotifyEvent(accessString);
}

void BBMBPS::processPresenceUpdate(int code)
{
    std::string presenceString = "onupdate";

    switch (code) {
        case BBMSP_DISPLAY_NAME:
            presenceString.append("displayname");
            break;
        case BBMSP_DISPLAY_PICTURE:
            presenceString.append("displaypicture");
            break;
        case BBMSP_PERSONAL_MESSAGE:
            presenceString.append("personalmessage");
            break;
        case BBMSP_STATUS:
            presenceString.append("status");
            break;
        case BBMSP_INSTALL_APP:
            presenceString.append("install");
            break;
        case BBMSP_UNINSTALL_APP:
            presenceString.append("uninstall");
            break;
        case BBMSP_INVITATION_RECEIVED:
            presenceString.append("invitation");
            break;
        default:
            presenceString.append("unknown");
            break;
    }
}

int BBMBPS::WaitForEvents()
{
    int status = bbmsp_request_events(0);

    bps_event_t *event = NULL;
    bbmsp_event_t *bbmEvent;
    bbmsp_profile_t *bbmProfile;

    for (;;) {
        bps_get_event(&event, -1);

        if (event) {
            int event_domain = bps_event_get_domain(event);

            if (event_domain == bbmsp_get_domain()) {
                int eventCategory = 0;
                int eventType = 0;
                int code = 0;
                int profileUpdateType = 0;

                bbmsp_event_get(event, &bbmEvent);

                if (bbmsp_event_get_category(event, &eventCategory) == BBMSP_SUCCESS) {
                    switch (eventCategory) {
                        case BBMSP_REGISTRATION:
                            if (bbmsp_event_get_type(event, &eventType) == BBMSP_SUCCESS) {
                                switch (eventType) {
                                    case BBMSP_SP_EVENT_ACCESS_CHANGED:
                                        processAccessCode(bbmsp_event_access_changed_get_access_error_code(bbmEvent));
                                        break;
                                }
                            }
                            break;
                        case BBMSP_USER_PROFILE:
                            if (bbmsp_event_get_type(event, &eventType) == BBMSP_SUCCESS) {
                                switch (eventType) {
                                    case BBMSP_SP_EVENT_PROFILE_CHANGED:
                                        bbmsp_event_profile_changed_get_profile(bbmEvent, &bbmProfile);
                                        profileUpdateType = bbmsp_event_profile_changed_get_presence_update_type(bbmEvent);
                                        break;
                                }
                            }
                            break;
                    }
                }
            }
        }
    }
    return (status == BPS_SUCCESS) ? 0 : 1;
}

int BBMBPS::GetActiveChannel()
{
    return m_eventChannel;
}

void BBMBPS::SendEndEvent()
{
    bps_event_t *end_event = NULL;
    bps_event_create(&end_event, m_endEventDomain, 0, NULL, NULL);
    bps_channel_push_event(m_eventChannel, end_event);
}

void BBMBPS::Register(const std::string uuid)
{
    bbmsp_register(uuid.c_str());
}

void BBMBPS::GetProfile(BBMContact *bbmContact)
{
    bbmsp_profile_t *profile;
    bbmsp_profile_create(&profile);
    bbmsp_get_user_profile(profile);

    char buffer[4096];
    bbmsp_profile_get_display_name(profile, buffer, sizeof(buffer));
    bbmContact->displayName = buffer;
    bbmsp_profile_get_status_message(profile, buffer, sizeof(buffer));
    bbmContact->statusMessage = buffer;
    bbmsp_profile_get_personal_message(profile, buffer, sizeof(buffer));
    bbmContact->personalMessage = buffer;
    bbmsp_profile_get_ppid(profile, buffer, sizeof(buffer));
    bbmContact->ppid = buffer;
    bbmsp_profile_get_handle(profile, buffer, sizeof(buffer));
    bbmContact->handle = buffer;
    bbmsp_profile_get_app_version(profile, buffer, sizeof(buffer));
    bbmContact->appVersion = buffer;

    bbmsp_image_t *avatar;
    bbmsp_profile_get_avatar(profile, avatar);
    fprintf(stderr, "image type: %d\n", bbmsp_image_get_type(avatar));

    int value;
    value = bbmsp_profile_get_status(profile);
    if (value == 0) {
        bbmContact->status = "available";
    }
    else if (value == 1) {
        bbmContact->status = "busy";
    }

    value = bbmsp_profile_get_platform_version(profile);
    std::stringstream ss;
    ss << value;
    bbmContact->bbmsdkVersion = ss.str();

    // Get location information
    bbmsp_profile_location_t *location;
    bbmsp_profile_location_create(&location);
    bbmsp_profile_get_profile_location(profile, location);
    bbmsp_profile_location_get_country_code(location, buffer, sizeof(buffer));
    bbmContact->countryCode = buffer;

    void *flagImage;
    bbmsp_profile_location_get_country_flag(location, flagImage);
    bbmContact->countryFlag = buffer;
    bbmsp_profile_location_get_timezone(location, buffer, sizeof(buffer));
    bbmContact->timezone = buffer;
    bbmsp_profile_location_destroy(&location);
    bbmsp_profile_destroy(&profile);
}

void BBMBPS::SetStatus(int status, const std::string statusMessage)
{
    bbmsp_set_user_profile_status(status, statusMessage.c_str(), statusMessage.length());
}

void BBMBPS::SetPersonalMessage(const std::string personalMessage)
{
    bbmsp_set_user_profile_personal_message(personalMessage.c_str(), personalMessage.length());
}

} // namespace webworks