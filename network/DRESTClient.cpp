#include "DRESTClient.h"
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

namespace asio  =   boost::asio;
namespace beast =   boost::beast;
namespace http  =   beast::http;
namespace pt    =   boost::property_tree;
using     tcp   =   boost::asio::ip::tcp;


#define DEFAULT_HTTP_VERSION HTTP_1_1
#define DEFAULT_CONTENT_TYPE CONTENT_TYPE_NONE
#define DEFAULT_ENCODE ENCODE_NONE
#define DEFAULT_TIMEOUT_SEC 30
#define DEFAULT_USER_AGENT "DRESTClient/0.1"
//#define DEFAULT_USER_AGENT BOOST_BEAST_VERSION_STRING

#define DEBUG

namespace DTools {
namespace DNetwork {

// ***************************************************************************************
// ***************************** DRESTClient class ***************************************
// ***************************************************************************************
DRESTClient::DRESTClient(boost::asio::io_context& io_context) :
    TcpResolver(asio::make_strand(io_context)),
    TcpStream(asio::make_strand(io_context))
{
    Connected=false;
    DisconnectAfter=false;
    TimeoutSec=DEFAULT_TIMEOUT_SEC;
    UserAgent=DEFAULT_USER_AGENT;
    HttpVersion=DEFAULT_HTTP_VERSION;
    ContentType=DEFAULT_CONTENT_TYPE;
    EncodeType=DEFAULT_ENCODE;
    KeepAlive=true;
}

/**
 * @brief Set version of http protocol.
 * @param Version   ->  Can be HTTP_1_0 or HTTP_1_1 (default HTTP_1_1).
 */
void DRESTClient::SetHttpVersion(DRESTClient::DHttpVersion dHttpVersion)
{
    HttpVersion=dHttpVersion;
}

/**
 * @brief Set content-type header field.
 * @param dContentType  ->  Can be one of DContentType enumarator.
 */
void DRESTClient::SetContentType(DRESTClient::DContentType dContentType)
{
    ContentType=dContentType;
}

/**
 * @brief Set encode-type header field.
 * @param dEncodeType  ->  Can be one of DContentType enumarator.
 */
void DRESTClient::SetEncodeType(DRESTClient::DEncodeType dEncodeType)
{
    EncodeType=dEncodeType;
}

/**
 * @brief Set comunication timout in seconds.
 * @param Sec   ->  Number of seconds.
 */
void DRESTClient::SetTimeout(uint8_t Sec)
{
    TimeoutSec=Sec;
}

/**
 * @brief Set client request url
 * @param Url   ->  REST service url
 */
void DRESTClient::SetUrl(const std::string Url)
{
    dUri.Set(Url);
}

/**
 * @brief Set keep-alive option in request
 * @param Enabled   ->  It true "in http header "Connection: keep-alive" is add to header.
 */
void DRESTClient::SetKeepAlive(bool Enabled)
{
    KeepAlive=Enabled;
}

/**
 * @brief Create a list o params (key=value) in body section of header using map<string,sting> as source.
 * @param Params    ->  a map<string,string> containing list of params.
 */
void DRESTClient::SetReqHdrBodyParams(std::map<std::string,std::string> Params)
{
    HttpReqHdrBodyParams=Params;
}

/**
 * @brief Create a list o params (key=value) in body using map<string,sting> as source.
 * @param Params    ->  a map<string,string> containing list of params.
 */
void DRESTClient::SetReqBodyParams(std::map<std::string,std::string> Params)
{
    HttpReqBodyParams=Params;
}

/**
 * @brief Add key=value param to body field of header.
 * @param Key   ->  Key string.
 * @param Value ->  Value string.
 */
void DRESTClient::AddReqHdrBodyParam(std::string Key, std::string Value)
{
    HttpReqHdrBodyParams.insert(std::make_pair(Key,Value));
}

/**
 * @brief Add key=value param to request body.
 * @param Key   ->  Key string.
 * @param Value ->  Value string.
 */
void DRESTClient::AddReqBodyParam(std::string Key, std::string Value)
{
    HttpReqBodyParams.insert(std::make_pair(Key,Value));
}

/**
 * @brief Clear body section in header.
 */
void DRESTClient::ClearHrdBodyParams(void)
{
    HttpReqHdrBodyParams.clear();
}

/**
 * @brief Clear request body.
 */
void DRESTClient::ClearBodyParams(void)
{
    HttpReqBodyParams.clear();
}

/**
 * @brief Clear all buffers, request, response, uri, params and set default values;
 */
void DRESTClient::Clear(void)
{
    if (Connected) Disconnect();
    HttpRequest.clear();
    HttpResponse.clear();
    Buffer.clear();
    dUri.Clear();
    HttpReqHdrBodyParams.clear();
    HttpReqBodyParams.clear();
    Connected=false;
    DisconnectAfter=false;
    TimeoutSec=DEFAULT_TIMEOUT_SEC;
    UserAgent=DEFAULT_USER_AGENT;
    HttpVersion=DEFAULT_HTTP_VERSION;
    ContentType=DEFAULT_CONTENT_TYPE;
    EncodeType=DEFAULT_ENCODE;
}

/**
 * @return comunication timout in seconds.
 */
uint8_t DRESTClient::GetTimeout(void)
{
    return(TimeoutSec);
}

/**
 * @return current encode-type as DEncodeType.
 */
DRESTClient::DEncodeType DRESTClient::GetEncodeType(void)
{
    return(EncodeType);
}

/**
 * @return current content-type as DContentType
 */
DRESTClient::DContentType DRESTClient::GetContentType(void)
{
    return(ContentType);
}

/**
 * @brief Retrive body content of header.
 * @param ResultStr ->  String into put body.
 * @return size of body in bytes.
 */
size_t DRESTClient::GetReqHdrBodyAsString(std::string &ResultStr)
{
    ResultStr.clear();
    auto itr=HttpReqHdrBodyParams.begin();
    ResultStr.append(itr->first+"="+itr->second);
    for (++itr; itr!=HttpReqHdrBodyParams.end(); ++itr) {
        ResultStr.append("&"+itr->first+"="+itr->second);
    }

    return(ResultStr.size());
}

/**
 * @brief Retrive body content of header.
 * @param ResultStr     ->  String into put body.
 * @param EncodeType    ->  Type of encoding to apply, can be one of DEncodeType enumarated.
 * @return size of body in bytes.
 */
size_t DRESTClient::GetReqHdrBodyAsString(std::string &ResultStr, DEncodeType dEncodeType)
{
    std::string HdrBody;
    GetReqHdrBodyAsString(HdrBody);
    if (dEncodeType == ENCODE_NONE) {
        ResultStr.assign(HdrBody);
    }
    else {
        Encode(HdrBody,ResultStr,dEncodeType);
    }

    return(ResultStr.size());
}

/**
 * @brief Retrive body content of header.
 * @param EncodeType    ->  Type of encoding to apply, can be one of DEncodeType enumarated.
 * @return body as std::string.
 */
std::string DRESTClient::GetReqHdrBodyAsString(DEncodeType dEncodeType)
{
    std::string HdrBody;
    if (dEncodeType == ENCODE_NONE) {
        GetReqHdrBodyAsString(HdrBody);
    }
    else {
        GetReqHdrBodyAsString(HdrBody,dEncodeType);
    }

    return(HdrBody);
}

/**
 * @brief Retrive request body content.
 * @param ResultStr ->  String into put body.
 * @return size of body in bytes.
 */
size_t DRESTClient::GetReqBodyAsString(std::string &ResultStr)
{
    ResultStr.clear();
    auto itr=HttpReqBodyParams.begin();
    ResultStr.append(itr->first+"="+itr->second);
    for (++itr; itr!=HttpReqBodyParams.end(); ++itr) {
        ResultStr.append("&"+itr->first+"="+itr->second);
    }

    return(ResultStr.size());
}

/**
 * @brief Retrive request body content.
 * @param ResultStr     ->  String into put body.
 * @param EncodeType    ->  Type of encoding to apply, can be one of DEncodeType enumarated.
 * @return size of body in bytes.
 */
size_t DRESTClient::GetReqBodyAsString(std::string &ResultStr, DEncodeType dEncodeType)
{
    std::string Body;
    GetReqBodyAsString(Body);

    if (dEncodeType == ENCODE_NONE) {
        ResultStr.assign(Body);
    }
    else {
        Encode(Body,ResultStr,dEncodeType);
    }

    return(ResultStr.size());
}

/**
 * @brief Retrive request body content.
 * @param EncodeType    ->  Type of encoding to apply, can be one of DEncodeType enumarated.
 * @return body as std::string.
 */
std::string DRESTClient::GetReqBodyAsString(DEncodeType dEncodeType)
{
    std::string Body;
    if (dEncodeType == ENCODE_NONE) {
        GetReqBodyAsString(Body);
    }
    else {
        GetReqBodyAsString(Body,dEncodeType);
    }

    return(Body);
}

/**
 * @return reference to DUri object that contain Url.
 */
DUri& DRESTClient::GetUri(void)
{
    return(dUri);
}

/**
 * @return build and return current HttpRequest.
 * @param RequestType   ->  Type of request can be one of REQ_POST, REQ_PUT, REQ_GET, REQ_DELETE.
 */
DRESTClient::DHttpRequest DRESTClient::GetHttpRequest(DRequestType dRequestType)
{
    PrepareHttpRequest(dRequestType);
    return(HttpRequest);
}

/**
 * @return true if keep-alive option is active.
 */
bool DRESTClient::GetKeepAlive(void)
{
    return KeepAlive;
}

/**
 * @brief Encode string according to DEncodeType value.
 * @param Content       ->  String to encode.
 * @param ResultStr     ->  Destination string for encoded result.
 * @param EncodeType    ->  Can be one of DEncodeType enumarated values.
 * @return size in bytes of encoded string.
 */
size_t DRESTClient::Encode(std::string& Content, std::string& ResultStr, DEncodeType dEncodeType)
{
    switch (dEncodeType) {
        case ENCODE_USER_INFO:
            ResultStr=DUri::EncodeUserInfo(Content);
            break;
        case ENCODE_HOST:
            ResultStr=DUri::EncodeHost(Content);
            break;
        case ENCODE_PATH:
            ResultStr=DUri::EncodePath(Content);
            break;
        case ENCODE_QUERY:
            ResultStr=DUri::EncodeQuery(Content);
            break;
        case ENCODE_FRAGMENT:
            ResultStr=DUri::EncodeFragment(Content);
            break;
        case ENCODE_ALL:
            ResultStr=DUri::Encode(Content);
            break;
        case ENCODE_NONE:
            break;
        default:
            std::cerr << "EncodeType not supported" << std::endl;
    }
    return(ResultStr.size());
}

/**
 * @brief Encode string according to DEncodeType value.
 * @param Content       ->  String to encode.
 * @param EncodeType    ->  Can be one of DEncodeType enumarated values.
 * @return encoded string.
 */
std::string DRESTClient::Encode(std::string& Content, DEncodeType dEncodeType)
{
    std::string ResultStr;
    Encode(Content,ResultStr,dEncodeType);
    return(ResultStr);
}

/**
 * @brief Perform a sync (blocking) connection.
 * @param Force ->  If true force connection in case of Connected variable is true.
 * @return true on success connection, otherwise false. Can retrive error from LastStrStatus string.
 * This function is blocking until connection hand-shake is ended or time-out is reached.
 */
bool DRESTClient::Connect(bool Force)
{
    if (Connected && !Force) {
        std::cerr << "Already connected" << std::endl;
        return false;
    }
    else if (dUri.Host.empty()) {
        std::cerr << "Missing host in url";
        return false;
    }
    else if (dUri.Port.empty()) {
        std::cerr << "Missing port in url";
        return false;
    }

    Connected=false;

    beast::error_code ec;
    // Print Resolver info
    boost::asio::ip::tcp::resolver::iterator iter = TcpResolver.resolve(dUri.Host,dUri.Port,ec);
    boost::asio::ip::tcp::resolver::iterator end;
    while (iter != end) {
        boost::asio::ip::tcp::endpoint endpoint = *iter++;
        std::cout << endpoint << std::endl;
    }
    // Sync resolve
    asio::ip::tcp::resolver::results_type results=TcpResolver.resolve(dUri.Host,dUri.Port,ec);
    if (ec) {
        std::cerr << "error resolve" << ": " << ec.message() << "\n";
        return false;
    }
    // Sync connect
    TcpStream.expires_after(std::chrono::seconds(TimeoutSec));
    TcpStream.connect(results,ec);
    if (ec) {
        std::cerr << "error connect" << ": " << ec.value() << ": " << ec.message() << "\n";
        return false;
    }
    std::cout << "Connected" << std::endl;
    Connected=true;
    return true;
}

/**
 * @brief DRESTClient::AsyncConnect
 * @param Force
 */
void DRESTClient::AsyncConnect(bool Force)
{
    if (Connected && !Force) {
        std::cerr << "Already connected" << std::endl;
        return;
    }
    else if (dUri.Host.empty()) {
        std::cerr << "Missing host in url";
        return;
    }
    else if (dUri.Port.empty()) {
        std::cerr << "Missing port in url";
        return;
    }

    Connected=false;
    // Async resolve
    TcpResolver.async_resolve(dUri.Host,dUri.Port, [this] (boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
        if (ec) {
            std::cerr << "resolve" << ": " << ec.message() << "\n";
        }
        // Async connect
        TcpStream.expires_after(std::chrono::seconds(30));
        TcpStream.async_connect(results,[this](boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type) {
            if (ec) {
                std::cerr << "connect" << ": " << ec.message() << "\n";
            }
            Connected=true;
        });
    });
}

/**
 * @brief Perform a sync (blocking) diconnect.
 * @param Force ->  If true force disconnection in case of Connected variable is false.
 * @return true on success connection, otherwise false. Can retrive error from LastStrStatus string.
 * This function is blocking until disconnection is made.
 */
bool DRESTClient::Disconnect(bool Force)
{
    if (!Connected && !Force) {
        return false;
    }

    beast::error_code ec;
    TcpStream.socket().shutdown(tcp::socket::shutdown_both, ec);

    if (ec && ec != beast::errc::not_connected) {
        std::cerr << "error shutdown" << ": " << ec.message() << "\n";
        return false;
    }
    std::cout << "Disconnected" << std::endl;
    Connected=false;
    return true;
}

/**
 * @brief Build the HttpRequest.
 * @param Verb  ->  Type of request can be one of REQ_POST, REQ_PUT, REQ_GET, REQ_DELETE.
 */
void DRESTClient::PrepareHttpRequest(DRESTClient::DRequestType dReqestType)
{
    // Set request Header
    HttpRequest.clear();
    // Http version
    HttpRequest.version(HttpVersion);
    //Verb
    HttpRequest.method(dReqestType);
    // Target
    if (!dUri.Path.empty()) {
        HttpRequest.target(dUri.Path);
    }
    // User agent
    HttpRequest.set(http::field::user_agent,UserAgent);
    // Host
    HttpRequest.set(http::field::host,dUri.Host);
    // Keep alive
    HttpRequest.keep_alive(KeepAlive);
    // Content type
    HttpRequest.set(http::field::content_type,DContents[ContentType].Verb);
    // Header body
    if (HttpReqHdrBodyParams.size() > 0) {
        HttpRequest.set(http::field::body,GetReqHdrBodyAsString(EncodeType));
    }
    // Body
    if (HttpReqBodyParams.size() > 0) {
        HttpRequest.body()=GetReqBodyAsString(EncodeType);
    }
    // Build request
    HttpRequest.prepare_payload();
}

/**
 * @brief Send a POST request using current data stored.
 */
void DRESTClient::SendPOST(void)
{
    PrepareHttpRequest(REQ_POST);
    Send();
}

/**
 * @brief Send a PUT request using current data stored.
 */
void DRESTClient::SendPUT(void)
{
    PrepareHttpRequest(REQ_PUT);
    Send();
}

/**
 * @brief Send a GET request using current data stored.
 */
void DRESTClient::SendGET(void)
{
    PrepareHttpRequest(REQ_GET);
    Send();
}

void DRESTClient::Send(void)
{
    if (!Connected) {
        if (!Connect()) {
            DisconnectAfter=true;
            std::cerr << "Connection failed" << std::endl;
            return;
        }
    }
    else if (dUri.Host.empty()) {
        std::cerr << "Missing host in url";
        return;
    }

    // Send request
    AsyncWrite();
}

/**
 * @brief Perform an async write of HttpRequest.
 * The function return immediatly and OnWrite funcion is called after data is sent.
 */
void DRESTClient::AsyncWrite(void)
{
    #ifdef DEBUG
        std::stringstream ss;
        ss << std::endl << "HttpRequest:" << std::endl;
        ss << HttpRequest << std::endl;
        Log(ss.str());
    #endif

    TcpStream.expires_after(std::chrono::seconds(TimeoutSec));
    http::async_write(TcpStream,HttpRequest,beast::bind_front_handler(&DRESTClient::OnWrite,shared_from_this()));
}

/**
 * @brief Callback fired on async_write data sent.
 * @param ec                ->  contain error_code on write error.
 * @param bytes_transferred ->  contain numer of byte sent.
 */
void DRESTClient::OnWrite(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        std::cerr << "write" << ": " << ec.message() << "\n";
        return;
    }
    AsyncRead();
}

/**
 * @brief Perform an async read operation.
 * The function return immediatly and OnReade funcion is called after data is receved.
 * Responce wil be put in HttpResponse object.
 */
void DRESTClient::AsyncRead(void)
{
    http::async_read(TcpStream,Buffer,HttpResponse,beast::bind_front_handler(&DRESTClient::OnRead,shared_from_this()));
}

/**
 * @brief Callback fired on async_read data received.
 * @param ec                ->  contain error_code on write error.
 * @param bytes_transferred ->  contain number of bytes received.
 */
void DRESTClient::OnRead(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        std::cerr << "read" << ": " << ec.message() << "\n";
    }

