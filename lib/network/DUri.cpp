#include "libdpp/network/DUri.h"

#include <string>
#include <regex>
//#include <sstream>
//#include <iostream>

namespace DTools {
namespace DNetwork {

DUri::DUri (std::string Uri)
{
    Set(Uri);
}

bool DUri::Set(std::string Uri)
{
    UriString=Uri;
    Valid=false;
    if (!UriString.empty()) {
        Valid=Parse();
    }
    return(Valid);
}

void DUri::Clear(void)
{
    UriString.clear();
    Scheme.clear();
    Authority.clear();
    Path.clear();
    Query.clear();
    Fragment.clear();
    User.clear();
    Pwd.clear();
    Host.clear();
    Port.clear();
}

bool DUri::Parse()
{
    std::cmatch MatchUri;
    std::cmatch MatchAuthority;
    // Validate Uri
    //if (!std::regex_match(UriString.c_str(), MatchUri, std::regex(URI_VALIDATE_REGEX))) {
    if (!IsValidUri(UriString)) {
        Error("Wrong Uri format");
        return false;
    }
    // Split Uri
    if (!std::regex_match(UriString.c_str(), MatchUri, std::regex(RFC2396_SPLIT_REGEX))) {
        Error("Uri can not be splitted");
        return false;
    }
    // Uri parts
    Scheme.assign(MatchUri[MATCH_URI_SCHEME].first, MatchUri[MATCH_URI_SCHEME].second);
    Authority=std::string(MatchUri[MATCH_URI_AUTHORITY].first, MatchUri[MATCH_URI_AUTHORITY].second);
    Path.assign(MatchUri[MATCH_URI_PATH].first, MatchUri[MATCH_URI_PATH].second);
    Query.assign(MatchUri[MATCH_URI_QUERY].first, MatchUri[MATCH_URI_QUERY].second);
    Fragment.assign(MatchUri[MATCH_URI_FRAGMENT].first, MatchUri[MATCH_URI_FRAGMENT].second);
    // Parse Authority

    if (!Authority.empty()) {
        // Authority present
        if (!std::regex_match(Authority.c_str(), MatchAuthority, std::regex(AUTHORITY_SPLIT_REGEX))) {
            Error("Wrong Authority format");
            return false;
        }

        // Authority parts
        User.assign(MatchAuthority[MATCH_AUTH_USER].first, MatchAuthority[MATCH_AUTH_USER].second);
        Pwd.assign(MatchAuthority[MATCH_AUTH_PWD].first, MatchAuthority[MATCH_AUTH_PWD].second);
        Host.assign(MatchAuthority[MATCH_AUTH_HOST].first, MatchAuthority[MATCH_AUTH_HOST].second);
        Port.assign(MatchAuthority[MATCH_AUTH_PORT].first, MatchAuthority[MATCH_AUTH_PORT].second);
        if (Port.empty()) {
            // Port not set in uri, try to detect commons
            if (Scheme == "https")      Port="443";
            if (Scheme == "http")       Port="80";
            if (Scheme == "ssh")        Port="22";
            if (Scheme == "ftp")        Port="21";
            if (Scheme == "mysql")      Port="3306";
            if (Scheme == "mongo")      Port="27017";
            if (Scheme == "mongo+srv")  Port="27017";
            if (Scheme == "kafka")      Port="9092";
            if (Scheme == "postgres")   Port="5432";
            if (Scheme == "postgresql") Port="5432";
            if (Scheme == "redis")      Port="6379";
            if (Scheme == "zookeeper")  Port="2181";
            if (Scheme == "ldap")       Port="389";
            if (Scheme == "ldaps")      Port="636";
        }
    }
    return true;
}

bool DUri::IsValid(void)
{
    return(Valid);
}

std::string DUri::PrintUriParts(void)
{
    std::stringstream ss;

    ss << "Url:       " << UriString    << std::endl;
    ss << "Scheme:    " << Scheme       << std::endl;
    ss << "Authority: " << Authority    << std::endl;
    ss << "Path:      " << Path         << std::endl;
    ss << "Query:     " << Query        << std::endl;
    ss << "Fragment:  " << Fragment     << std::endl;
    ss << "User:      " << User         << std::endl;
    ss << "Password:  " << Pwd          << std::endl;
    ss << "Host:      " << Host         << std::endl;
    ss << "Port:      " << Port         << std::endl;

    return(ss.str());
}

std::string DUri::EncodeUserInfo(const std::string& UserInfoStr)
{
    return(Encode(UserInfoStr,R"(:)"));
}

std::string DUri::EncodeHost(const std::string& HostStr)
{
    return(Encode(HostStr,R"(\[:\])"));
}

std::string DUri::EncodePath(const std::string& PathStr)
{
    return(Encode(PathStr,R"(\/.@%;=)"));
}

std::string DUri::EncodeQuery(const std::string& QueryStr)
{
    return(Encode(QueryStr,R"(\/?)"));
}

std::string DUri::EncodeFragment(const std::string& FragmentStr) {
    return(Encode(FragmentStr,R"(\/.@&l;=%)"));
}

std::string DUri::Encode(std::string Uri, std::string Ignore)
{
    std::ostringstream out;
    std::regex Regex(R"([!'\(\)*0-9A-Za-z\-._~)" + Ignore + R"(])");

    for (char &c : Uri) {
        if (std::regex_match((std::string){c},Regex)) {
            out << c;
        }
        else {
            out << "%" << std::uppercase << std::hex << (0xff & c);
        }
    }
    return out.str();
}


std::string DUri::Decode(const std::string& EncodedUri)
{
    std::string DecodedUri=EncodedUri;
    std::smatch sm;
    std::string haystack;

    int dynamicLength=DecodedUri.size()-2;
    if (DecodedUri.size() < 3) return DecodedUri;

    for (int i=0; i<dynamicLength; i++) {
        haystack = DecodedUri.substr(i, 3);
        if (std::regex_match(haystack,sm,std::regex("%[0-9A-F]{2}"))) {
            haystack=haystack.replace(0, 1, "0x");
            std::string rc={(char)std::stoi(haystack, nullptr, 16)};
            DecodedUri=DecodedUri.replace(DecodedUri.begin() + i, DecodedUri.begin() + i + 3, rc);
        }
        dynamicLength=DecodedUri.size() - 2;
    }
    return DecodedUri;
}

/**
 * @brief Validate an URI string.
 * @param UriString ->  uri string to check.
 * @return true if UriString is a valid URI format, otherwise false.
 */
bool DUri::IsValidUri(std::string UriString)
{
    std::cmatch MatchUri;
    if (!std::regex_match(UriString.c_str(), MatchUri, std::regex(URI_VALIDATE_REGEX))) {
        return false;
    }
    return true;
}

/**
 * @brief Validate an URL string.
 * @param UriString ->  url string to check.
 * @return true if UriString is a valid URL format, otherwise false.
 */
bool DUri::IsValidUrl(std::string UrlString)
{
    std::cmatch MatchUrl;
    if (!std::regex_match(UrlString.c_str(), MatchUrl, std::regex(URL_VALIDATE_REGEX))) {
        return false;
    }
    return true;
}

// ****************************  Log/error functions *************************************
/**
* @brief Set LastStrStatus and make log callback.
* @param LogMsg	->  Message to log.
*
* N.B. If LogMsg is empty, callback with LastStrStatus is performed.
**/
void DUri::Log(std::string LogMsg)
{
	if (!LogMsg.empty()) {
		LastStrStatus="INFO: "+LogMsg;
	}
}

/**
* @brief Set LastStrStatus and make error callback.
* @param ErrorMsg	->  Error message.
*
* N.B. If LogMsg is empty, callback with LastStrStatus is performed.
**/
void DUri::Error(std::string ErrorMsg)
{
	if (!ErrorMsg.empty()) {
		LastStrStatus="ERROR: "+ErrorMsg;
	}
}

//! @return LastStrStatus string.
std::string DUri::GetLastStatus(void)
{
	return(LastStrStatus);
}
// ***************************************************************************************

} // DTools
} // DNetwork
