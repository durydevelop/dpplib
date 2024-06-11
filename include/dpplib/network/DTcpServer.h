/*
* from https://stackoverflow.com/questions/7754695/boost-asio-async-write-how-to-not-interleaving-async-write-calls
*/

#ifndef DTcpServerH
#define DTcpServerH

#include <boost/asio.hpp>
#include <unordered_set>
#include <chrono>
#include <deque>

class DTcpServer {
    public:
        class Connection : public std::enable_shared_from_this<Connection> {
            friend class DTcpServer;
            void DoRead();
            void DoWrite();
            bool IsWriting() const;
            void Start();
            void Close();
            void Shutdown();

            DTcpServer& server_;
            uint8_t* RecvBuff;
            size_t BuffLen;
            //asio::streambuf buf_in_;
            std::mutex buffers_mtx_;
            //std::vector<std::vector<uint8_t>> buffers_[2]; // a double buffer
            std::vector<std::vector<uint8_t>> buffers_[2]; // a double buffer
            std::vector<boost::asio::const_buffer> buffer_seq_;
            std::vector<boost::asio::const_buffer> Buffers[2]; // a double buffer
            std::vector<boost::asio::const_buffer> BufferSeq;
            int active_buffer_ = 0;
            bool closing_ = false;
            bool closed_ = false;
            boost::asio::steady_timer read_timer;
            boost::asio::steady_timer write_timer;
            boost::asio::ip::tcp::socket Socket;
            size_t ID;
            bool wr;

        public:
            Connection(DTcpServer& server, size_t ConnectionID=0);
            ~Connection();
            void Write(std::string data);
            void Write(const uint8_t data[], const size_t len);
            void Write(boost::asio::const_buffer);
            boost::asio::ip::tcp::endpoint RemoteEndpoint() const;
            size_t GetID();
        };

    private:
        typedef void (*DCallbackErr)(const std::shared_ptr<DTcpServer::Connection>, const std::error_code&);
        typedef void (*DCallbackConn)(const std::shared_ptr<DTcpServer::Connection>);
        typedef void (*DCallbackData)(const std::shared_ptr<DTcpServer::Connection>, const uint8_t& Data, const size_t Size);
        typedef void (*DCallbackClosed)(const size_t ConnectionID);

        void DoAccept();
        void DoCallbackError(const std::shared_ptr<DTcpServer::Connection> connection, const std::error_code& ec);
        void DoCallbackAccept(const std::shared_ptr<DTcpServer::Connection> connection);

        boost::asio::io_service io_service_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::unordered_set<std::shared_ptr<Connection>> active_connections_;
        uint8_t ReadTimeoutSec;
        uint8_t WriteTimeoutSec;
        size_t LastConnectionID;

        DCallbackErr OnError;
        DCallbackConn OnAccept;
        DCallbackData OnDataRecvd;
        DCallbackClosed OnClosed;

    public:
        enum TimeoutSec {TIMEOUT_DISABLE=0};
        void SetOnError(DCallbackErr Callback);
        void SetOnAccept(DCallbackConn Callback);
        void SetOnDataRecvd(DCallbackData Callback);
        void SetOnClosed(DCallbackClosed Callback);
        void SetReadTimeout(uint8_t TimeoutSec);
        void SetWriteTimeout(uint8_t TimeoutSec);
        void SetTimeout(uint8_t TimeoutSec);
        DTcpServer(int port, uint8_t timeout_read=TIMEOUT_DISABLE, uint8_t timeout_write=TIMEOUT_DISABLE);
        void Run();
        void Stop();

};

#endif