    //std::cout << std::endl << "HttpResponse:" << std::endl;
    //std::cout << HttpResponse << std::endl << std::endl;

    DoResponseCallback();

    if (DisconnectAfter) {
        DisconnectAfter=false;
        Disconnect();
    }
}

// *****************************  Response callback **************************************
/**
 * @brief Register a global callback.
 * @param callback  ->  DGlobalCallback type function to register.
 */
void DRESTClient::SetOnResponse(DGlobalCallbackResponse callback)
{
        GlobalCallbackResponse=callback;
        MemberCallbackResponse=nullptr;
        MemberCalbackObj=nullptr;
}

/**
 * @brief Register a class member callback.
 * @param callback  ->  DMemberCallback type function to register (e.g. ClassName::CallbackFunc).
 * @param ClassObj  ->  Class pointer in which callback is called.
 */
void DRESTClient::SetOnResponse(DMemberCallbackResponse callback, void *ClassObj)
{
		GlobalCallbackResponse=nullptr;
		MemberCallbackResponse=callback;
		MemberCalbackObj=ClassObj;
}

void DRESTClient::DoResponseCallback(void)
{
	if(GlobalCallbackResponse != NULL) {
		GlobalCallbackResponse(HttpResponse);
	}
	else if((MemberCallbackResponse != NULL) && (MemberCalbackObj != NULL)) {
		MemberCallbackResponse(MemberCalbackObj,HttpResponse);
	}
}
// ***************************************************************************************

