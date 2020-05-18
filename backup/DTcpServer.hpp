#ifndef DTcpServerH
#define DTcpServerH

#include <asio.hpp>

class DTcpSession : public std::enable_shared_from_this<DTcpSession> {
    public:
        DTcpSession(asio::ip::tcp::socket socket);
        void start();

    private:
        void do_read();
        void do_write(std::size_t length);

        asio::ip::tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
};

class DTcpServer {
    public:
        DTcpServer(asio::io_context& io_context, short port);

    private:
        void do_accept();

        asio::ip::tcp::acceptor acceptor_;
        asio::ip::tcp::socket socket_;
};
#endif
