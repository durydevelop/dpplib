#ifndef DRESTClientH
#define DRESTClientH

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include "libdpp/network/DUri.h"

/*
The most common type are:

Type application
    application/java-archive
    application/EDI-X12
    application/EDIFACT
    application/javascript
    application/octet-stream
    application/ogg
    application/pdf
    application/xhtml+xml
    application/x-shockwave-flash
    application/json
    application/ld+json
    application/xml
    application/zip
+   application/x-www-form-urlencoded

Type audio
    audio/mpeg
    audio/x-ms-wma
    audio/vnd.rn-realaudio
    audio/x-wav

Type image
    image/gif
    image/jpeg
    image/png
    image/tiff
    image/vnd.microsoft.icon
    image/x-icon
    image/vnd.djvu
    image/svg+xml

Type multipart
    multipart/mixed
    multipart/alternative
    multipart/related (using by MHTML (HTML mail).)
    multipart/form-data

Type text
    text/css
    text/csv
    text/html
    text/javascript (obsolete)
+   text/plain
    text/xml

Type video
    video/mpeg
    video/mp4
    video/quicktime
    video/x-ms-wmv
    video/x-msvideo
    video/x-flv
    video/webm
Type vnd :
    application/vnd.android.package-archive
    application/vnd.oasis.opendocument.text
    application/vnd.oasis.opendocument.spreadsheet
    application/vnd.oasis.opendocument.presentation
    application/vnd.oasis.opendocument.graphics
    application/vnd.ms-excel
    application/vnd.openxmlformats-officedocument.spreadsheetml.sheet
    application/vnd.ms-powerpoint
    application/vnd.openxmlformats-officedocument.presentationml.presentation
    application/msword
    application/vnd.openxmlformats-officedocument.wordprocessingml.document
    application/vnd.mozilla.xul+xml
*/

namespace DTools {
namespace DNetwork {
class DRESTClient : public std::enable_shared_from_this<DRESTClient> {
    public:
        // Enumerates
        enum DHttpVersion {HTTP_1_0 =   10,
                           HTTP_1_1 =   11
        };
        enum DContentType {CONTENT_TYPE_NONE, CONTENT_TYPE_TEXT_PLAIN, CONTENT_TYPE_URL_ENCODED};
        enum DEncodeType  {ENCODE_USER_INFO, ENCODE_HOST, ENCODE_PATH, ENCODE_QUERY, ENCODE_FRAGMENT, ENCODE_ALL, ENCODE_NONE};
        // Redefined types
        typedef boost::beast::http::response<boost::beast::http::string_body> DHttpResponse;
        typedef boost::beast::http::request<boost::beast::http::string_body> DHttpRequest;
        typedef boost::beast::http::verb DRequestType;
        // Constants
        const DRequestType REQ_POST=DRequestType::post;
        const DRequestType REQ_PUT=DRequestType::put;
        const DRequestType REQ_GET=DRequestType::get;
        const DRequestType REQ_DELETE=DRequestType::delete_;

        // Functions
        explicit DRESTClient(boost::asio::io_context& ioc);
        void SetTimeout(uint8_t Sec);
        uint8_t GetTimeout(void);
        void SetUrl(const std::string Url);
        void SetHttpVersion(DHttpVersion dHttpVersion);
        void SetContentType(DContentType dContentType);
        void SetEncodeType(DEncodeType dEncodeType);
        void SetKeepAlive(bool Enable);
        DEncodeType GetEncodeType(void);
        DContentType GetContentType(void);
        DHttpRequest GetHttpRequest(DRequestType dRequestType);
        bool GetKeepAlive(void);
        void SetReqHdrBodyParams(std::map<std::string,std::string> Params);
        void SetReqBodyParams(std::map<std::string,std::string> Params);
        void AddReqHdrBodyParam(std::string Key, std::string Value);
        void AddReqBodyParam(std::string Key, std::string Value);
        void ClearHrdBodyParams(void);
        void ClearBodyParams(void);
        void Clear(void);
        DUri& GetUri(void);
        bool Connect(bool Force = false);
        void AsyncConnect(bool Force);
        bool Disconnect(bool Force = false);
        void SendPOST(void);
        void SendPUT(void);
        void SendGET(void);
        void SendDELETE(void);