// *******************************  Log callbacks ****************************************
/**
 * @brief Register a global callback.
 * @param callback  ->  DGlobalCallback type function to register.
 */
void DRESTClient::SetOnLog(DGlobalCallbackLog callback)
{
        GlobalCallbackLog=callback;
        MemberCallbackLog=nullptr;
        MemberCalbackObj=nullptr;
}

/**
 * @brief Register a class member callback.
 * @param callback  ->  DMemberCallback type function to register (e.g. ClassName::CallbackFunc).
 * @param ClassObj  ->  Class pointer in which callback is called.
 */
void DRESTClient::SetOnLog(DMemberCallbackLog callback, void *ClassObj)
{
		GlobalCallbackLog=nullptr;
		MemberCallbackLog=callback;
		MemberCalbackObj=ClassObj;
}

void DRESTClient::DoLogCallback(void)
{
	if(GlobalCallbackLog != NULL) {
		GlobalCallbackLog(LastStrStatus);
	}
	else if((MemberCallbackLog != NULL) && (MemberCalbackObj != NULL)) {
		MemberCallbackLog(MemberCalbackObj,LastStrStatus);
	}
}
// ***************************************************************************************

// ******************************* Error callbacks ***************************************
/**
 * @brief Register an error global callback.
 * @param callback  ->  DGlobalCallback type function to register.
 */
