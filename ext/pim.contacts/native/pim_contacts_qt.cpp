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

#include <json/writer.h>
#include <json/reader.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <QSet>
#include <QMap>
#include "pim_contacts_qt.hpp"

namespace webworks {

std::map<std::string, AttributeKind::Type> PimContactsQt::attributeKindMap;
std::map<std::string, AttributeSubKind::Type> PimContactsQt::attributeSubKindMap;

PimContactsQt::PimContactsQt()
{
    static bool mapInit = false;

    if (!mapInit) {
        createAttributeKindMap();
        createAttributeSubKindMap();
        createKindAttributeMap();
        createSubKindAttributeMap();
        mapInit = true;
    }
}

PimContactsQt::~PimContactsQt()
{
}

QSet<ContactId> PimContactsQt::singleFieldSearch(const Json::Value& search_field_json, /*const Json::Value& sort_fields_json,*/ bool favorite)
{
    ContactService contact_service;
    ContactSearchFilters contact_filter;
    QList<SearchField::Type> search_fields;
    QList<SortSpecifier> sort_fields;
    QList<Contact> results;
    QSet<ContactId> contact_ids_set;

    switch (search_field_json["fieldName"].asInt()) {
        case SearchField::FirstName:
            search_fields.append(SearchField::FirstName);
            break;
        case SearchField::LastName:
            search_fields.append(SearchField::LastName);
            break;
        case SearchField::CompanyName:
            search_fields.append(SearchField::CompanyName);
            break;
        case SearchField::Phone:
            search_fields.append(SearchField::Phone);
            break;
        case SearchField::Email:
            search_fields.append(SearchField::Email);
            break;
        case SearchField::BBMPin:
            search_fields.append(SearchField::BBMPin);
            break;
        case SearchField::LinkedIn:
            search_fields.append(SearchField::LinkedIn);
            break;
        case SearchField::Twitter:
            search_fields.append(SearchField::Twitter);
            break;
        case SearchField::VideoChat:
            search_fields.append(SearchField::VideoChat);
            break;
    }

    contact_filter.setSearchFields(search_fields);

    contact_filter.setSearchValue(QString(search_field_json["fieldValue"].asString().c_str()));
/*
    for (int i = 0; i < sort_fields_json.size(); i++) {
        SortOrder::Type order;
        SortColumn::Type sort_field;

        if (sort_fields_json[i]["desc"].asBool()) {
            order = SortOrder::Descending;
        } else {
            order = SortOrder::Ascending;
        }

        if (sort_fields_json[i]["fieldName"] == SortColumn::FirstName) {
            sort_field = SortColumn::FirstName;
        } else if (sort_fields_json[i]["fieldName"] == SortColumn::LastName) {
            sort_field = SortColumn::LastName;
        } else if (sort_fields_json[i]["fieldName"] == SortColumn::CompanyName) {
            sort_field = SortColumn::CompanyName;
        }

        sort_fields.append(SortSpecifier(sort_field, order));
    }

    // sorting here don't really make sense because this is not the final search results
    contact_filter.setSortBy(sort_fields);
*/
    if (favorite) {
        contact_filter.setIsFavourite(favorite);
    }

    results = contact_service.searchContacts(contact_filter);

    for (int i = 0; i < results.size(); i++) {
        contact_ids_set.insert(results[i].id());
        m_contactSearchMap[results[i].id()] = results[i];
    }

    return contact_ids_set;
}

void PimContactsQt::populateContactField(const Contact& contact, AttributeKind::Type kind, Json::Value& contact_item)
{
    QList<ContactAttribute> attrs = contact.filteredAttributes(kind);
    QList<ContactAttribute>::const_iterator k = attrs.constBegin();
    Json::Value array;

    while (k != attrs.constEnd()) {
        ContactAttribute curr_attr = *k;
        Json::Value val;
        std::map<AttributeSubKind::Type, std::string>::const_iterator type_iter = subKindAttributeMap.find(curr_attr.subKind());

        if (type_iter != subKindAttributeMap.end()) {
            val["type"] = Json::Value(type_iter->second);
            val["value"] = Json::Value(curr_attr.value().toStdString());
            array.append(val);
        } else {
            // TODO
        }
    }

    std::map<AttributeKind::Type, std::string>::const_iterator field_iter = kindAttributeMap.find(kind);

    if (field_iter != kindAttributeMap.end()) {
        contact_item[field_iter->second] = array;
    } else {
        // TODO
    }
}

Json::Value PimContactsQt::assembleSearchResults(const QSet<ContactId>& results, const Json::Value& contact_fields, const Json::Value& sort_fields_json, int limit)
{
    // TODO index by first name for now
    QMap<QString, Contact> complete_results;

    QSet<ContactId>::const_iterator i = results.constBegin();
    Contact current_contact;

    while (i != results.constEnd()) {
        current_contact = m_contactSearchMap[*i];
        complete_results[current_contact.firstName()] = current_contact;
        ++i;
    }

    QMap<QString, Contact>::const_iterator j = complete_results.constBegin();
    QList<Contact> contact_list;
    while (j != complete_results.constEnd()) {
        contact_list.append(j.value());
        ++j;
    }

    Json::Value contact_obj;
    Json::Value contact_array;

    if (limit == -1) {
        limit = contact_list.size();
    } else {
        limit = min(limit, contact_list.size());
    }

    for (int i = 0; i < limit; i++) {
        Json::Value contact_item;

        for (int j = 0; j < contact_fields.size(); j++) {
            std::map<std::string, AttributeKind::Type>::const_iterator kind_iter = attributeKindMap.find(contact_fields[j].asString());

            if (kind_iter != attributeKindMap.end()) {
                switch (kind_iter->second) {
                case AttributeKind::Name:
                    {
                        contact_item["name"] = Json::Value();
                        contact_item["name"]["givenName"] = Json::Value(contact_list[i].firstName().toStdString());
                        contact_item["name"]["familyName"] = Json::Value(contact_list[i].lastName().toStdString());
                        contact_item["displayName"] = Json::Value(contact_list[i].displayName().toStdString());
                        // TODO other fields in name
                        break;
                    }

                case AttributeKind::Email:
                    {
                        populateContactField(contact_list[i], AttributeKind::Email, contact_item);
                        break;
                    }
                }
            }
        }

        // TODO have to look at what fields to include
        contact_item["displayName"] = Json::Value(contact_list[i].displayName().toStdString());
        contact_item["contactId"] = Json::Value(contact_list[i].id());

        contact_array.append(contact_item);
    }

    contact_obj["contacts"] = contact_array;

    return contact_obj;
}

std::string PimContactsQt::find(const std::string& args_json)
{
    Json::Reader reader;
    Json::Value args_obj;
    bool parse = reader.parse(args_json, args_obj);

    if (!parse) {
        fprintf(stderr, "%s", "error parsing\n");
        throw "Cannot parse JSON object";
    }

    m_contactSearchMap.clear();

    ContactService contact_service;
    QList<Contact> contact_list;
    Json::Value contact_fields;
    QSet<ContactId> results;

    if (args_obj.empty()) {
        ContactListFilters contact_filter;
        contact_list = contact_service.contacts(contact_filter);
    } else {
        ContactSearchFilters contact_filter;
        const Json::Value::Members args_key = args_obj.getMemberNames();

        for (int i = 0; i < args_key.size(); i++) {
            const std::string key = args_key[i];

            if (key == "fields") {
                contact_fields = args_obj[key];
            } else if (key == "options") {
                Json::Value filter = args_obj["filter"];

                if (filter != NULL && filter.isArray()) {
                    for (int j = 0; j < filter.size(); j++) {
                        QSet<ContactId> current_results = singleFieldSearch(filter[j], /*options_obj["sort"],*/ args_obj[key]["favorite"].asBool());

                        if (current_results.empty()) {
                            // no need to continue...can return right away
                            results = current_results;
                            break;
                        } else {
                            if (j == 0) {
                                results = current_results;
                            } else {
                                results.intersect(current_results);
                            }
                        }
                    }
                }
            }
        }
    }
    
    Json::Value contact_obj = assembleSearchResults(results, contact_fields, NULL, args_obj["options"]["limit"].asInt());
    Json::FastWriter writer;

    return writer.write(contact_obj);
}

void PimContactsQt::createContact(const std::string& attributeJson) {
    Json::Reader reader;
    Json::Value attribute_obj;
    bool parse = reader.parse(attributeJson, attribute_obj);

    if (!parse) {
        fprintf(stderr, "%s", "error parsing\n");
        throw "Cannot parse JSON object";
    }

    const Json::Value::Members attribute_keys = attribute_obj.getMemberNames();

    Contact new_contact;
    ContactBuilder contact_builder(new_contact.edit());

    for (int i = 0; i < attribute_keys.size(); i++) {
        const std::string key = attribute_keys[i];

        std::map<std::string, AttributeKind::Type>::const_iterator kind_iter = attributeKindMap.find(key);

        if (kind_iter != attributeKindMap.end()) {
            switch (kind_iter->second) {
                case AttributeKind::Name:
                    {
                        // TODO: Are group keys needed here?
                        Json::Value name_obj = attribute_obj[key];
                        const Json::Value::Members name_fields = name_obj.getMemberNames();

                        for (int j = 0; j < name_fields.size(); j++) {
                            const std::string name_key = name_fields[j];
                            std::map<std::string, AttributeSubKind::Type>::const_iterator name_iter = attributeSubKindMap.find(name_key);

                            if (name_iter != attributeSubKindMap.end()) {
                                ContactAttribute attribute;
                                ContactAttributeBuilder attribute_builder(attribute.edit());

                                attribute_builder = attribute_builder.setKind(AttributeKind::Name);
                                attribute_builder = attribute_builder.setSubKind(name_iter->second);
                                attribute_builder = attribute_builder.setValue(QString(name_obj[name_key].asString().c_str()));

                                contact_builder = contact_builder.addAttribute(attribute);
                            }
                        }

                        break;
                    }
                case AttributeKind::OrganizationAffiliation:
                    {
                        Json::Value attribute_array = attribute_obj[key];

                        for (int j = 0; j < attribute_array.size(); j++) {
                            Json::Value field_obj = attribute_array[j];
                            const Json::Value::Members fields = field_obj.getMemberNames();

                            // TODO: need to use group keys for organizations

                            for (int k = 0; k < fields.size(); k++) {
                                const std::string field_key = fields[k];
                                std::map<std::string, AttributeSubKind::Type>::const_iterator subkind_iter = attributeSubKindMap.find(field_key);

                                if (subkind_iter != attributeSubKindMap.end()) {
                                    ContactAttribute attribute;
                                    ContactAttributeBuilder attribute_builder(attribute.edit());

                                    attribute_builder = attribute_builder.setKind(kind_iter->second);
                                    attribute_builder = attribute_builder.setSubKind(subkind_iter->second);
                                    attribute_builder = attribute_builder.setValue(QString(field_obj[field_key].asString().c_str()));

                                    contact_builder = contact_builder.addAttribute(attribute);
                                }
                            }
                        }

                        break;
                    }
                case AttributeKind::Phone:
                case AttributeKind::Email:
                case AttributeKind::Fax:
                case AttributeKind::Pager:
                case AttributeKind::InstantMessaging:
                case AttributeKind::Website:
                case AttributeKind::Profile:
                    {
                        Json::Value field_array = attribute_obj[key];

                        for (int j = 0; j < field_array.size(); j++) {
                            Json::Value field_obj = field_array[j];

                            std::string type = field_obj["type"].asString();
                            std::string value = field_obj["value"].asString();
                            bool pref = field_obj["pref"].asBool();

                            std::map<std::string, AttributeSubKind::Type>::const_iterator subkind_iter = attributeSubKindMap.find(type);

                            if (subkind_iter != attributeSubKindMap.end()) {
                                ContactAttribute attribute;
                                ContactAttributeBuilder attribute_builder(attribute.edit());

                                attribute_builder = attribute_builder.setKind(kind_iter->second);
                                attribute_builder = attribute_builder.setSubKind(subkind_iter->second);
                                attribute_builder = attribute_builder.setValue(QString(value.c_str()));

                                contact_builder = contact_builder.addAttribute(attribute);
                            }
                        }

                        break;
                    }
                case AttributeKind::Invalid:
                    {
                        if (key == "addresses") {
                            Json::Value address_array = attribute_obj[key];

                            for (int j = 0; j < address_array.size(); j++) {
                                Json::Value address_obj = address_array[j];

                                ContactPostalAddress address;
                                ContactPostalAddressBuilder address_builder(address.edit());

                                if (address_obj.isMember("type")) {
                                    std::string value = address_obj["type"].asString();
                                    std::map<std::string, AttributeSubKind::Type>::const_iterator subkind_iter = attributeSubKindMap.find(value);

                                    if (subkind_iter != attributeSubKindMap.end()) {
                                        address_builder = address_builder.setSubKind(subkind_iter->second);
                                    }
                                }

                                if (address_obj.isMember("address1")) {
                                    std::string value = address_obj["address1"].asString();
                                    address_builder = address_builder.setLine1(QString(value.c_str()));
                                }

                                if (address_obj.isMember("address2")) {
                                    std::string value = address_obj["address2"].asString();
                                    address_builder = address_builder.setLine2(QString(value.c_str()));
                                }

                                if (address_obj.isMember("locality")) {
                                    std::string value = address_obj["locality"].asString();
                                    address_builder = address_builder.setCity(QString(value.c_str()));
                                }

                                if (address_obj.isMember("region")) {
                                    std::string value = address_obj["region"].asString();
                                    address_builder = address_builder.setRegion(QString(value.c_str()));
                                }

                                if (address_obj.isMember("country")) {
                                    std::string value = address_obj["country"].asString();
                                    address_builder = address_builder.setCountry(QString(value.c_str()));
                                }

                                if (address_obj.isMember("postalCode")) {
                                    std::string value = address_obj["postalCode"].asString();
                                    address_builder = address_builder.setPostalCode(QString(value.c_str()));
                                }

                                contact_builder = contact_builder.addPostalAddress(address);
                            }
                        }

                        break;
                    }
            }
        }
    }

    ContactService contact_service;
    contact_service.createContact(new_contact, false);
}

void PimContactsQt::deleteContact(const std::string& contactJson) {
    Json::Reader reader;
    Json::Value obj;
    bool parse = reader.parse(contactJson, obj);

    if (!parse) {
        fprintf(stderr, "%s", "error parsing\n");
        throw "Cannot parse JSON object";
    }

    ContactId contact_id = obj["contactId"].asInt();

    ContactService service;
    service.deleteContact(contact_id);
}

void PimContactsQt::createAttributeKindMap() {
    attributeKindMap["phoneNumbers"] = AttributeKind::Phone;
    attributeKindMap["faxNumbers"] = AttributeKind::Fax;
    attributeKindMap["pagerNumber"] = AttributeKind::Pager;
    attributeKindMap["emails"] = AttributeKind::Email;
    attributeKindMap["urls"] = AttributeKind::Website;
    attributeKindMap["socialNetworks"] = AttributeKind::Profile;
    attributeKindMap["anniversary"] = AttributeKind::Date;
    attributeKindMap["birthday"] = AttributeKind::Date;
    attributeKindMap["name"] = AttributeKind::Name;
    attributeKindMap["displayName"] = AttributeKind::Name;
    attributeKindMap["organizations"] = AttributeKind::OrganizationAffiliation;
    attributeKindMap["education"] = AttributeKind::Education;
    attributeKindMap["note"] = AttributeKind::Note;
    attributeKindMap["ims"] = AttributeKind::InstantMessaging;
    attributeKindMap["videoChat"] = AttributeKind::VideoChat;
    attributeKindMap["addresses"] = AttributeKind::Invalid;
}

void PimContactsQt::createAttributeSubKindMap() {
    attributeSubKindMap["other"] = AttributeSubKind::Other;
    attributeSubKindMap["home"] = AttributeSubKind::Home;
    attributeSubKindMap["work"] = AttributeSubKind::Work;
    attributeSubKindMap["mobile"] = AttributeSubKind::PhoneMobile;
    attributeSubKindMap["direct"] = AttributeSubKind::FaxDirect;
    attributeSubKindMap["blog"] = AttributeSubKind::Blog;
    attributeSubKindMap["resume"] = AttributeSubKind::WebsiteResume;
    attributeSubKindMap["portfolio"] = AttributeSubKind::WebsitePortfolio;
    attributeSubKindMap["personal"] = AttributeSubKind::WebsitePersonal;
    attributeSubKindMap["company"] = AttributeSubKind::WebsiteCompany;
    attributeSubKindMap["facebook"] = AttributeSubKind::ProfileFacebook;
    attributeSubKindMap["twitter"] = AttributeSubKind::ProfileTwitter;
    attributeSubKindMap["linkedin"] = AttributeSubKind::ProfileLinkedIn;
    attributeSubKindMap["gist"] = AttributeSubKind::ProfileGist;
    attributeSubKindMap["tungle"] = AttributeSubKind::ProfileTungle;
    attributeSubKindMap["birthday"] = AttributeSubKind::DateBirthday;
    attributeSubKindMap["anniversary"] = AttributeSubKind::DateAnniversary;
    attributeSubKindMap["givenName"] = AttributeSubKind::NameGiven;
    attributeSubKindMap["familyName"] = AttributeSubKind::NameSurname;
    attributeSubKindMap["honorificPrefix"] = AttributeSubKind::Title;
    attributeSubKindMap["honorificSuffix"] = AttributeSubKind::NameSuffix;
    attributeSubKindMap["middleName"] = AttributeSubKind::NameMiddle;
    attributeSubKindMap["phoneticGivenName"] = AttributeSubKind::NamePhoneticGiven;
    attributeSubKindMap["phoneticFamilyName"] = AttributeSubKind::NamePhoneticSurname;
    attributeSubKindMap["name"] = AttributeSubKind::OrganizationAffiliationName;
    attributeSubKindMap["department"] = AttributeSubKind::OrganizationAffiliationDetails;
    attributeSubKindMap["title"] = AttributeSubKind::Title;
    attributeSubKindMap["BbmPin"] = AttributeSubKind::InstantMessagingBbmPin;
    attributeSubKindMap["Aim"] = AttributeSubKind::InstantMessagingAim;
    attributeSubKindMap["Aliwangwang"] = AttributeSubKind::InstantMessagingAliwangwang;
    attributeSubKindMap["GoogleTalk"] = AttributeSubKind::InstantMessagingGoogleTalk;
    attributeSubKindMap["Sametime"] = AttributeSubKind::InstantMessagingSametime;
    attributeSubKindMap["Icq"] = AttributeSubKind::InstantMessagingIcq;
    attributeSubKindMap["Jabber"] = AttributeSubKind::InstantMessagingJabber;
    attributeSubKindMap["MsLcs"] = AttributeSubKind::InstantMessagingMsLcs;
    attributeSubKindMap["Skype"] = AttributeSubKind::InstantMessagingSkype;
    attributeSubKindMap["YahooMessenger"] = AttributeSubKind::InstantMessagingYahooMessenger;
    attributeSubKindMap["YahooMessegerJapan"] = AttributeSubKind::InstantMessagingYahooMessengerJapan;
    attributeSubKindMap["BbPlaybook"] = AttributeSubKind::VideoChatBbPlaybook;
}

void PimContactsQt::createKindAttributeMap() {
    kindAttributeMap[AttributeKind::Phone] = "phoneNumbers";
    kindAttributeMap[AttributeKind::Fax] = "faxNumbers";
    kindAttributeMap[AttributeKind::Pager] = "pagerNumber";
    kindAttributeMap[AttributeKind::Email] = "emails";
    kindAttributeMap[AttributeKind::Website] = "urls";
    kindAttributeMap[AttributeKind::Profile] = "socialNetworks";
    //attributeKindMap[AttributeKind::Date] = "anniversary";
    // attributeKindMap[AttributeKind::Date] = "birthday";
    //attributeKindMap["name"] = AttributeKind::Name;
    //attributeKindMap["displayName"] = AttributeKind::Name;
    kindAttributeMap[AttributeKind::OrganizationAffiliation] = "organizations";
    kindAttributeMap[AttributeKind::Education] = "education";
    kindAttributeMap[AttributeKind::Note] = "note";
    kindAttributeMap[AttributeKind::InstantMessaging] = "ims";
    kindAttributeMap[AttributeKind::VideoChat] = "videoChat";
    kindAttributeMap[AttributeKind::Invalid] = "addresses";
}

void PimContactsQt::createSubKindAttributeMap() {
    subKindAttributeMap[AttributeSubKind::Other] = "other";
    subKindAttributeMap[AttributeSubKind::Home] = "home";
    subKindAttributeMap[AttributeSubKind::Work] = "work";
    subKindAttributeMap[AttributeSubKind::PhoneMobile] = "mobile";
    subKindAttributeMap[AttributeSubKind::FaxDirect] = "direct";
    subKindAttributeMap[AttributeSubKind::Blog] = "blog";
    subKindAttributeMap[AttributeSubKind::WebsiteResume] = "resume";
    subKindAttributeMap[AttributeSubKind::WebsitePortfolio] = "portfolio";
    subKindAttributeMap[AttributeSubKind::WebsitePersonal] = "personal";
    subKindAttributeMap[AttributeSubKind::WebsiteCompany] = "company";
    subKindAttributeMap[AttributeSubKind::ProfileFacebook] = "facebook";
    subKindAttributeMap[AttributeSubKind::ProfileTwitter] = "twitter";
    subKindAttributeMap[AttributeSubKind::ProfileLinkedIn] = "linkedin";
    subKindAttributeMap[AttributeSubKind::ProfileGist] = "gist";
    subKindAttributeMap[AttributeSubKind::ProfileTungle] = "tungle";
    subKindAttributeMap[AttributeSubKind::DateBirthday] = "birthday";
    subKindAttributeMap[AttributeSubKind::DateAnniversary] = "anniversary";
    subKindAttributeMap[AttributeSubKind::NameGiven] = "givenName";
    subKindAttributeMap[AttributeSubKind::NameSurname] = "familyName";
    subKindAttributeMap[AttributeSubKind::Title] = "honorificPrefix";
    subKindAttributeMap[AttributeSubKind::NameSuffix] = "honorificSuffix";
    subKindAttributeMap[AttributeSubKind::NameMiddle] = "middleName";
    subKindAttributeMap[AttributeSubKind::NamePhoneticGiven] = "phoneticGivenName";
    subKindAttributeMap[AttributeSubKind::NamePhoneticSurname] = "phoneticFamilyName";
    subKindAttributeMap[AttributeSubKind::OrganizationAffiliationName] = "name";
    subKindAttributeMap[AttributeSubKind::OrganizationAffiliationDetails] = "department";
    subKindAttributeMap[AttributeSubKind::Title] = "title";
    subKindAttributeMap[AttributeSubKind::InstantMessagingBbmPin] = "BbmPin";
    subKindAttributeMap[AttributeSubKind::InstantMessagingAim] = "Aim";
    subKindAttributeMap[AttributeSubKind::InstantMessagingAliwangwang] = "Aliwangwang";
    subKindAttributeMap[AttributeSubKind::InstantMessagingGoogleTalk] = "GoogleTalk";
    subKindAttributeMap[AttributeSubKind::InstantMessagingSametime] = "Sametime";
    subKindAttributeMap[AttributeSubKind::InstantMessagingIcq] = "Icq";
    subKindAttributeMap[AttributeSubKind::InstantMessagingJabber] = "Jabber";
    subKindAttributeMap[AttributeSubKind::InstantMessagingMsLcs] = "MsLcs";
    subKindAttributeMap[AttributeSubKind::InstantMessagingSkype] = "Skype";
    subKindAttributeMap[AttributeSubKind::InstantMessagingYahooMessenger] = "YahooMessenger";
    subKindAttributeMap[AttributeSubKind::InstantMessagingYahooMessengerJapan] = "YahooMessegerJapan";
    subKindAttributeMap[AttributeSubKind::VideoChatBbPlaybook] = "BbPlaybook";
}

} // namespace webworks
