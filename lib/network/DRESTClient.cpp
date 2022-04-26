#include "libdpp/network/DRESTClient.h"
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


//#include <iostream>

namespace asio  =   boost::asio;
namespace beast =   boost::beast;
namespace http  =   beast::http;
namespace pt    =   boost::property_tree;
using     tcp   =   boost::asio::ip::tcp;

#define DEFAULT_HTTP_VERSION    HTTP_1_1
#define DEFAULT_CONTENT_TYPE    CONTENT_TYPE_NONE
#define DEFAULT_ENCODE          ENCODE_NONE
#define DEFAULT_TIMEOUT_SEC     30
#define DEFAULT_USER_AGENT      "DRESTClient/0.1"
//#define DEFAULT_USER_AGENT BOOST_BEAST_VERSION_STRING

#define DEBUG

/** TODO:
 *  OK_Connect(std::string Url)
 *  OK_Blocking Send e Read
 *  _Async Send e Read
 *  _OnAsyncConnect
 *  OK_Aggiornare callbacks con std::funcion
 **/

namespace DTools {
namespace DNetwork {

// ***************************************************************************************
// ***************************** DRESTClient class ***************************************
// ***************************************************************************************

/**
 * @brief DRESTClient::DRESTClient constructor.
 */
DRESTClient::DRESTClient() :
    ioc(),
    TcpResolver(asio::make_strand(ioc)),
    TcpStream(asio::make_strand(ioc))
{
    // Keep io_context alive
    //boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard(boost::asio::make_work_guard(ioc));
    Connected=false;
    //DisconnectAfter=false;
    TimeoutSec=DEFAULT_TIMEOUT_SEC;
    UserAgent=DEFAULT_USER_AGENT;
    HttpVersion=DEFAULT_HTTP_VERSION;
    HttpReqContentTypeStr=DContents[DEFAULT_CONTENT_TYPE].Verb;
    HttpReqEncodeType=DEFAULT_ENCODE;
    HttpReqKeepAlive=true;
    CallbackLog=nullptr;
    CallbackResponse=nullptr;
    CallbackError=nullptr;
}

/**
 * @brief DRESTClient::DRESTClient
 * @param io_context
 * Connect() call is made on first Send()
 */
/*
DRESTClient::DRESTClient(boost::asio::io_context& io_context) :
    TcpResolver(asio::make_strand(io_context)),
    TcpStream(asio::make_strand(io_context))
{
    Connected=false;
    DisconnectAfter=false;
    TimeoutSec=DEFAULT_TIMEOUT_SEC;
    UserAgent=DEFAULT_USER_AGENT;
    HttpVersion=DEFAULT_HTTP_VERSION;
    HttpReqContentTypeStr=DContents[DEFAULT_CONTENT_TYPE].Verb;
    HttpReqEncodeType=DEFAULT_ENCODE;
    HttpReqKeepAlive=true;
    CallbackLog=nullptr;
    CallbackResponse=nullptr;
    CallbackError=nullptr;
}
*/
/**
 * @brief Set version of http protocol.
 * @param dHttpVersion   ->  Can be HTTP_1_0 or HTTP_1_1.
 */
void DRESTClient::SetHttpVersion(DRESTClient::DHttpVersion dHttpVersion)
{
    HttpVersion=dHttpVersion;
}

/**
 * @brief Set version of http protocol directly as string.
 * @param HttpVersionString   ->  Version string, can be "1.0" or "1.1".
 * N.B. If HttpVersionString is not "1.0" HTTP_1_1 is set.
 */
void DRESTClient::SetHttpVersion(std::string HttpVersionString)
{
    HttpVersion=HttpVersionString == "1.0" ? HTTP_1_0 : HTTP_1_1;
}

/**
 * @brief Set content-type header field.
 * @param dContentType  ->  Can be one of DContentType enumarator.
 */
void DRESTClient::SetHttpReqContentType(DRESTClient::DContentType dContentType)
{
    HttpReqContentTypeStr=DContents[dContentType].Verb;
}

/**
 * @brief Set content-type header field directly as string.
 * @param ContentTypeStr  ->  Sting containing a valid http content type.
 */
void DRESTClient::SetHttpReqContentType(std::string ContentTypeString)
{
    HttpReqContentTypeStr=ContentTypeString;
}

/**
 * @brief Set encode-type header field.
 * @param dEncodeType  ->  Can be one of DContentType enumarator.
 */
void DRESTClient::SetHttpReqEncodeType(DRESTClient::DEncodeType dEncodeType)
{
    HttpReqEncodeType=dEncodeType;
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
 * @brief Set client request url.
 * @param Url   ->  REST service url.
 * @return false if url is not valid, otherwise true.
 */
bool DRESTClient::SetUrl(const std::string Url)
{
    if (!dUri.Set(Url)) {
        LastError=dUri.GetLastStatus();
        return false;
    }
    return true;
}

/**
 * @brief Set keep-alive option in HttpRequest.
 * @param Enabled   ->  It true "in http header "Connection: keep-alive" is add to header.
 */
void DRESTClient::SetHttpReqKeepAlive(bool Enabled)
{
    HttpReqKeepAlive=Enabled;
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
 * @brief Fill header body with a string.
 * @param BodyStr   ->  Header body content as string.
 */
void DRESTClient::SetHdrBody(std::string HdrBodyStr)
{
    HdrBody.assign(HdrBodyStr);
}

/**
 * @brief Fill body with a string.
 * @param BodyStr   ->  Header body content as string.
 */
void DRESTClient::SetBody(std::string BodyStr)
{
    Body.assign(BodyStr);
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
void DRESTClient::ClearHrdBody(void)
{
    HttpReqHdrBodyParams.clear();
    HdrBody.clear();
}

/**
 * @brief Clear request body.
 */
void DRESTClient::ClearBody(void)
{
    HttpReqBodyParams.clear();
    Body.clear();
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
    HdrBody.clear();
    Body.clear();
    Connected=false;
    //DisconnectAfter=false;
    TimeoutSec=DEFAULT_TIMEOUT_SEC;
    UserAgent=DEFAULT_USER_AGENT;
    HttpVersion=DEFAULT_HTTP_VERSION;
    HttpReqContentTypeStr=DContents[DEFAULT_CONTENT_TYPE].Verb;
    HttpReqEncodeType=DEFAULT_ENCODE;
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
DRESTClient::DEncodeType DRESTClient::GetHttpReqEncodeType(void)
{
    return(HttpReqEncodeType);
}

/**
 * @return current content-type as DContentType
 */
std::string DRESTClient::GetHttpReqContentTypeStr(void)
{
    return(HttpReqContentTypeStr);
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
 * @param ResultStr ->  String where to put body.
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
bool DRESTClient::GetHttpReqKeepAlive(void)
{
    return HttpReqKeepAlive;
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
            Error("EncodeType not supported");
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
    // TODO: per ora non va HttpRequest.keep_alive(HttpReqKeepAlive);
    // aggiungo direttamente
    if (HttpReqKeepAlive) {
        HttpRequest.set("Connection","Keep-Alive");
    }
    // Content type
    HttpRequest.set(http::field::content_type,HttpReqContentTypeStr);
    // Header body
    if (HttpReqHdrBodyParams.size() > 0) {
        HttpRequest.set(http::field::body,GetReqHdrBodyAsString(HttpReqEncodeType));
    }
    else if (!HdrBody.empty()) {
        HttpRequest.set(http::field::body,HdrBody);
    }
    // Body
    if (HttpReqBodyParams.size() > 0) {
        HttpRequest.body()=GetReqBodyAsString(HttpReqEncodeType);
    }
    else if (!Body.empty()) {
        HttpRequest.body()=Body;
    }
    // Build request
    HttpRequest.prepare_payload();
}

/**
 * @brief Perform a sync (blocking) connection.
 * @param Url   ->  REST service url.
 * @param Force ->  If true, force ri-connection in case of Connected variable is true.
 * @return true on success connection, otherwise false. You can retrive error from LastError string.
 * This function is blocking until connection hand-shake is ended or time-out is reached.
 */
bool DRESTClient::Connect(const std::string& Url, bool Force)
{
    SetUrl(Url);
    return(Connect(Force));
}
/**
 * @brief Perform a sync (blocking) connection to url previously set with SetUrl(...).
 * @param Force ->  If true, force ri-connection in case of Connected variable is true.
 * @return true on success connection, otherwise false. Can retrive error from LastError string.
 * This function is blocking until connection hand-shake is ended or time-out is reached.
 */
bool DRESTClient::Connect(bool Force)
{
    if (Connected && !Force) {
        Error("Already connected");
        return false;
    }
    else if (dUri.Host.empty()) {
        Error("Missing host in url");
        return false;
    }
    else if (dUri.Port.empty()) {
        Error("Missing port in url");
        return false;
    }

    Connected=false;

    beast::error_code ec;
    // Sync resolve
    asio::ip::tcp::resolver::results_type results=TcpResolver.resolve(dUri.Host,dUri.Port,ec);
    if (ec) {
        Error("Error resolve: "+ec.message());
        return false;
    }
    // Sync connect
    TcpStream.expires_after(std::chrono::seconds(TimeoutSec));
    TcpStream.connect(results,ec);
    if (ec) {
        Error("Error connect: "+ec.message());
        return false;
    }
    Connected=true;
    return true;
}

/**
 * @brief Perform a sync (blocking) diconnect.
 * @param Force ->  If true force disconnection in case of Connected variable is false.
 * @return true on success connection, otherwise false. Can retrive error from LastError string.
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
        Error("Error shutdown: "+ec.message());
        return false;
    }
    Log("Disconnected");
    Connected=false;
    return true;
}

/**
 * @brief Send a blocking POST using current data stored.
 * @param WaitForResponse   ->  If true, blocking funcion Read() is called and the funcion return after data received.
 * @return true on success otherwise false. You can retrive error from LastError string.
 */
bool DRESTClient::SendPOST(bool WaitForResponse)
{
    bool DisconnectAfterSent=false;

    if (!Connected) {
        if (!Connect()) {
            //DisconnectAfter=true;
            Error("Connection failed:");
            return false;
        }
        // If were not connected, disconnect after sent.
        DisconnectAfterSent=true;
    }
    else if (dUri.Host.empty()) {
        Error("Missing host in url");
        return false;
    }

    PrepareHttpRequest(REQ_POST);
    bool Ret=Send();
    if (Ret) {
        if (WaitForResponse) {
            Ret=Read();
        }
    }

    if (DisconnectAfterSent) {
        Disconnect();
    }

    return(Ret);
}

/**
 * @brief Send a blocking PUT using current data stored.
 * @param WaitForResponse   ->  If true, blocking funcion Read() is called and the funcion return after data received.
 * @return true on success otherwise false. You can retrive error from LastError string.
 */
bool DRESTClient::SendPUT(bool WaitForResponse)
{
    PrepareHttpRequest(REQ_PUT);
    if (Send()) {
        if (WaitForResponse) {
            return(Read());
        }
    }
    return false;
}

/**
 * @brief Send a blocking GET using current data stored.
 * @param WaitForResponse   ->  If true, blocking funcion Read() is called and the funcion return after data received.
 * @return true on success otherwise false. You can retrive error from LastError string.
 */
bool DRESTClient::SendGET(bool WaitForResponse)
{
    PrepareHttpRequest(REQ_GET);
    if (Send()) {
        if (WaitForResponse) {
            return(Read());
        }
    }
    return false;
}

/**
 * @brief Perform a blocking send of the current HttpRequest.
 * @return true on success otherwise false. You can retrive error from LastError string.
 */
bool DRESTClient::Send(void)
{
    if (dUri.Host.empty()) {
        Error("Missing host in url");
        return false;
    }
/*
    if (!Connected) {
        if (!Connect()) {
            //DisconnectAfter=true;
            Error("Connection failed:");
            return false;
        }
        // If were not connected, disconnect after sent.
        DisconnectAfterSent=true;
    }
    else if (dUri.Host.empty()) {
        Error("Missing host in url");
        return false;
    }
*/
    HttpResponse.clear();

    // Send request
    bool Ret=Write();

    return(Ret);
}

/**
 * @brief Perform the real socket write (blocking) of the current HttpRequest.
 * The function return only when all data are sent.
 */
bool DRESTClient::Write(void)
{
    #ifdef DEBUG
        std::stringstream ss;
        ss << std::endl << "HttpRequest:" << std::endl;
        ss << HttpRequest << std::endl;
        Log("Writing...");
        Log(ss.str());
    #endif

    TcpStream.expires_after(std::chrono::seconds(TimeoutSec));

    beast::error_code ec;
    size_t BytesWritten=http::write(TcpStream,HttpRequest,ec);

    #ifdef DEBUG
        Log("Written "+std::to_string(BytesWritten)+" Bytes");
    #endif

    if (ec) {
        Error("Write failed: "+ec.message());
        return false;
    }
    return true;
}

/**
 * @brief Make a blocking read.
 * @return true on success read otherwise false. You can retrive error from LastError string.
 * Data received are stored in HttpResponse.
 */
bool DRESTClient::Read(void)
{
    boost::beast::error_code ec;

    size_t BytesRead=http::read(TcpStream,Buffer,HttpResponse,ec);
    #ifdef DEBUG
        Log("Read "+std::to_string(BytesRead)+" Bytes");
    #endif
    if (ec) {
        Error("Read failed: "+ec.message());
        return false;
    }

    DoResponseCallback();
    return true;
}

// *****************************  Response callback **************************************
/**
 * @brief Register the global callback.
 * @param callback  ->  DCallbackResponse type function to register.
 * For thread safety, callback should be set in this way:
 * @code
 * auto callback=std::bind(&MainWindow::Callback,this,std::placeholders::_1);
 * RestClient.SetOnResponse(callback);
 * @endcode
 */
void DRESTClient::SetOnResponse(DCallbackResponse callback)
{
    CallbackResponse=callback;
}

void DRESTClient::DoResponseCallback(void)
{
	if (CallbackResponse) {
		CallbackResponse(HttpResponse);
	}
}
// ***************************************************************************************

// *******************************  Log callbacks ****************************************
/**
 * @brief Register the log callback.
 * @param callback  ->  DGlobalCallback type function to register.
 * For thread safety, callback should be set in this way:
 * @code
 * auto callback=std::bind(&MainWindow::Callback,this,std::placeholders::_1);
 * RestClient.SetOnLog(callback);
 * @endcode
 */
void DRESTClient::SetOnLog(DCallbackLog callback)
{
    CallbackLog=callback;
}

void DRESTClient::DoLogCallback(void)
{
	if (CallbackLog) {
		CallbackLog(LastError);
	}
}
// ***************************************************************************************

// ******************************* Error callbacks ***************************************
/**
 * @brief Register the error callback.
 * @param callback  ->  DCallbackError type function to register.
 * For thread safety, callback should be set in this way:
 * @code
 * auto callback=std::bind(&MainWindow::Callback,this,std::placeholders::_1);
 * RestClient.SetOnError(callback);
 * @endcode
 */
void DRESTClient::SetOnError(DCallbackError callback)
{
    CallbackError=callback;
}

void DRESTClient::DoErrorCallback(void)
{
	if (CallbackError) {
		CallbackError(LastError);
	}
}
// ***************************************************************************************


// ****************************  Log/error functions *************************************
/**
* @brief Set LastError and make log callback.
* @param LogMsg	->  Message to log.
*
* N.B. If LogMsg is empty, callback with LastError is performed.
**/
void DRESTClient::Log(std::string LogMsg)
{
	if (!LogMsg.empty()) {
		LastError=LogMsg;
	}
	DoLogCallback();
}

/**
* @brief Set LastError and make error callback.
* @param ErrorMsg	->  Error message.
*
* N.B. If LogMsg is empty, callback with LastError is performed.
**/
void DRESTClient::Error(std::string ErrorMsg)
{
	if (!ErrorMsg.empty()) {
		LastError=ErrorMsg;
	}
	DoErrorCallback();
}

//! @return LastError string.
std::string DRESTClient::GetLastError(void)
{
	return(LastError);
}
// ***************************************************************************************

} // DNetwork
} // DTools