void DRESTClient::SetOnError(DGlobalCallbackError callback)
{
        GlobalCallbackError=callback;
        MemberCallbackError=nullptr;
        MemberCalbackObj=nullptr;
}

/**
 * @brief Register a class member callback.
 * @param callback  ->  DMemberCallback type function to register (e.g. ClassName::CallbackFunc).
 * @param ClassObj  ->  Class pointer in which callback is called.
 */
void DRESTClient::SetOnError(DMemberCallbackError callback, void *ClassObj)
{
		GlobalCallbackError=nullptr;
		MemberCallbackError=callback;
		MemberCalbackObj=ClassObj;
}

void DRESTClient::DoErrorCallback(void)
{
	if(GlobalCallbackError != NULL) {
		GlobalCallbackError(LastStrStatus);
	}
	else if((MemberCallbackError != NULL) && (MemberCalbackObj != NULL)) {
		MemberCallbackError(MemberCalbackObj,LastStrStatus);
	}
}
// ***************************************************************************************


// ****************************  Log/error functions *************************************
/**
* @brief Set LastStrStatus and make log callback.
* @param LogMsg	->  Message to log.
*
* N.B. If LogMsg is empty, callback with LastStrStatus is performed.
**/
void DRESTClient::Log(std::string LogMsg)
{
	if (!LogMsg.empty()) {
		LastStrStatus=LogMsg;
	}
	DoLogCallback();
}

/**
* @brief Set LastStrStatus and make error callback.
* @param ErrorMsg	->  Error message.
*
* N.B. If LogMsg is empty, callback with LastStrStatus is performed.
**/
void DRESTClient::Error(std::string ErrorMsg)
{
	if (!ErrorMsg.empty()) {
		LastStrStatus=ErrorMsg;
	}
	DoErrorCallback();
}

//! @return LastStrStatus string.
std::string DRESTClient::GetLastStatus(void)
{
	return(LastStrStatus);
}
// ***************************************************************************************

/*
void RESTClient::createGetRequest(char const* host, char const* target, int version) {
    m_httpRequest.version(version);
    m_httpRequest.method(http::verb::get);
    m_httpRequest.target(target);
    m_httpRequest.set(http::field::host, host);
    m_httpRequest.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
}


std::string RESTClient::createBody() {
    pt::ptree tree;
    pt::json_parser::read_json("test.json",tree);
    std::basic_stringstream<char> jsonStream;
    pt::json_parser::write_json(jsonStream, tree, false);
    std::cout << "json stream :" << jsonStream.str() << std::endl;
    return jsonStream.str();
}
*/

} // DNetwork
} // DTools