    private:
        size_t GetReqHdrBodyAsString(std::string &ResultStr);
        size_t GetReqHdrBodyAsString(std::string &ResultStr, DEncodeType dEncodeType);
        std::string GetReqHdrBodyAsString(DEncodeType dEncodeType = ENCODE_NONE);
        size_t GetReqBodyAsString(std::string &ResultStr);
        size_t GetReqBodyAsString(std::string &ResultStr, DEncodeType EncodeType);
        std::string GetReqBodyAsString(DEncodeType dEncodeType = ENCODE_NONE);
        size_t Encode(std::string& Content, std::string& ResultStr, DEncodeType dEncodeType);
        std::string Encode(std::string& Content, DEncodeType dEncodeType);
        void PrepareHttpRequest(DRequestType dRequestType);
        void Send(void);
        void AsyncWrite(void);
        void AsyncRead(void);
        void OnWrite(boost::beast::error_code ec, std::size_t bytes_transferred);
        void OnRead(boost::beast::error_code ec, std::size_t bytes_transferred);

        bool Connected;
        boost::asio::ip::tcp::resolver TcpResolver;
        boost::beast::tcp_stream TcpStream;

        bool DisconnectAfter;

        uint8_t TimeoutSec;
        std::string UserAgent;
        DHttpVersion HttpVersion;
        DContentType ContentType;
        DEncodeType EncodeType;
        bool KeepAlive;
        DUri dUri;
        boost::beast::flat_buffer Buffer;
        DHttpRequest HttpRequest;
        DHttpResponse HttpResponse;
        std::map<std::string,std::string> HttpReqHdrBodyParams;
        std::map<std::string,std::string> HttpReqBodyParams;

    private:
        struct DContent {
                DContentType Type;
                std::string Verb;
        };
        inline static const struct DContent DContents[] = {
            {   CONTENT_TYPE_NONE           ,   ""                                  },
            {   CONTENT_TYPE_TEXT_PLAIN     ,   "text/plain"                        },
            {   CONTENT_TYPE_URL_ENCODED    ,   "application/x-www-form-urlencoded" },
        };

    // Callback Stuffs
    public:
        // Log
        typedef std::function<void (std::string)> DGlobalCallbackLog;
        typedef std::function<void (void*, std::string)> DMemberCallbackLog;
        void SetOnLog(DGlobalCallbackLog callback);
        void SetOnLog(DMemberCallbackLog callback, void *ClassObj);
        std::string GetLastStatus(void);
        // Response
        typedef std::function<void (DHttpResponse&)> DGlobalCallbackResponse;
        typedef std::function<void (void*,DHttpResponse&)> DMemberCallbackResponse;
        void SetOnResponse(DGlobalCallbackResponse callback);
        void SetOnResponse(DMemberCallbackResponse callback, void *ClassObj);
        // Error
        typedef DGlobalCallbackLog DGlobalCallbackError;
        typedef DMemberCallbackLog DMemberCallbackError;
        void SetOnError(DGlobalCallbackError callback);
        void SetOnError(DMemberCallbackError callback, void *ClassObj);
    private:
        void* MemberCalbackObj;
        // Log
        DGlobalCallbackLog GlobalCallbackLog;
        DMemberCallbackLog MemberCallbackLog;
        void DoLogCallback(void);
        void Log(std::string);
        std::string LastStrStatus;
        // Response
        DGlobalCallbackResponse GlobalCallbackResponse;
        DMemberCallbackResponse MemberCallbackResponse;
        void DoResponseCallback(void);
        // Error
        DGlobalCallbackError GlobalCallbackError;
        DMemberCallbackError MemberCallbackError;
        void DoErrorCallback(void);
        void Error(std::string);
};

} // DNetwork
} // DTools
#endif
