#ifndef DUdpServerH
#define DUdpServerH

#include <asio.hpp>
#include <memory>
#include <iostream>
#include <functional>

class DUdpServer; // forward declaration

struct DUdpSession : std::enable_shared_from_this<DUdpSession> {

    DUdpSession(DUdpServer* server);
    void handle_request(const std::error_code& ec, std::size_t BytesRecvd);
    void handle_sent(const std::error_code& ec, std::size_t BytesSent);

    asio::ip::udp::endpoint RemoteEndpoint;
    uint8_t *RecvBuffer;
    uint8_t *SendBuffer;
    DUdpServer* Server;
    size_t BufferSize;
};

class DUdpServer
{
    public:
        typedef std::shared_ptr<DUdpSession> shared_session;
        typedef void (*DCallbackData)(shared_session session, uint8_t*, size_t);
        typedef void (*DCallbackErr)(shared_session session, std::string);

        DUdpServer(asio::io_service& io_service);
        void Run();
        void SetOnDataRecvd(DCallbackData callback);
        void SetOnDataSent(DCallbackData callback);
		void SetOnError(DCallbackErr callback);
		void DoCallbackDataRecvd(shared_session session, uint8_t *data, size_t len);
		void DoCallbackDataSent(shared_session session, uint8_t *data, size_t len);
		void DoCallbackError(shared_session session, std::string Error);
		void Send(shared_session session, uint8_t *data, size_t len);

    private:
        void StartReceiveSession();
        void handle_receive(shared_session session, const std::error_code& ec, std::size_t bytes_recvd);
        void PostResponse(shared_session const& session, size_t len);
        void handle_response(shared_session session, const std::error_code& ec, std::size_t BytesRecvd);

        asio::ip::udp::socket  socket_;
        asio::io_service::strand strand_;
        asio::io_service *ioservice;
        DCallbackData OnDataRecvd;
        DCallbackData OnDataSent;
        DCallbackErr OnError;

        friend struct DUdpSession;
};

#endif // DUdpServer

