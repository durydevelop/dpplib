#ifndef DUdpServerH
#define DUdpServerH

//#include <cstdlib>
//#include <iostream>
//#include <asio/ts/buffer.hpp>
//#include <asio/ts/internet.hpp>
#include <asio.hpp>

class DUdpServer {
    public:
        DUdpServer(asio::io_context& io_context, short port);
        void do_receive();
        void do_send(std::size_t length);

    private:
        asio::ip::udp::socket socket_;
        asio::ip::udp::endpoint sender_endpoint_;
        enum { max_length = 1024 };
        char data_[max_length];
};

#endif // DUdpServer
