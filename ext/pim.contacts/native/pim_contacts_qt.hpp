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

#ifndef PIM_CONTACTS_QT_H_
#define PIM_CONTACTS_QT_H_

#include <string>
#include <map>
#include <json/value.h>
#include <bb/pim/contacts/ContactService.hpp>
#include <bb/pim/contacts/ContactConsts.hpp>
#include <bb/pim/contacts/Contact.hpp>
#include <bb/pim/contacts/ContactBuilder.hpp>
#include <bb/pim/contacts/ContactAttribute.hpp>
#include <bb/pim/contacts/ContactAttributeBuilder.hpp>
#include <bb/pim/contacts/ContactPostalAddress.hpp>
#include <bb/pim/contacts/ContactPostalAddressBuilder.hpp>
#include "../common/plugin.h"

namespace webworks {

using namespace bb::pim::contacts;

class PimContactsQt {
public:
    PimContactsQt();
    ~PimContactsQt();
    std::string find(const std::string& optionsJson);
    void createContact(const std::string& attributeJson);
    void deleteContact(const std::string& contactJson);

    static void createAttributeKindMap();
    static void createAttributeSubKindMap();
    static void createKindAttributeMap();
    static void createSubKindAttributeMap();

private:
    QSet<ContactId> singleFieldSearch(const Json::Value& search_field_json, /*const Json::Value& sort_fields_json,*/ bool favorite);
    Json::Value assembleSearchResults(const QSet<ContactId>& results, const Json::Value& contact_fields, const Json::Value& sort_fields_json, int limit);
    void populateContactField(const Contact& contact, AttributeKind::Type kind, Json::Value& contact_item);

    static std::map<std::string, AttributeKind::Type> attributeKindMap;
    static std::map<std::string, AttributeSubKind::Type> attributeSubKindMap;
    static std::map<AttributeKind::Type, std::string> kindAttributeMap;
    static std::map<AttributeSubKind::Type, std::string> subKindAttributeMap;
    std::map<ContactId, Contact> m_contactSearchMap;
};

} // namespace webworks

#endif // PIM_CONTACTS_QT_H_
